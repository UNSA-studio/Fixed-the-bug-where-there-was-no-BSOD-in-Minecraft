#include <windows.h>
#include <shellapi.h>

/*
 * BSOD overlay - a tiny standalone Win32 program.
 *
 * Spawned ONLY by the unhandled-exception filter, i.e. when the JVM is
 * genuinely dying. Becomes a CHILD of the dead Minecraft window, so the blue
 * screen literally lives INSIDE the game window: it moves with it, minimises
 * with it, and can never cover anything else. Draws the classic Windows 10
 * BSOD (QR code included), waits for the game process to disappear, relaunches
 * it via the restart script, quits.
 *
 * argv: [1]=stop code hex  [2..5]=window l,t,r,b (fallback size only)
 *       [6]=game pid  [7]=restart cmd
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
static char g_status[160] = "";

static HFONT g_faceFont;
static HFONT g_bodyFont;
static HFONT g_boldFont;

/* Child of the Minecraft window: the OS moves it whenever the game moves. */
static HWND g_mcWindow;

/* Finds the biggest visible top-level window of our own process (the MC window). */
static BOOL CALLBACK FindMcWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    RECT wr;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
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
            SetTimer(hwnd, 2, 2500, NULL);
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

        SelectObject(dc, g_bodyFont);
        if (g_status[0]) {
            TextOutA(dc, x, y, g_status, lstrlenA(g_status));
            y += h / 14;
        }

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
    case WM_LBUTTONDOWN:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (wp == VK_ESCAPE) {
            PostQuitMessage(0);
        }
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

    int width, height;
    if (r <= l || b <= t || l < 0 || t < 0) {
        width = GetSystemMetrics(SM_CXSCREEN) * 3 / 4;
        height = GetSystemMetrics(SM_CYSCREEN) * 3 / 4;
    } else {
        width = r - l;
        height = b - t;
    }

    /* Find the Minecraft window so we can become its child: the OS then moves
     * and clips us automatically whenever the game window moves or minimises.
     * The blue screen literally lives INSIDE the game window. */
    g_mcWindow = NULL;
    EnumWindows(FindMcWindowProc, (LPARAM) &g_mcWindow);

    if (g_mcWindow) {
        RECT cr;
        GetClientRect(g_mcWindow, &cr);
        width = cr.right;
        height = cr.bottom;

        /* WS_CHILD, positioned at 0,0 inside the game window. */
        HWND hwnd = CreateWindowExA(0,
                                    "UnsaBsodOverlay", "BSOD",
                                    WS_CHILD | WS_VISIBLE,
                                    0, 0, width, height,
                                    g_mcWindow, NULL, hInst, NULL);
        if (!hwnd) {
            return 1;
        }
        SetFocus(hwnd);
    } else {
        /* Fallback: standalone popup centred on screen. */
        int left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
        int top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
        HWND hwnd = CreateWindowExA(0,
                                    "UnsaBsodOverlay", "BSOD",
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