package www.unsa.bsod.com.crash;

import java.io.IOException;
import java.io.InputStream;
import java.lang.management.ManagementFactory;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import com.mojang.logging.LogUtils;
import net.neoforged.fml.loading.FMLPaths;
import org.slf4j.Logger;

/**
 * Sets up native-crash detection.
 *
 * Windows: loads a tiny DLL that registers an unhandled-exception filter.
 * Such a filter only fires when NOTHING handled the exception - i.e. the JVM
 * is genuinely dying. HotSpot's benign page faults (implicit null checks!)
 * are consumed by SEH long before this, so the infamous false-positive of the
 * old vectored-handler approach is structurally impossible now.
 *
 * Linux: starts a zero-intrusion watchdog process that polls the game pid and
 * watches for a fresh hs_err log. No signals are touched, so the JVM's own
 * crash machinery stays completely untouched.
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
        Path dll = extractResource("bsod_crash_hook.dll", bsodDir);
        Path overlay = extractResource("bsod_overlay.exe", bsodDir);
        Path restartCmd = writeRestartScript(bsodDir, true);

        System.load(dll.toAbsolutePath().toString());
        install0(overlay.toAbsolutePath().toString(),
                restartCmd.toAbsolutePath().toString());
        LOGGER.info("[BSOD] Native crash hook installed (windows, unhandled-exception filter)");
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
     * while the game is alive - after a crash the process no longer exists,
     * so any "find the running java process" trick would find nothing.
     */
    private static Path writeRestartScript(Path dir, boolean windows) throws IOException {
        String cmdLine = ProcessHandle.current().info().commandLine().orElse("");
        Files.writeString(dir.resolve("bsod_restart_cmd.txt"), cmdLine,
                StandardCharsets.UTF_8);

        if (windows) {
            Files.writeString(dir.resolve("bsod_restart.ps1"),
                    "$cmd = Get-Content -LiteralPath "
                        + "(Join-Path $PSScriptRoot 'bsod_restart_cmd.txt') -Raw\n"
                        + "if ($cmd) { Invoke-Expression $cmd.Trim() }\n",
                    StandardCharsets.UTF_8);
            Path cmd = dir.resolve("bsod_restart.cmd");
            Files.writeString(cmd,
                    "@echo off\r\n"
                    + "rem Relaunch Minecraft with the command line captured at install time.\r\n"
                    + "powershell -NoProfile -ExecutionPolicy Bypass -File "
                    + "\"%~dp0bsod_restart.ps1\"\r\n",
                    StandardCharsets.UTF_8);
            return cmd;
        }

        Path sh = dir.resolve("bsod_restart.sh");
        Files.writeString(sh,
                "#!/bin/sh\n"
                + "DIR=$(dirname \"$0\")\n"
                + "CMD=$(cat \"$DIR/bsod_restart_cmd.txt\")\n"
                + "[ -n \"$CMD\" ] && sh -c \"$CMD\"\n",
                StandardCharsets.UTF_8);
        sh.toFile().setExecutable(true, false);
        return sh;
    }

    public static BsodState currentState() {
        return CrashCoordinator.activeState();
    }

    private static native void install0(String overlayPath, String restartPath);

    /** Keeps the import used for potential future pid needs. */
    @SuppressWarnings("unused")
    private static String currentJvmPid() {
        return ManagementFactory.getRuntimeMXBean().getName();
    }
}