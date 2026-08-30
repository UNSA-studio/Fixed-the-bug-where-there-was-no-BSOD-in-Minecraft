#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*
 * BSOD overlay - a tiny standalone Win32 program.
 *
 * Spawned ONLY by the unhandled-exception filter, i.e. when the JVM is
 * genuinely dying.
 *
 * Two-phase display:
 *   Phase 1: while the game process is STILL ALIVE, the overlay is a WS_CHILD
 *            of the Minecraft window - it lives strictly inside the game
 *            window and moves with it.
 *   Phase 2: the moment the game process dies, we RE-PARENT ourselves to
 *            NULL (independent popup at the exact same screen position) and
 *            keep the blue screen alive for the restart countdown. The blue
 *            screen never disappears together with the dying game window.
 *
 * Watchdog mode (--watch):
 *   The unhandled-exception filter approach turned out to be dead code on
 *   HotSpot - the JVM consumes its own fatal exceptions internally and exits
 *   without ever calling our filter (proven by native_hook.log: two installs,
 *   zero filter calls). So Windows now uses the same zero-intrusion design
 *   that already works on Linux: the overlay runs as a WATCHDOG for the
 *   whole game session. It polls the game process and, on death, only shows
 *   the blue screen if a FRESH hs_err_pid*.log appeared (i.e. a native crash)
 *   - a normal exit never triggers it. The blue screen is then placed at the
 *   LAST KNOWN position of the game window.
 *
 * argv: [--watch <pid> <restart cmd> <game dir> | <stop code> <pid> <restart cmd>]
 *
 * Diagnostic log: overlay.log, written next to the restart script.
 */

/* QR for https://www.minecraft.net, 25x25, ECC M. */
static const int QR_SIZE = 25;
static const unsigned char QR_BITS[25][25] = {
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0},
    {1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0},
    {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
};

static char  g_codeText[32];
static DWORD g_gamePid;
static char  g_restartCmd[MAX_PATH * 2];

static int  g_ticks = 0;
static int  g_percent = 0;
static int  g_relaunchDone = 0;
static int  g_deathHandled = 0;

static HFONT g_faceFont;
static HFONT g_bodyFont;
static HFONT g_boldFont;

/* Child of the Minecraft window: the OS moves it whenever the game moves. */
static HWND g_mcWindow;

static void LogLine(const char* fmt, ...) {
    /* Always next to the overlay exe itself (it lives in the BSOD folder),
     * regardless of how we were launched or which CWD we inherited. */
    char path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, path, sizeof(path))) {
        return;
    }
    char* slash = strrchr(path, '\\');
    if (!slash) {
        return;
    }
    *(slash + 1) = '\0';
    lstrcatA(path, "overlay.log");
    FILE* f = fopen(path, "a");
    if (!f) {
        return;
    }
    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(f, "[%02u:%02u:%02u] ", st.wHour, st.wMinute, st.wSecond);
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fputc('\n', f);
    fclose(f);
}

/* Finds the top-level window of the DYING game process (not ourselves). */
static BOOL CALLBACK FindMcWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    RECT wr;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != g_gamePid || pid == GetCurrentProcessId()) {
        return TRUE;
    }
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }
    if (GetWindowRect(hwnd, &wr)
            && (wr.right - wr.left) > 240 && (wr.bottom - wr.top) > 160) {
        HWND* out = (HWND*) lParam;
        *out = hwnd;
        return FALSE; /* found it, stop enumeration */
    }
    return TRUE;
}

/* ---- Watchdog helpers (Windows mirror of the Linux zero-intrusion design) ---- */

/* Last known position of the game window, tracked while it is alive. */
static RECT g_lastRect = { -1, -1, -1, -1 };

/* Finds the top-level window of the game process and remembers its rect. */
static void TrackGameWindow(DWORD pid) {
    g_gamePid = pid;
    HWND hwnd = NULL;
    EnumWindows(FindMcWindowProc, (LPARAM) &hwnd);
    if (hwnd) {
        GetWindowRect(hwnd, &g_lastRect);
    }
}

/* Copies the path of the most recently written hs_err into |out| (empty on
 * none). |since| filters files written before the watchdog started. */
static void NewestHsErrPath(const char* gameDir, const FILETIME* since, char* out,
                            size_t outLen) {
    char pattern[MAX_PATH * 2];
    lstrcpynA(pattern, gameDir, sizeof(pattern));
    size_t len = lstrlenA(pattern);
    if (len && pattern[len - 1] != '\\' && pattern[len - 1] != '/') {
        lstrcatA(pattern, "\\");
    }
    lstrcatA(pattern, "hs_err_pid*.log");
    out[0] = '\0';

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return;
    }
    char dirPart[MAX_PATH * 2];
    lstrcpynA(dirPart, pattern, sizeof(dirPart));
    char* slash = strrchr(dirPart, '\\');
    if (slash) {
        *(slash + 1) = '\0';
    } else {
        dirPart[0] = '\0';
    }
    FILETIME best = { 0, 0 };
    do {
        if (CompareFileTime(&fd.ftLastWriteTime, since) > 0
                && CompareFileTime(&fd.ftLastWriteTime, &best) > 0) {
            best = fd.ftLastWriteTime;
            lstrcpynA(out, dirPart, (int) outLen);
            lstrcatA(out, fd.cFileName);
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

/* Pulls the Windows exception code out of the first line of an hs_err file,
 * e.g. "# EXCEPTION_ACCESS_VIOLATION (0xc0000005) at pc=...". */
static void ExtractStopCode(const char* hsErrPath, char* out, size_t outLen) {
    lstrcpynA(out, "NATIVE_CRASH", (int) outLen);
    FILE* f = fopen(hsErrPath, "rb");
    if (!f) {
        return;
    }
    char head[512];
    size_t n = fread(head, 1, sizeof(head) - 1, f);
    fclose(f);
    head[n] = '\0';

    const char* p = head;
    while ((p = strstr(p, "0x")) != NULL) {
        const char* hex = p + 2;
        int digits = 0;
        while ((hex[digits] >= '0' && hex[digits] <= '9')
                || (hex[digits] >= 'a' && hex[digits] <= 'f')
                || (hex[digits] >= 'A' && hex[digits] <= 'F')) {
            digits++;
        }
        if (digits == 8) {
            lstrcpynA(out, "0x", (int) outLen);
            lstrcatA(out, hex); /* full copy then cut - lstrcpynA handles len */
            out[10] = '\0';
            return;
        }
        p = hex;
    }
}

/* Scans <gameDir> for hs_err_pid*.log files written after the given FILETIME.
 * Returns 1 if at least one fresh hs_err exists (a native crash happened). */
static int FreshHsErrExists(const char* gameDir, const FILETIME* since) {
    char pattern[MAX_PATH * 2];
    lstrcpynA(pattern, gameDir, sizeof(pattern));
    size_t len = lstrlenA(pattern);
    if (len && pattern[len - 1] != '\\' && pattern[len - 1] != '/') {
        lstrcatA(pattern, "\\");
    }
    lstrcatA(pattern, "hs_err_pid*.log");

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return 0;
    }
    int found = 0;
    do {
        if (CompareFileTime(&fd.ftLastWriteTime, since) > 0) {
            found = 1;
            break;
        }
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    return found;
}

/* Waits for the game process to die. Returns the process exit code. */
static DWORD WaitForGameDeath(DWORD pid) {
    DWORD exitCode = (DWORD) -1;
    HANDLE proc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                              FALSE, pid);
    if (proc) {
        WaitForSingleObject(proc, INFINITE);
        GetExitCodeProcess(proc, &exitCode);
        CloseHandle(proc);
    }
    return exitCode;
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        SetTimer(hwnd, 1, 500, NULL);
        break;
    }
    case WM_TIMER: {
        if (wp == 2) {          /* goodbye timer */
            PostQuitMessage(0);
            break;
        }
        g_ticks++;
        if (g_percent < 99) {
            g_percent = g_ticks * 5;
            if (g_percent > 99) {
                g_percent = 99;
            }
        }

        /* Is the game still alive? */
        HANDLE proc = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                                  FALSE, g_gamePid);
        int dead = (proc == NULL || WaitForSingleObject(proc, 0) == WAIT_OBJECT_0);
        if (proc) {
            CloseHandle(proc);
        }

        if (dead && !g_deathHandled) {
            g_deathHandled = 1;
            /* The game window dies with the process - break free as an
             * independent borderless window at the SAME screen position. */
            if (g_mcWindow && IsWindow(g_mcWindow)) {
                LogLine("game dead - re-parenting to desktop");
                POINT origin = { 0, 0 };
                ClientToScreen(g_mcWindow, &origin);
                RECT cr;
                GetClientRect(g_mcWindow, &cr);
                SetParent(hwnd, NULL);
                DWORD style = GetWindowLongA(hwnd, GWL_STYLE);
                style &= ~WS_CHILD;
                style |= WS_POPUP;
                SetWindowLongA(hwnd, GWL_STYLE, style);
                SetWindowPos(hwnd, NULL, origin.x, origin.y,
                             cr.right, cr.bottom,
                             SWP_NOZORDER | SWP_NOACTIVATE);
                g_mcWindow = NULL;
            }
        }

        if (dead && g_ticks >= 8 && !g_relaunchDone) {
            KillTimer(hwnd, 1);
            g_percent = 100;
            g_relaunchDone = 1;
            LogLine("relaunching via %s", g_restartCmd);
            SHELLEXECUTEINFOA sei;
            ZeroMemory(&sei, sizeof(sei));
            sei.cbSize = sizeof(sei);
            sei.lpVerb = "open";
            sei.lpFile = g_restartCmd;
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExA(&sei);
            SetTimer(hwnd, 2, 2500, NULL);
        }

        /* Still on stage: keep following the (alive) game window. */
        if (!dead && g_mcWindow && IsWindow(g_mcWindow)) {
            RECT cr;
            GetClientRect(g_mcWindow, &cr);
            SetWindowPos(hwnd, NULL, 0, 0, cr.right, cr.bottom,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        int w = rc.right;
        int h = rc.bottom;

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));

        int margin = h / 10;
        int x = margin + h / 14;
        int y = margin;

        SelectObject(dc, g_faceFont);
        TextOutA(dc, x, y, ":(", 2);
        y += h / 5 + h / 22;

        SelectObject(dc, g_bodyFont);
        TextOutA(dc, x, y, "Your PC ran into a problem and needs to restart.", 47);
        y += h / 17;
        TextOutA(dc, x, y,
                 "We're just collecting some error info, and then we'll restart for you.",
                 69);
        y += h / 12;

        SelectObject(dc, g_boldFont);
        char pct[48];
        wsprintfA(pct, "%d%% complete", g_percent);
        TextOutA(dc, x, y, pct, lstrlenA(pct));
        y += h / 9;

        /* No status text: a real BSOD shows nothing but the bare elements. */

        /* ---- QR block, like the real thing: mid-screen, left aligned ---- */
        int qrScale = h / 300;
        if (qrScale < 2) {
            qrScale = 2;
        }
        int qrPixels = QR_SIZE * qrScale;
        if (qrPixels < w / 6) {
            qrScale = w / 6 / QR_SIZE;
            if (qrScale < 1) {
                qrScale = 1;
            }
            qrPixels = QR_SIZE * qrScale;
        }
        int qrX = x;
        int qrY = y + 6;
        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        for (int row = 0; row < QR_SIZE; row++) {
            for (int col = 0; col < QR_SIZE; col++) {
                if (QR_BITS[row][col]) {
                    RECT cell = { qrX + col * qrScale, qrY + row * qrScale,
                                  qrX + (col + 1) * qrScale, qrY + (row + 1) * qrScale };
                    FillRect(dc, &cell, whiteBrush);
                }
            }
        }
        DeleteObject(whiteBrush);

        SelectObject(dc, g_bodyFont);
        int tx = qrX + qrPixels + h / 30;
        int ty = qrY + qrPixels / 2 - h / 28;
        TextOutA(dc, tx, ty, "For more information about this issue", 37);
        ty += h / 26;
        TextOutA(dc, tx, ty, "and possible fixes, visit", 25);
        ty += h / 26;
        TextOutA(dc, tx, ty, "https://www.minecraft.net", 25);

        /* ---- Stop code, bottom right like the real thing ---- */
        SelectObject(dc, g_bodyFont);
        char stop[128];
        wsprintfA(stop, "Stop code: %s", g_codeText);
        SIZE sz;
        GetTextExtentPoint32A(dc, stop, lstrlenA(stop), &sz);
        TextOutA(dc, w - sz.cx - h / 16, h - sz.cy - h / 14, stop, lstrlenA(stop));

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
    case WM_DESTROY:
        /* The blue screen refuses to be dismissed while it is on stage -
         * no Alt+F4, no close, nothing. Only after it has relaunched the
         * game may it be closed. */
        if (!g_relaunchDone) {
            return 0;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

/* Blue screen window class: WS_POPUP with no close button is still an
 * independent application that can be dismissed - which would kill the
 * joke. The real anti-dismissal trick: a plain WS_POPUP window pair where
 * the VISIBLE blue screen is a child of a tiny invisible owner window, so
 * there is no title bar, no taskbar entry, no close box at all. */
static const char* const kOverlayClass = "UnsaBsodOverlay";

static LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CLOSE:
    case WM_DESTROY:
        /* Closing the (invisible) owner would kill the show - refuse. */
        if (g_alive) {
            return 0;
        }
        break;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

static void RegisterClasses(HINSTANCE hInst) {
    WNDCLASSA overlay;
    ZeroMemory(&overlay, sizeof(overlay));
    overlay.lpfnWndProc = OverlayProc;
    overlay.hInstance = hInst;
    overlay.hCursor = LoadCursor(NULL, IDC_ARROW);
    overlay.hbrBackground = CreateSolidBrush(RGB(0, 0x78, 0xD7));
    overlay.lpszClassName = kOverlayClass;
    RegisterClassA(&overlay);

    WNDCLASSA owner;
    ZeroMemory(&owner, sizeof(owner));
    owner.lpfnWndProc = OwnerProc;
    owner.hInstance = hInst;
    owner.lpszClassName = "UnsaBsodOwner";
    RegisterClassA(&owner);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdLine, int show) {
    (void) prev; (void) cmdLine; (void) show;

    int argc = __argc;
    char** argv = __argv;

    /* ---- Watchdog mode: guard the whole game session. Primary signal is a
     * FRESH hs_err file APPEARING - the JVM writes it while the game window
     * is still alive, so we can move INTO the game window as a child and the
     * blue screen appears in-place, milliseconds after the crash. If that
     * watcher misses it (no hs_err but the process died anyway), fall back
     * to waiting for process death. ---- */
    if (argc >= 2 && lstrcmpA(argv[1], "--watch") == 0) {
        DWORD pid = (argc >= 3) ? (DWORD) strtoul(argv[2], NULL, 10) : 0;
        if (argc >= 4 && argv[3][0]) {
            lstrcpynA(g_restartCmd, argv[3], sizeof(g_restartCmd));
        }
        char gameDir[MAX_PATH * 2] = "";
        if (argc >= 5 && argv[4][0]) {
            lstrcpynA(gameDir, argv[4], sizeof(gameDir));
        }
        LogLine("watchdog start: pid=%lu", (unsigned long) pid);

        TrackGameWindow(pid);
        FILETIME started;
        GetSystemTimeAsFileTime(&started);

        /* Watch for a fresh hs_err while the process is alive. The moment
         * one appears, the game window is still there - attach to it and
         * show the blue screen immediately (in-place, inside the window). */
        int crashConfirmed = 0;
        for (;;) {
            HANDLE proc = OpenProcess(SYNCHRONIZE, FALSE, pid);
            if (!proc) {
                break;
            }
            DWORD w = WaitForSingleObject(proc, 200);
            CloseHandle(proc);
            if (w == WAIT_OBJECT_0) {
                break;
            }
            TrackGameWindow(pid);

            if (!crashConfirmed && FreshHsErrExists(gameDir, &started)) {
                char hsErrPath[MAX_PATH * 2];
                NewestHsErrPath(gameDir, &started, hsErrPath, sizeof(hsErrPath));
                ExtractStopCode(hsErrPath, g_codeText, sizeof(g_codeText));
                LogLine("native crash confirmed (%s) - game window still alive,"
                        " showing BSOD in-place",
                        g_codeText);
                crashConfirmed = 1;
                break;      /* fall through to window creation NOW */
            }
        }

        if (!crashConfirmed) {
            /* The process died without hs_err being seen: confirm via the
             * files written before exit (covers very fast deaths). */
            if (FreshHsErrExists(gameDir, &started)) {
                char hsErrPath[MAX_PATH * 2];
                NewestHsErrPath(gameDir, &started, hsErrPath, sizeof(hsErrPath));
                ExtractStopCode(hsErrPath, g_codeText, sizeof(g_codeText));
                LogLine("game died with hs_err (%s)", g_codeText);
                crashConfirmed = 1;
                TrackGameWindow(pid);   /* last chance for the rect */
            } else {
                LogLine("no fresh hs_err - normal exit, staying silent");
                return 0;
            }
        }
    } else {
        if (argc >= 2) {
            lstrcpynA(g_codeText, argv[1], sizeof(g_codeText));
        } else {
            lstrcpyA(g_codeText, "UNKNOWN");
        }
        if (argc >= 3) {
            g_gamePid = (DWORD) strtoul(argv[2], NULL, 10);
        }
        if (argc >= 4 && argv[3][0]) {
            lstrcpynA(g_restartCmd, argv[3], sizeof(g_restartCmd));
        }
        LogLine("overlay start: pid=%lu code=%s",
                (unsigned long) g_gamePid, g_codeText);
    }

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0x78, 0xD7));
    wc.lpszClassName = "UnsaBsodOverlay";
    RegisterClassA(&wc);

    int width = GetSystemMetrics(SM_CXSCREEN) * 3 / 4;
    int height = GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
    int left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    /* The game window is already gone in watchdog mode: use its LAST KNOWN
     * position so the blue screen appears exactly where the game was. */
    if (g_lastRect.right > g_lastRect.left && g_lastRect.bottom > g_lastRect.top) {
        left = g_lastRect.left;
        top = g_lastRect.top;
        width = g_lastRect.right - g_lastRect.left;
        height = g_lastRect.bottom - g_lastRect.top;
    }
/* Live (crash just confirmed via hs_err while the process still runs):
     * move INTO the game window as a child - blue screen appears in-place,
     * strictly clipped inside it, following it. Dead (hs_err seen after
     * death, or a direct launch): place at the last known position. */
    g_mcWindow = NULL;
    if (g_gamePid) {
        EnumWindows(FindMcWindowProc, (LPARAM) &g_mcWindow);
    }

    if (g_mcWindow) {
        LogLine("attached as child of game window (in-place)");
        RECT cr;
        GetClientRect(g_mcWindow, &cr);
        width = cr.right;
        height = cr.bottom;

        HWND hwnd = CreateWindowExA(0,
                                    kOverlayClass, "BSOD",
                                    WS_CHILD | WS_VISIBLE,
                                    0, 0, width, height,
                                    g_mcWindow, NULL, hInst, NULL);
        if (!hwnd) {
            LogLine("CreateWindow(child) failed, gle=%lu", GetLastError());
            return 1;
        }
        SetFocus(hwnd);
    } else {
        LogLine("standalone popup at %ld,%ld (%ldx%ld)",
                (long) left, (long) top, (long) width, (long) height);
        /* Borderless, no taskbar button, refuses Alt+F4 (see WM_CLOSE). */
        HWND hwnd = CreateWindowExA(WS_EX_TOOLWINDOW,
                                    kOverlayClass, "BSOD",
                                    WS_POPUP | WS_VISIBLE,
                                    left, top, width, height,
                                    NULL, NULL, hInst, NULL);
        if (!hwnd) {
            return 1;
        }
        SetForegroundWindow(hwnd);
    }

    g_faceFont = CreateFontA(-(height / 5), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    g_bodyFont = CreateFontA(-(height / 24), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    g_boldFont = CreateFontA(-(height / 19), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}