#include <jni.h>
#include <windows.h>

/*
 * BSOD native crash hook.
 *
 * Registers a vectored exception handler that runs BEFORE HotSpot's own
 * handlers. For fatal exception codes we launch a detached PowerShell
 * overlay that draws the classic blue screen while the JVM finishes its
 * own crash reporting (hs_err log). We then always return
 * EXCEPTION_CONTINUE_SEARCH so JVM behaviour stays untouched.
 *
 * Deliberately crash-context-safe: the handler only touches kernel32/user32
 * string APIs and CreateProcessA - no CRT heap allocations.
 */

static char g_scriptPath[MAX_PATH];
static char g_restartPath[MAX_PATH];

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
        RECT* out = (RECT*) lParam;
        *out = wr;
        return FALSE; /* found it, stop enumeration */
    }
    return TRUE;
}

static LONG WINAPI BsodVectoredHandler(PEXCEPTION_POINTERS info) {
    DWORD code = (info && info->ExceptionRecord) ? info->ExceptionRecord->ExceptionCode : 0;

    /* Access violation, fail-fast, integer div-by-zero, breakpoint. */
    if (code != 0xC0000005ul && code != 0xC0000409ul &&
        code != 0xC0000094ul && code != 0x80000003ul) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    /* Capture the MC window rectangle so the overlay can sit exactly where
     * the game window was, instead of covering the whole desktop. */
    RECT mcRect = { -1, -1, -1, -1 };
    EnumWindows(FindMcWindowProc, (LPARAM) &mcRect);

    char cmd[MAX_PATH * 2 + 260];
    lstrcpyA(cmd, "powershell -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File \"");
    lstrcatA(cmd, g_scriptPath);
    lstrcatA(cmd, "\" 0x");
    {
        char num[16];
        const char* digits = "0123456789ABCDEF";
        int i;
        for (i = 0; i < 8; i++) {
            num[i] = digits[(code >> ((7 - i) * 4)) & 0xF];
        }
        num[8] = '\0';
        lstrcatA(cmd, num);
    }
    lstrcatA(cmd, " \"");
    lstrcatA(cmd, g_restartPath);
    lstrcatA(cmd, "\"");
    wsprintfA(cmd + lstrlenA(cmd), " %ld %ld %ld %ld",
              (long) mcRect.left, (long) mcRect.top,
              (long) mcRect.right, (long) mcRect.bottom);

    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        CreateProcessA(NULL, cmd, NULL, NULL, FALSE,
                       NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                       NULL, NULL, &si, &pi);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

__declspec(dllexport) void JNICALL
Java_www_unsa_bsod_com_crash_NativeCrashHook_install0(JNIEnv* env, jclass cls,
                                                     jstring scriptPath,
                                                     jstring restartPath) {
    const char* s = (*env)->GetStringUTFChars(env, scriptPath, NULL);
    const char* r = (*env)->GetStringUTFChars(env, restartPath, NULL);
    lstrcpynA(g_scriptPath, s, MAX_PATH);
    lstrcpynA(g_restartPath, r, MAX_PATH);
    (*env)->ReleaseStringUTFChars(env, scriptPath, s);
    (*env)->ReleaseStringUTFChars(env, restartPath, r);

    AddVectoredExceptionHandler(1, BsodVectoredHandler);
}