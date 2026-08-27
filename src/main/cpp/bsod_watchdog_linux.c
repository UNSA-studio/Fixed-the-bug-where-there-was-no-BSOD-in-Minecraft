/*
 * BSOD watchdog + X11 overlay for Linux, in one dependency-free binary.
 *
 * argv: [1] = game pid (decimal)   [2] = restart script path
 *                          [3] = game dir (where hs_err files appear)
 *
 * ZERO intrusion into the game: no signals, no ptrace, no exception hooks.
 * We just poll:
 *   - is the game process still alive?
 *   - did a new hs_err_pid*.log appear in the game dir?
 *
 * When the game dies WITH an hs_err file = native crash -> show the overlay
 * exactly over the (dead) game window, wait, relaunch via the restart script.
 * When the game dies WITHOUT hs_err = normal exit -> do nothing, quit.
 *
 * The overlay is a plain X11 window (no override-redirect, no always-on-top):
 * draggable, lives inside the old game window rectangle, blocks nothing.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/* QR for https://www.minecraft.net, 25x25, ECC M. */
static const int QR_SIZE = 25;
static const unsigned char QR_BITS[25][25] = {
    {1,1,1,1,1,1,1,0,0,1,1,1,0,0,1,0,1,0,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1,1,0,1,0,0,0,0,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0,1,0,1,1,1,0,1},
    {1,0,1,1,1,0,1,0,1,0,1,1,0,1,1,1,0,0,1,0,1,1,1,0,1},
    {1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,0,1,0,1,0,1,1,1,0,1},
    {1,0,0,0,0,0,1,0,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,0,0,0,0,0,0},
    {1,0,1,1,1,1,1,0,0,1,0,1,0,1,1,1,0,0,1,1,1,1,1,0,0},
    {0,0,1,1,1,1,0,1,0,1,1,1,0,0,0,0,1,1,0,1,0,0,0,1,0},
    {1,1,0,0,1,1,1,1,0,0,0,0,0,1,1,1,0,1,1,1,1,1,0,1,1},
    {1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,1,1,1,1,0,1,1,1},
    {1,0,0,0,0,1,0,1,1,0,1,0,0,0,1,0,1,1,0,1,0,1,0,1,0},
    {1,0,0,0,0,1,1,1,0,0,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1},
    {1,0,0,1,1,0,0,0,0,0,0,1,0,0,0,1,0,1,0,1,1,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,0,1,1,1,0,0,1,1,1,1,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,0,1,1,0,1,1,0,0,0,1,1,0,0,0},
    {1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,0,1,0,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,1,0,0,0,1,1,0,1,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,0,1,0,0},
    {1,0,1,1,1,0,1,0,1,0,1,1,0,1,0,0,0,0,1,0,1,1,1,1,1},
    {1,0,1,1,1,0,1,0,1,0,1,0,0,1,1,0,1,0,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,0,1},
    {1,1,1,1,1,1,1,0,1,0,1,0,1,1,1,0,0,0,0,1,1,1,1,1,1},
};

static int  g_percent = 0;
static int  g_ticks = 0;
static char g_status[128] = "";

/* Finds the largest viewable top-level window on the root (the MC window). */
static Window FindBiggestWindow(Display* dpy, Window root, int* outX, int* outY,
                                int* outW, int* outH) {
    Window target = None;
    unsigned long bestArea = 0;
    Window rWin, pWin;
    unsigned int nKids;
    Window* kids = NULL;
    if (XQueryTree(dpy, root, &rWin, &pWin, &kids, &nKids) && kids) {
        for (unsigned int i = 0; i < nKids; i++) {
            XWindowAttributes a;
            if (XGetWindowAttributes(dpy, kids[i], &a) && a.map_state == IsViewable) {
                unsigned long area = (unsigned long) a.width * a.height;
                if (area > bestArea && a.width > 240 && a.height > 160) {
                    bestArea = area;
                    target = kids[i];
                }
            }
        }
        XFree(kids);
    }
    if (target != None) {
        XWindowAttributes a;
        XGetWindowAttributes(dpy, target, &a);
        Window child;
        XTranslateCoordinates(dpy, target, root, 0, 0, outX, outY, &child);
        *outW = a.width;
        *outH = a.height;
    }
    return target;
}

/* True when the given pid no longer exists. */
static int ProcessGone(pid_t pid) {
    return kill(pid, 0) != 0;
}

/* True when an hs_err_pid*.log exists in dir. */
static int HsErrExists(const char* dir) {
    DIR* d = opendir(dir);
    if (!d) {
        return 0;
    }
    struct dirent* e;
    int found = 0;
    while ((e = readdir(d)) != NULL) {
        if (strncmp(e->d_name, "hs_err_pid", 10) == 0) {
            found = 1;
            break;
        }
    }
    closedir(d);
    return found;
}

static void RunOverlay(Display* dpy, pid_t gamePid, const char* gameDir,
                       const char* restartCmd) {
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    int x, y, w, h;

    if (FindBiggestWindow(dpy, root, &x, &y, &w, &h) == None) {
        w = DisplayWidth(dpy, scr) * 3 / 4;
        h = DisplayHeight(dpy, scr) * 3 / 4;
        x = (DisplayWidth(dpy, scr) - w) / 2;
        y = (DisplayHeight(dpy, scr) - h) / 2;
    }

    XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));
    swa.background_pixel = WhitePixel(dpy, scr);
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
                   | Button1MotionMask | ButtonReleaseMask;

    /* Plain top-level window: the WM manages it, it is draggable, not on top. */
    Window overlay = XCreateSimpleWindow(dpy, root, x, y, (unsigned) w, (unsigned) h,
                                         0, BlackPixel(dpy, scr), WhitePixel(dpy, scr));
    XMapWindow(dpy, overlay);

    Colormap cmap = DefaultColormap(dpy, scr);
    XColor blue, white;
    XParseColor(dpy, cmap, "#0078D7", &blue);
    XAllocColor(dpy, cmap, &blue);
    white.pixel = WhitePixel(dpy, scr);
    XSetWindowBackground(dpy, overlay, blue.pixel);
    XStoreName(dpy, overlay, "BSOD");
    XClearWindow(dpy, overlay);
    XFlush(dpy);

    GC gc = XCreateGC(dpy, overlay, 0, NULL);
    XSetForeground(dpy, gc, white.pixel);
    XFontStruct* bigFont = XLoadQueryFont(dpy, "-*-helvetica-bold-r-*-*-*-240-*-*-*-*-*-*");
    XFontStruct* bodyFont = XLoadQueryFont(dpy, "-*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*");

    /* Offscreen QR pixmap drawn once. */
    int qrScale = h / 300;
    if (qrScale < 2) {
        qrScale = 2;
    }
    int qrPixels = QR_SIZE * qrScale;
    Pixmap qrPix = XCreatePixmap(dpy, overlay, qrPixels, qrPixels,
                                 DefaultDepth(dpy, scr));
    XGCValues gcv;
    GC qgc = XCreateGC(dpy, qrPix, 0, NULL);
    gcv.foreground = white.pixel;
    XChangeGC(dpy, qgc, GCForeground, &gcv);
    XSetWindowBackground(dpy, qrPix, blue.pixel);
    XFillRectangle(dpy, qrPix, qgc, 0, 0, (unsigned) qrPixels, (unsigned) qrPixels);
    for (int row = 0; row < QR_SIZE; row++) {
        for (int col = 0; col < QR_SIZE; col++) {
            if (QR_BITS[row][col]) {
                XFillRectangle(dpy, qrPix, qgc,
                               col * qrScale, row * qrScale,
                               (unsigned) qrScale, (unsigned) qrScale);
            }
        }
    }
    XFreeGC(dpy, qgc);

    int running = 1;
    int dragX = 0, dragY = 0;
    int dragging = 0;
    int relaunchDone = 0;

    while (running) {
        XEvent ev;
        while (XPending(dpy)) {
            XNextEvent(dpy, &ev);
            if (ev.type == Expose) {
                XClearWindow(dpy, overlay);
            } else if (ev.type == KeyPress) {
                running = 0;
            } else if (ev.type == ButtonPress && ev.xbutton.button == Button1) {
                dragging = 1;
                dragX = ev.xbutton.x;
                dragY = ev.xbutton.y;
            } else if (ev.type == MotionNotify && dragging) {
                XMoveWindow(dpy, overlay,
                            ev.xmotion.x_root - dragX, ev.xmotion.y_root - dragY);
            } else if (ev.type == ButtonRelease && ev.xbutton.button == Button1) {
                dragging = 0;
            }
        }

        XSetWindowBackground(dpy, overlay, blue.pixel);
        XClearWindow(dpy, overlay);

        int tx = w / 12, ty = h / 8;
        if (bigFont) {
            XSetFont(dpy, gc, bigFont->fid);
            XDrawString(dpy, overlay, gc, tx, ty, ":(", 2);
            ty += h / 5;
        }
        if (bodyFont) {
            XSetFont(dpy, gc, bodyFont->fid);
            XDrawString(dpy, overlay, gc, tx, ty,
                        "Your PC ran into a problem and needs to restart.", 47);
            ty += h / 20;
            XDrawString(dpy, overlay, gc, tx, ty,
                        "We're just collecting some error info, and then we'll restart for you.", 69);
            ty += h / 12;

            char pct[32];
            snprintf(pct, sizeof(pct), "%d%% complete", g_percent);
            XDrawString(dpy, overlay, gc, tx, ty, pct, strlen(pct));
            ty += h / 9;

            if (g_status[0]) {
                XDrawString(dpy, overlay, gc, tx, ty, g_status, strlen(g_status));
                ty += h / 14;
            }

            XCopyArea(dpy, qrPix, overlay, gc, 0, 0,
                      (unsigned) qrPixels, (unsigned) qrPixels, tx, ty);
            int textX = tx + qrPixels + w / 40;
            int textY = ty + qrPixels / 2;
            XDrawString(dpy, overlay, gc, textX, textY,
                        "For more information about this issue", 37);
            textY += h / 22;
            XDrawString(dpy, overlay, gc, textX, textY,
                        "and possible fixes, visit", 25);
            textY += h / 22;
            XDrawString(dpy, overlay, gc, textX, textY,
                        "https://www.minecraft.net", 25);
        }
        XFlush(dpy);

        g_ticks++;
        if (g_percent < 100) {
            g_percent++;
            if (g_percent > 99) {
                g_percent = 100;
                strcpy(g_status, "Minecraft exited. Relaunching...");
            }
        }

        if (g_percent >= 100 && !relaunchDone && restartCmd[0]) {
            relaunchDone = 1;
            pid_t p = fork();
            if (p == 0) {
                execl("/bin/sh", "sh", restartCmd, (char*) NULL);
                _exit(127);
            }
            waitpid(p, NULL, 0);
            break;
        }

        sleep(1);
    }

    XFreePixmap(dpy, qrPix);
    XFreeGC(dpy, gc);
    if (bigFont) XFreeFont(dpy, bigFont);
    if (bodyFont) XFreeFont(dpy, bodyFont);
    (void) gamePid;
    (void) gameDir;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }
    pid_t gamePid = (pid_t) atoi(argv[1]);
    const char* restartCmd = argv[2];
    const char* gameDir = argc >= 4 ? argv[3] : ".";

    /* Wait for the game to exit. */
    while (!ProcessGone(gamePid)) {
        sleep(2);
    }

    /* No hs_err => clean exit, nothing to do. */
    if (!HsErrExists(gameDir)) {
        return 0;
    }

    /* Native crash confirmed: show the overlay. */
    Display* dpy = XOpenDisplay(NULL);
    if (!dpy) {
        return 0;
    }
    RunOverlay(dpy, gamePid, gameDir, restartCmd);
    XCloseDisplay(dpy);
    return 0;
}