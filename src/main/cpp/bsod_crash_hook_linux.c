#define _GNU_SOURCE
#include <jni.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*
 * BSOD native crash hook for Linux.
 *
 * Installs POSIX signal handlers for the fatal hardware-ish signals. In the
 * handler we may ONLY use async-signal-safe calls, so we simply fork() and
 * execv() the X11 overlay binary, passing the saved original PID so the
 * overlay can locate and cover the (already doomed) Minecraft window, plus
 * the restart script path. We then restore the default action so the JVM
 * still produces its hs_err log exactly like vanilla.
 */

static char g_overlayPath[4096];
static char g_restartPath[4096];
static char g_pidText[16];

static void BsodSignalHandler(int sig, siginfo_t* info, void* uctx) {
    (void) info;
    (void) uctx;

    pid_t child = fork();
    if (child == 0) {
        /* Child: become the overlay. execl is async-signal-safe. */
        execl(g_overlayPath, g_overlayPath, g_pidText, g_restartPath, (char*) NULL);
        _exit(127);
    }

    /* Restore default handling and re-raise so HotSpot writes hs_err. */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
    raise(sig);
}

__attribute__((visibility("default")))
void JNICALL
Java_www_unsa_bsod_com_crash_NativeCrashHook_install0(JNIEnv* env, jclass cls,
                                                     jstring overlayPath,
                                                     jstring restartPath) {
    (void) cls;
    const char* o = (*env)->GetStringUTFChars(env, overlayPath, NULL);
    const char* r = (*env)->GetStringUTFChars(env, restartPath, NULL);
    strncpy(g_overlayPath, o, sizeof(g_overlayPath) - 1);
    strncpy(g_restartPath, r, sizeof(g_restartPath) - 1);
    (*env)->ReleaseStringUTFChars(env, overlayPath, o);
    (*env)->ReleaseStringUTFChars(env, restartPath, r);

    snprintf(g_pidText, sizeof(g_pidText), "%d", (int) getpid());

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = BsodSignalHandler;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
}