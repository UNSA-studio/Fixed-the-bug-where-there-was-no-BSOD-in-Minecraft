package www.unsa.bsod.com.report;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.TimeUnit;

import www.unsa.bsod.com.BsodMod;

/**
 * Resolves the user's real desktop directory across platforms.
 * On Windows it asks the registry first (OneDrive redirection and friends),
 * on Linux it consults xdg-user-dir, on macOS it is simply ~/Desktop.
 */
public final class DesktopPaths {

    private DesktopPaths() {
    }

    public static Path desktopDirectory() {
        String os = System.getProperty("os.name", "").toLowerCase(java.util.Locale.ROOT);

        try {
            if (os.contains("win")) {
                Path fromRegistry = windowsDesktopFromRegistry();
                if (fromRegistry != null) {
                    return fromRegistry;
                }
            } else if (os.contains("mac")) {
                return simpleHomeDesktop();
            } else {
                Path fromXdg = linuxDesktopFromXdg();
                if (fromXdg != null) {
                    return fromXdg;
                }
            }
        } catch (Throwable t) {
            BsodMod.LOGGER.debug("[BSOD] Desktop detection failed for platform {}", os, t);
        }

        return simpleHomeDesktop();
    }

    private static Path windowsDesktopFromRegistry() throws IOException, InterruptedException {
        ProcessBuilder pb = new ProcessBuilder(
                "reg", "query",
                "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders",
                "/v", "Desktop");
        pb.redirectErrorStream(true);
        Process process = pb.start();
        String output = new String(process.getInputStream().readAllBytes(), StandardCharsets.UTF_8);
        process.waitFor(5, TimeUnit.SECONDS);

        for (String line : output.split("\r?\n")) {
            int typeIdx = line.indexOf("REG_EXPAND_SZ");
            if (typeIdx < 0) {
                typeIdx = line.indexOf("REG_SZ");
            }
            if (typeIdx < 0) {
                continue;
            }
            String raw = line.substring(typeIdx).replaceFirst("REG(_EXPAND)?_SZ\\s*", "").trim();
            String expanded = expandWindowsVariables(raw);
            if (!expanded.isBlank()) {
                Path candidate = Path.of(expanded);
                if (Files.isDirectory(candidate)) {
                    return candidate;
                }
            }
        }
        return null;
    }

    private static String expandWindowsVariables(String value) {
        StringBuilder sb = new StringBuilder();
        int i = 0;
        while (i < value.length()) {
            char c = value.charAt(i);
            if (c == '%' && i + 1 < value.length()) {
                int end = value.indexOf('%', i + 1);
                if (end > i + 1) {
                    String var = value.substring(i + 1, end);
                    String resolved = System.getenv(var);
                    if (resolved != null) {
                        sb.append(resolved);
                        i = end + 1;
                        continue;
                    }
                }
            }
            sb.append(c);
            i++;
        }
        return sb.toString();
    }

    private static Path linuxDesktopFromXdg() throws IOException, InterruptedException {
        try {
            Process process = new ProcessBuilder("xdg-user-dir", "DESKTOP").start();
            String out = new String(process.getInputStream().readAllBytes(),
                    StandardCharsets.UTF_8).trim();
            process.waitFor(5, TimeUnit.SECONDS);
            if (!out.isBlank() && !out.equals("x-desktop")) {
                Path candidate = Path.of(out);
                if (Files.isDirectory(candidate)) {
                    return candidate;
                }
            }
        } catch (IOException e) {
            // xdg-user-dir not installed - fall through.
        }
        return null;
    }

    private static Path simpleHomeDesktop() {
        try {
            Path candidate = Path.of(System.getProperty("user.home"), "Desktop");
            if (Files.isDirectory(candidate)) {
                return candidate;
            }
        } catch (Throwable ignored) {
        }
        return null;
    }
}