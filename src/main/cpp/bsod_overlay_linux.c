/*
 * BSOD overlay for Linux/X11.
 *
 * argv: [1] = game pid (decimal)  [2] = restart command path
 *
 * Finds the biggest mapped window owned by the given pid via XQueryTree,
 * covers it with a Windows-blue override-redirect window, draws the classic
 * ":(" screen with XDrawString, waits until the game process dies, launches
 * the restart script and quits. ESC or click closes the overlay.
 *
 * Deliberately plain Xlib - no toolkit, starts in milliseconds.
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static int g_ticks = 0;
static int g_percent = 0;
static char g_status[128] = "Collecting error info...";

int main(int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }
    pid_t gamePid = (pid_t) atoi(argv[1]);
    const char* restartCmd = argc >= 3 ? argv[2] : "";

    Display* dpy = XOpenDisplay(NULL);
    if (!dpy) {
        return 1; /* headless or no X - nothing we can do */
    }
    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);

    /* Find the largest top-level window of the whole tree (the MC window). */
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

    int x, y, w, h;
    if (target != None) {
        XWindowAttributes a;
        XGetWindowAttributes(dpy, target, &a);
        Window child;
        /* Translate to root coordinates so we cover exactly where it sits. */
        XTranslateCoordinates(dpy, target, root, 0, 0, &x, &y, &child);
        w = a.width;
        h = a.height;
    } else {
        w = DisplayWidth(dpy, scr) * 3 / 4;
        h = DisplayHeight(dpy, scr) * 3 / 4;
        x = (DisplayWidth(dpy, scr) - w) / 2;
        y = (DisplayHeight(dpy, scr) - h) / 2;
    }

    XSetWindowAttributes swa;
    memset(&swa, 0, sizeof(swa));
    swa.background_pixel = WhitePixel(dpy, scr);
    swa.override_redirect = True;   /* no WM decorations, stays above */
    swa.event_mask = ExposureMask | KeyPressMask | ButtonPressMask;

    Window overlay = XCreateWindow(dpy, root, x, y, (unsigned) w, (unsigned) h,
                                   0, CopyFromParent, InputOutput, CopyFromParent,
                                   CWBackPixel | CWOverrideRedirect | CWEventMask, &swa);
    XMapWindow(dpy, overlay);

    Colormap cmap = DefaultColormap(dpy, scr);
    XColor blueExact, blue;
    XParseColor(dpy, cmap, "#0078D7", &blueExact);
    XAllocColor(dpy, cmap, &blueExact, &blue);
    XSetWindowBackground(dpy, overlay, blue.pixel);

    XStoreName(dpy, overlay, "BSOD");
    XFlush(dpy);

    GC gc = XCreateGC(dpy, overlay, 0, NULL);
    XSetForeground(dpy, gc, WhitePixel(dpy, scr));
    XFontStruct* bigFont = XLoadQueryFont(dpy, "-*-helvetica-bold-r-*-*-*-240-*-*-*-*-*-*");
    XFontStruct* bodyFont = XLoadQueryFont(dpy, "-*-helvetica-medium-r-*-*-*-140-*-*-*-*-*-*");

    volatile sig_atomic_t running = 1;
    while (running) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            if (ev.type == KeyPress || ev.type == ButtonPress) {
                running = 0;
            } else if (ev.type == Expose) {
                g_percent += g_percent < 99 ? 5 : 0;
            }
        }

        /* Is the game still alive? */
        if (kill(gamePid, 0) != 0) {
            strcpy(g_status, "Minecraft exited. Relaunching...");
            if (restartCmd[0]) {
                pid_t p = fork();
                if (p == 0) {
                    execl("/bin/sh", "sh", restartCmd, (char*) NULL);
                    _exit(127);
                }
                waitpid(p, NULL, 0);
            }
            break;
        }

        XSetWindowBackground(dpy, overlay, blue.pixel);
        XClearWindow(dpy, overlay);

        int tx = 48, ty = 64;
        if (bigFont) {
            XSetFont(dpy, gc, bigFont->fid);
            XDrawString(dpy, overlay, gc, tx, ty, ":(", 2);
            ty += 90;
        }
        if (bodyFont) {
            XSetFont(dpy, gc, bodyFont->fid);
            XDrawString(dpy, overlay, gc, tx, ty,
                        "Your PC ran into a problem and needs to restart.", 47);
            ty += 28;
            XDrawString(dpy, overlay, gc, tx, ty,
                        "We're just collecting some error info, and then we'll restart for you.", 69);
            ty += 40;

            char pct[32];
            snprintf(pct, sizeof(pct), "%d%% complete", g_percent);
            XDrawString(dpy, overlay, gc, tx, ty, pct, strlen(pct));
            ty += 34;
            XDrawString(dpy, overlay, gc, tx, ty, g_status, strlen(g_status));
        }
        XFlush(dpy);

        g_ticks++;
        if (g_percent < 99 && g_ticks % 2 == 0) {
            g_percent++;
        }
        sleep(1);
    }

    XCloseDisplay(dpy);
    return 0;
}