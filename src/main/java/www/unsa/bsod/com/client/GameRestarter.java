package www.unsa.bsod.com.client;

import java.lang.management.ManagementFactory;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

import net.neoforged.fml.loading.FMLPaths;
import www.unsa.bsod.com.BsodMod;

/**
 * Restarts the game without closing the window.
 *
 * Strategy: replay our own process command line. {@link ProcessHandle#current()}
 * gives us the exact argv that launched this very instance - java binary, JVM
 * flags, classpath and main class included - so the new instance starts exactly
 * like the original one did, regardless of which launcher set it up.
 *
 * If replaying fails we log loudly and keep showing the BSOD.
 */
public final class GameRestarter {

    public static final String RESTART_MARKER_PROPERTY = "bsod.restarted";

    private GameRestarter() {
    }

    /** Attempts to spawn a fresh game instance. Returns true when spawned. */
    public static boolean restart() {
        try {
            List<String> command = new ArrayList<>(ProcessHandle.current().info().commandLine()
                    .map(GameRestarter::tokenize)
                    .orElseGet(ArrayList::new));

            if (command.isEmpty() || command.size() < 2) {
                BsodMod.LOGGER.warn("[BSOD] Could not determine the original command line; restart aborted");
                return false;
            }

            // Strip an old marker if present, then add ours so the child knows.
            command.removeIf(arg -> arg.contains(RESTART_MARKER_PROPERTY));
            command.add(command.size() - 1, "-D" + RESTART_MARKER_PROPERTY + "=true");

            Path workingDir = FMLPaths.GAMEDIR.get().toAbsolutePath();
            ProcessBuilder pb = new ProcessBuilder(command);
            pb.directory(workingDir.toFile());
            pb.inheritIO();

            Process process = pb.start();
            BsodMod.LOGGER.info("[BSOD] Restart spawned (pid {}): {}",
                    process.pid(), String.join(" ", command));
            return true;
        } catch (Throwable t) {
            BsodMod.LOGGER.error("[BSOD] Restart failed", t);
            return false;
        }
    }

    /**
     * Splits the raw command line back into argv tokens. On Windows the whole
     * line arrives as one string with quoting rules that are close enough to
     * POSIX for our purposes; on Linux it is already space-separated.
     */
    private static List<String> tokenize(String commandLine) {
        List<String> tokens = new ArrayList<>();
        StringBuilder current = new StringBuilder();
        boolean inQuotes = false;

        for (int i = 0; i < commandLine.length(); i++) {
            char c = commandLine.charAt(i);
            switch (c) {
                case '"' -> inQuotes = !inQuotes;
                case ' ', '\t' -> {
                    if (inQuotes) {
                        current.append(c);
                    } else if (!current.isEmpty()) {
                        tokens.add(current.toString());
                        current.setLength(0);
                    }
                }
                default -> current.append(c);
            }
        }
        if (!current.isEmpty()) {
            tokens.add(current.toString());
        }
        return tokens;
    }

    /** Fallback used only when ProcessHandle cannot provide the command line. */
    @SuppressWarnings("unused")
    private static List<String> fallbackCommand() {
        List<String> cmd = new ArrayList<>();
        String os = System.getProperty("os.name", "").toLowerCase();
        cmd.add(Path.of(System.getProperty("java.home"), "bin",
                os.contains("win") ? "java.exe" : "java").toString());

        // Carry over memory settings from the original JVM args.
        for (String arg : ManagementFactory.getRuntimeMXBean().getInputArguments()) {
            if (arg.startsWith("-Xmx") || arg.startsWith("-Xms")) {
                cmd.add(arg);
            }
        }
        cmd.add("-cp");
        cmd.add(System.getProperty("java.class.path", ""));
        cmd.add("-D" + RESTART_MARKER_PROPERTY + "=true");
        cmd.add(System.getProperty("bsod.fallbackMain",
                "net.neoforged.fml.loading.ImmediateWindowHandler"));
        return cmd;
    }
}