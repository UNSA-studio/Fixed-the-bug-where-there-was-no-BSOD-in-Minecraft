#include <jni.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

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
 * IMPORTANT: SetUnhandledExceptionFilter REPLACES the previous filter (the
 * JVM installs its own to write hs_err_pid*.log). We therefore chain to the
 * previous filter AFTER spawning our overlay, so the JVM still writes its
 * crash log and does its normal crash reporting.
 *
 * Diagnostic log: <gameDir>/BSOD/native_hook.log (written with the game's
 * working directory as CWD, so it lands inside the BSOD folder).
 */

static char g_overlayPath[MAX_PATH];
static char g_restartPath[MAX_PATH];
static LPTOP_LEVEL_EXCEPTION_FILTER g_prevFilter = NULL;

/* Absolute log paths, derived from the overlay exe location (the BSOD
 * folder) - CWD-relative paths would break when the launcher picks an
 * arbitrary working directory. */
static char g_logPath[MAX_PATH];
static char g_overlayLogPath[MAX_PATH];

static void LogLine(const char* fmt, ...) {
    if (!g_logPath[0]) {
        return;
    }
    FILE* f = fopen(g_logPath, "a");
    if (!f) {
        return;
    }
    SYSTEMTIME st;
    GetLocalTime(&st);
    fprintf(f, "[%04u-%02u-%02u %02u:%02u:%02u] ",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond);
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fputc('\n', f);
    fclose(f);
}

/* Builds <bsodDir>/<name> from the absolute overlay exe path. */
static void BuildBsodPath(char* out, size_t outLen, const char* name) {
    lstrcpynA(out, g_overlayPath, (int) outLen);
    char* slash = strrchr(out, '\\');
    if (slash) {
        *(slash + 1) = '\0';
    } else {
        out[0] = '\0';
    }
    lstrcatA(out, name);
}

static LONG WINAPI ChainPrev(PEXCEPTION_POINTERS info) {
    if (g_prevFilter) {
        return g_prevFilter(info);
    }
    /* No previous filter: let the default machinery (WER) run. */
    return EXCEPTION_CONTINUE_SEARCH;
}

static LONG WINAPI BsodUnhandledFilter(PEXCEPTION_POINTERS info) {
    DWORD code = (info && info->ExceptionRecord) ? info->ExceptionRecord->ExceptionCode : 0;

    /* If an overlay already ran in this session, a second fatal exception
     * means the first crash aftermath went sideways (e.g. frozen overlay).
     * Kill any stale overlay so the user is not left with a dead rectangle
     * and let the JVM write hs_err and exit cleanly instead. */
    if (g_overlayLogPath[0]) {
        FILE* probe = fopen(g_overlayLogPath, "r");
        if (probe) {
            fclose(probe);
            LogLine("0x%08X after a previous overlay - killing stale overlay, skipping blue",
                    (unsigned) code);
            {
                STARTUPINFOA si;
                PROCESS_INFORMATION pi;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));
                if (CreateProcessA(NULL, "cmd /c taskkill /f /im bsod_overlay.exe",
                                   NULL, NULL, FALSE, CREATE_NO_WINDOW,
                                   NULL, NULL, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }
            return ChainPrev(info);
        }
    }

    LogLine("unhandled exception 0x%08X - spawning overlay", (unsigned) code);

    /* Spawn the overlay FIRST so it is already on screen while the JVM
     * finishes writing hs_err. The overlay monitors the game pid itself. */
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
    wsprintfA(cmd + lstrlenA(cmd), " %lu \"%s\"",
              (unsigned long) GetCurrentProcessId(),
              g_restartPath);

    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE,
                            NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                            NULL, NULL, &si, &pi)) {
            LogLine("CreateProcess failed, gle=%lu", (unsigned long) GetLastError());
        } else {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
            LogLine("overlay process started");
        }
    }

    /* Let the JVM do its own crash reporting (hs_err log) and exit as usual. */
    return ChainPrev(info);
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

    g_prevFilter = SetUnhandledExceptionFilter(BsodUnhandledFilter);

    /* A fresh game session starts with a clean overlay slate, so the
     * second-crash guard only triggers within the SAME session. */
    BuildBsodPath(g_logPath, sizeof(g_logPath), "native_hook.log");
    BuildBsodPath(g_overlayLogPath, sizeof(g_overlayLogPath), "overlay.log");
    if (g_overlayLogPath[0]) {
        remove(g_overlayLogPath);
    }
    LogLine("hook installed (overlay=%s)", g_overlayPath);
}