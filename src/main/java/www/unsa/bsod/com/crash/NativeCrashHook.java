package www.unsa.bsod.com.crash;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import com.mojang.logging.LogUtils;
import net.neoforged.fml.loading.FMLPaths;
import org.slf4j.Logger;

/**
 * Sets up native-crash detection.
 *
 * IMPORTANT LESSON (proven on 2026-08-30 via native_hook.log): on HotSpot,
 * a SetUnhandledExceptionFilter NEVER fires for JVM fatal crashes - HotSpot
 * consumes its own fatal exceptions internally and exits. Any in-process
 * hook is dead code. Therefore BOTH Windows and Linux use the same
 * zero-intrusion design: a small watchdog process that observes the game
 * from outside and only reacts when a FRESH hs_err_pid*.log appears (a
 * native crash). Normal exits never trigger the blue screen.
 *
 * macOS: not supported - the system's own crash reporter takes over, and we
 * simply stay silent.
 */
public final class NativeCrashHook {

    private static final Logger LOGGER = LogUtils.getLogger();
    private static boolean attempted;

    private NativeCrashHook() {
    }

    public static synchronized void install() {
        if (attempted) {
            return;
        }
        attempted = true;
        try {
            String os = System.getProperty("os.name", "").toLowerCase();
            boolean windows = os.contains("win");
            boolean linux = os.contains("linux") || os.contains("nix") || os.contains("aix");
            if (!windows && !linux) {
                LOGGER.info("[BSOD] Native crash overlay not supported on {}, skipping", os);
                return;
            }

            Path bsodDir = FMLPaths.GAMEDIR.get().resolve("BSOD");
            Files.createDirectories(bsodDir);

            if (windows) {
                installWindows(bsodDir);
            } else {
                installLinuxWatchdog(bsodDir);
            }
        } catch (Throwable t) {
            LOGGER.warn("[BSOD] Native crash hook unavailable: {}", t.toString());
        }
    }

    private static void installWindows(Path bsodDir) throws IOException {
        Path overlay = extractResource("bsod_overlay.exe", bsodDir);
        Path restartCmd = writeRestartScript(bsodDir, true);

        long gamePid = ProcessHandle.current().pid();
        String gameDir = FMLPaths.GAMEDIR.get().toAbsolutePath().toString();

        new ProcessBuilder(overlay.toAbsolutePath().toString(),
                "--watch",
                String.valueOf(gamePid),
                restartCmd.toAbsolutePath().toString(),
                gameDir)
                .directory(bsodDir.toFile())
                .start();

        boolean captured = !Files.readString(
                bsodDir.resolve("bsod_restart_cmd.txt")).isBlank();
        LOGGER.info("[BSOD] Windows crash watchdog started (pid {}, zero-intrusion, "
                + "restart cmd captured: {})", gamePid, captured);
    }

    private static void installLinuxWatchdog(Path bsodDir) throws IOException {
        Path overlay = extractResource("bsod_overlay_linux", bsodDir);
        overlay.toFile().setExecutable(true, false);
        Path restartCmd = writeRestartScript(bsodDir, false);

        long gamePid = ProcessHandle.current().pid();
        String gameDir = FMLPaths.GAMEDIR.get().toAbsolutePath().toString();

        new ProcessBuilder(overlay.toAbsolutePath().toString(),
                String.valueOf(gamePid),
                restartCmd.toAbsolutePath().toString(),
                gameDir)
                .directory(bsodDir.toFile())
                .start();

        LOGGER.info("[BSOD] Linux crash watchdog started (pid {}, zero-intrusion)", gamePid);
    }

    /** Extracts a bundled resource next to the game for native loading. */
    private static Path extractResource(String name, Path targetDir) throws IOException {
        Path target = targetDir.resolve(name);
        try (InputStream in = NativeCrashHook.class.getResourceAsStream("/" + name)) {
            if (in == null) {
                throw new IOException("resource not found: " + name);
            }
            Files.copy(in, target, java.nio.file.StandardCopyOption.REPLACE_EXISTING);
        }
        return target;
    }

    /**
     * Writes the restart scripts. The exact JVM command line is captured NOW,
     * while the game is alive. Primary source is ProcessHandle.commandLine();
     * under some launchers (e.g. PCL2) it comes back empty, so we fall back
     * to reassembling "java + args" from sun.java.command, and finally to
     * leaving a marker for a launcher-script fallback.
     */
    private static Path writeRestartScript(Path dir, boolean windows) throws IOException {
        String cmdLine = ProcessHandle.current().info().commandLine().orElse("");

        if (cmdLine.isBlank()) {
            // Reassemble: java executable + every arg after the main class.
            String java = ProcessHandle.current().info().command().orElse("java");
            String sun = System.getProperty("sun.java.command", "");
            if (!sun.isBlank()) {
                String afterMain = sun.substring(sun.indexOf(' ') + 1);
                cmdLine = "\"" + java + "\" " + afterMain;
            }
        }

        // The game dir may contain spaces (e.g. "... 1.21.1-NF-TESTMods") -
        // always launch through cmd with the /s /c quoting pattern, otherwise
        // cmd strips the outer quotes of quoted paths like
        // "C:\Program Files\Java\...\java.exe" and the relaunch explodes.
        if (windows) {
            Files.writeString(dir.resolve("bsod_restart_cmd.txt"), cmdLine,
                    StandardCharsets.UTF_8);
            Path cmd = dir.resolve("bsod_restart.cmd");
            Files.writeString(cmd,
                    "@echo off\r\n"
                    + "setlocal\r\n"
                    + "set /p MC_CMD=<\"%~dp0bsod_restart_cmd.txt\"\r\n"
                    + "if not defined MC_CMD exit /b 1\r\n"
                    + "cd /d \"%~dp0..\"\r\n"
                    + "start \"Minecraft\" /d \"%~dp0..\" cmd /s /c \"%MC_CMD%\"\r\n",
                    StandardCharsets.UTF_8);
            return cmd;
        }

        Path sh = dir.resolve("bsod_restart.sh");
        Files.writeString(sh,
                "#!/bin/sh\n"
                + "DIR=$(dirname \"$0\")\n"
                + "CMD=$(cat \"$DIR/bsod_restart_cmd.txt\")\n"
                + "[ -n \"$CMD\" ] && cd \"$DIR/..\" && sh -c \"$CMD\"\n",
                StandardCharsets.UTF_8);
        sh.toFile().setExecutable(true, false);
        return sh;
    }

    public static BsodState currentState() {
        return CrashCoordinator.activeState();
    }
}