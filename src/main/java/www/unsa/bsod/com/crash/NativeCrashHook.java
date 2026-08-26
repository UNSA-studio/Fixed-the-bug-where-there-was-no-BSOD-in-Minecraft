package www.unsa.bsod.com.crash;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

import com.mojang.logging.LogUtils;
import net.neoforged.fml.loading.FMLPaths;
import org.slf4j.Logger;

/**
 * Loads a tiny native hook (Windows only) that registers a vectored exception
 * handler. When the JVM itself dies from an access violation or fail-fast, our
 * handler runs first and launches a PowerShell overlay that draws the classic
 * blue screen while the JVM writes its own hs_err log.
 *
 * The DLL is extracted to the BSOD folder at runtime; if anything fails we
 * simply stay silent - native crashes then behave exactly like vanilla.
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
            if (!os.contains("win")) {
                LOGGER.info("[BSOD] Native crash hook is Windows-only, skipping");
                return;
            }

            Path bsodDir = FMLPaths.GAMEDIR.get().resolve("BSOD");
            Files.createDirectories(bsodDir);

            Path dll = extractResource("bsod_crash_hook.dll", bsodDir);
            Path script = extractResource("bsod_overlay.ps1", bsodDir);
            Path restartCmd = writeRestartScript(bsodDir);

            System.load(dll.toAbsolutePath().toString());
            install0(script.toAbsolutePath().toString(),
                    restartCmd.toAbsolutePath().toString());
            LOGGER.info("[BSOD] Native crash hook installed - even hard JVM crashes will now show blue");
        } catch (Throwable t) {
            LOGGER.warn("[BSOD] Native crash hook unavailable: {}", t.toString());
        }
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
     * Writes a tiny .cmd that relaunches Minecraft via PCL's own launch script.
     * Kept as a separate file because embedding quotes inside the C string was
     * getting silly.
     */
    private static Path writeRestartScript(Path dir) throws IOException {
        Path cmd = dir.resolve("bsod_restart.cmd");
        StringBuilder sb = new StringBuilder();
        sb.append("@echo off\r\n");
        sb.append("rem Relaunch Minecraft through its launcher script after a crash.\r\n");
        sb.append("if exist \"%~dp0..\\..\\launch.bat\" (\r\n");
        sb.append("  start \"\" \"%~dp0..\\..\\launch.bat\"\r\n");
        powershellFallback(sb);
        sb.append("\r\n}\r\n");
        Files.writeString(cmd, sb.toString(), StandardCharsets.UTF_8);
        return cmd;
    }

    private static void powershellFallback(StringBuilder sb) {
        sb.append(") else (\r\n");
        sb.append("  rem Fallback: ask PowerShell to find and replay the last java command line.\r\n");
        sb.append("  powershell -NoProfile -Command \"")
          .append("$p=Get-CimInstance Win32_Process -Filter \\\"Name LIKE 'java%'\\\" ")
          .append("| ? { $_.CommandLine -match 'BootstrapLauncher' } | select -f 1; ")
          .append("if($p){ Start-Process cmd -ArgumentList \\\"/c $($p.CommandLine)\\\" }")
          .append("\"\r\n");
    }

    public static BsodState currentState() {
        return CrashCoordinator.activeState();
    }

    private static native void install0(String scriptPath, String restartPath);
}