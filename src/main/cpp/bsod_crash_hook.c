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

static LONG WINAPI BsodVectoredHandler(PEXCEPTION_POINTERS info) {
    DWORD code = (info && info->ExceptionRecord) ? info->ExceptionRecord->ExceptionCode : 0;

    /* Access violation, fail-fast, integer div-by-zero, breakpoint. */
    if (code != 0xC0000005ul && code != 0xC0000409ul &&
        code != 0xC0000094ul && code != 0x80000003ul) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    char cmd[MAX_PATH * 2 + 160];
    lstrcpyA(cmd, "powershell -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File \"");
    lstrcatA(cmd, g_scriptPath);
    lstrcatA(cmd, "\" 0x");
    {
        char hex[16];
        const char* digits = "0123456789ABCDEF";
        int i;
        for (i = 0; i < 8; i++) {
            hex[i] = digits[(code >> ((7 - i) * 4)) & 0xF];
        }
        hex[8] = '\0';
        lstrcatA(cmd, hex);
    }
    lstrcatA(cmd, " \"");
    lstrcatA(cmd, g_restartPath);
    lstrcatA(cmd, "\"");

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