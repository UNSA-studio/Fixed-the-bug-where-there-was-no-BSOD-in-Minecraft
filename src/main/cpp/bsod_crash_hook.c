#include <jni.h>
#include <windows.h>

/*
 * BSOD native crash hook.
 *
 * Uses SetUnhandledExceptionFilter: it fires ONLY when no frame-based handler
 * (SEH) claims the exception. That is exactly the "genuinely dying" case.
 * HotSpot's benign exceptions - including its implicit-null-check page faults
 * - are all handled by SEH long before we would ever see them, so there is no
 * risk of the overlay appearing while the game is still running (which the
 * previous AddVectoredExceptionHandler approach got hilariously wrong).
 *
 * In the filter we spawn the overlay and return EXCEPTION_CONTINUE_SEARCH so
 * the JVM continues with its normal crash reporting (hs_err log).
 */

static char g_overlayPath[MAX_PATH];
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

static LONG WINAPI BsodUnhandledFilter(PEXCEPTION_POINTERS info) {
    DWORD code = (info && info->ExceptionRecord) ? info->ExceptionRecord->ExceptionCode : 0;

    /* Capture the MC window rectangle so the overlay can sit exactly where
     * the game window was, instead of covering the whole desktop. */
    RECT mcRect = { -1, -1, -1, -1 };
    EnumWindows(FindMcWindowProc, (LPARAM) &mcRect);

    char cmd[MAX_PATH * 2 + 300];
    lstrcpyA(cmd, "cmd /c start \"\" \"");
    lstrcatA(cmd, g_overlayPath);
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
    wsprintfA(cmd + lstrlenA(cmd), " %ld %ld %ld %ld %lu \"%s\"",
              (long) mcRect.left, (long) mcRect.top,
              (long) mcRect.right, (long) mcRect.bottom,
              (unsigned long) GetCurrentProcessId(),
              g_restartPath);

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

    /* Let the JVM do its own crash reporting and exit as usual. */
    return EXCEPTION_CONTINUE_SEARCH;
}

__declspec(dllexport) void JNICALL
Java_www_unsa_bsod_com_crash_NativeCrashHook_install0(JNIEnv* env, jclass cls,
                                                     jstring overlayPath,
                                                     jstring restartPath) {
    const char* s = (*env)->GetStringUTFChars(env, overlayPath, NULL);
    const char* r = (*env)->GetStringUTFChars(env, restartPath, NULL);
    lstrcpynA(g_overlayPath, s, MAX_PATH);
    lstrcpynA(g_restartPath, r, MAX_PATH);
    (*env)->ReleaseStringUTFChars(env, overlayPath, s);
    (*env)->ReleaseStringUTFChars(env, restartPath, r);

    SetUnhandledExceptionFilter(BsodUnhandledFilter);
}