#include <windows.h>
#include <shellapi.h>

/*
 * BSOD overlay - a tiny standalone Win32 program.
 *
 * Spawned by the vectored exception handler at the instant the JVM dies from
 * a native crash. Draws the classic Windows-blue screen exactly over the
 * (now dead) Minecraft window rectangle, shows a live percentage, waits for
 * the game process to disappear, relaunches it via bsod_restart.cmd, quits.
 *
 * Deliberately dependency-free: pure user32/gdi32/shell32. No CRT heap use,
 * no network, no scripting engines - starts in milliseconds.
 *
 * argv: [1]=stop code hex  [2..5]=window l,t,r,b  [6]=game pid
 */

static char  g_codeText[32];
static DWORD g_gamePid;
static char  g_restartCmd[MAX_PATH * 2];

static int  g_ticks = 0;
static int  g_percent = 0;
static int  g_relaunchDone = 0;
static char g_status[160] = "Collecting error info...";

static HFONT g_faceFont;
static HFONT g_bodyFont;
static HFONT g_smallFont;
static HFONT g_boldFont;

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
        g_percent = g_ticks * 5;
        if (g_percent > 99) {
            g_percent = 99;
        }

        HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, g_gamePid);
        if (proc == NULL || WaitForSingleObject(proc, 0) == WAIT_OBJECT_0) {
            if (proc) {
                CloseHandle(proc);
            }
            KillTimer(hwnd, 1);
            g_percent = 100;
            lstrcpyA(g_status, "Minecraft exited. Relaunching...");
            InvalidateRect(hwnd, NULL, TRUE);

            if (!g_relaunchDone) {
                g_relaunchDone = 1;
                SHELLEXECUTEINFOA sei;
                ZeroMemory(&sei, sizeof(sei));
                sei.cbSize = sizeof(sei);
                sei.lpVerb = "open";
                sei.lpFile = g_restartCmd;
                sei.nShow = SW_SHOWNORMAL;
                ShellExecuteExA(&sei);
            }
            SetTimer(hwnd, 2, 2500, NULL); /* brief goodbye, then quit */
        } else {
            CloseHandle(proc);
            InvalidateRect(hwnd, NULL, TRUE);
        }
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

        int x = 56;
        int scale = w / 640;
        if (scale < 1) {
            scale = 1;
        }

        SelectObject(dc, g_faceFont);
        TextOutA(dc, x, 24 * scale, ":(", 2);

        int y = 24 * scale + 120;
        SelectObject(dc, g_bodyFont);
        TextOutA(dc, x, y, "Your PC ran into a problem and needs to restart.", 47);
        y += 34;
        TextOutA(dc, x, y,
                 "We're just collecting some error info, and then we'll restart for you.",
                 69);
        y += 52;

        SelectObject(dc, g_boldFont);
        char pct[48];
        wsprintfA(pct, "%d%% complete", g_percent);
        TextOutA(dc, x, y, pct, lstrlenA(pct));
        y += 44;

        SelectObject(dc, g_bodyFont);
        TextOutA(dc, x, y, g_status, lstrlenA(g_status));
        y += 40;

        char stop[96];
        wsprintfA(stop, "Stop code: %s", g_codeText);
        TextOutA(dc, x, y, stop, lstrlenA(stop));

        SelectObject(dc, g_smallFont);
        SetTextColor(dc, RGB(196, 222, 248));
        TextOutA(dc, x, h - 36 * scale, "Press ESC to close this screen", 30);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        break;
    case WM_LBUTTONDOWN:
        PostQuitMessage(0);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prev, LPSTR cmdLine, int show) {
    (void) prev; (void) cmdLine; (void) show;

    int argc = __argc;
    char** argv = __argv;

    if (argc >= 2) {
        lstrcpynA(g_codeText, argv[1], sizeof(g_codeText));
    } else {
        lstrcpyA(g_codeText, "UNKNOWN");
    }
    long l = -1, t = -1, r = -1, b = -1;
    if (argc >= 6) {
        l = atol(argv[2]);
        t = atol(argv[3]);
        r = atol(argv[4]);
        b = atol(argv[5]);
    }
    if (argc >= 7) {
        g_gamePid = (DWORD) atol(argv[6]);
    }
    if (argc >= 8 && argv[7][0]) {
        lstrcpynA(g_restartCmd, argv[7], sizeof(g_restartCmd));
    }

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0x78, 0xD7));
    wc.lpszClassName = "UnsaBsodOverlay";
    RegisterClassA(&wc);

    int left = l, top = t, width, height;
    if (r <= l || b <= t || l < 0 || t < 0) {
        /* Fallback: centered, roughly the default MC window size. */
        width = GetSystemMetrics(SM_CXSCREEN) * 3 / 4;
        height = GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
        left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
    } else {
        width = r - l;
        height = b - t;
    }

    HWND hwnd = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                "UnsaBsodOverlay", "BSOD",
                                WS_POPUP | WS_VISIBLE,
                                left, top, width, height,
                                NULL, NULL, hInst, NULL);
    if (!hwnd) {
        return 1;
    }
    SetForegroundWindow(hwnd);

    g_faceFont = CreateFontA(-(height / 5), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    g_bodyFont = CreateFontA(-(height / 22), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    g_boldFont = CreateFontA(-(height / 17), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
    g_smallFont = CreateFontA(-(height / 28), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return 0;
}