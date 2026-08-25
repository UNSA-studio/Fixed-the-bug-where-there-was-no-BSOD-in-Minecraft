package www.unsa.bsod.com.crash;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Set;

import net.minecraft.CrashReport;
import net.neoforged.fml.ModList;
import net.neoforged.neoforgespi.language.IModInfo;

/**
 * Figures out which loaded mods are involved in a crash.
 *
 * Two independent signals, merged and de-duplicated:
 * 1. NeoForge's own "Suspected Mods" line inside the crash report system details.
 * 2. A scan of the full stack trace against every loaded mod's ID and display name.
 *
 * If nothing can be determined the caller falls back to a single "Unknown" entry,
 * exactly as specified.
 */
public final class ModListDetector {

    private static final Set<String> NON_MOD_IDS = Set.of(
            "minecraft", "neoforge", "forge", "fml", "mixinextras", "java", "minecraftforge");

    private ModListDetector() {
    }

    /**
     * Full detection for a real {@link CrashReport}.
     * Never returns {@code null}; may return an empty list.
     */
    public static List<String> detectFromReport(CrashReport report) {
        Set<String> hits = new LinkedHashSet<>();

        // Signal 1: the loader's own suspicion list, when available.
        String details = safeSystemDetails(report);
        int idx = details.indexOf("Suspected Mods:");
        if (idx >= 0) {
            String tail = details.substring(idx + "Suspected Mods:".length());
            int eol = tail.indexOf('\n');
            String line = (eol >= 0 ? tail.substring(0, eol) : tail).trim();
            for (String token : line.split("[,;|]+")) {
                String id = token.trim();
                if (!id.isEmpty() && !id.equalsIgnoreCase("none")) {
                    hits.add(prettyById(id));
                }
            }
        }

        // Signal 2: brute-force scan of the stack trace against all loaded mods.
        Throwable cause = report.getException();
        String trace = cause == null ? "" : stackTraceOf(cause);
        matchLoadedMods(hits, trace);

        return new ArrayList<>(hits);
    }

    /**
     * Stack-trace-only variant, used for throwables that never went through
     * vanilla's CrashReport machinery.
     */
    public static List<String> detectInStackTrace(String stackTrace) {
        Set<String> hits = new LinkedHashSet<>();
        matchLoadedMods(hits, stackTrace == null ? "" : stackTrace);
        return new ArrayList<>(hits);
    }

    private static void matchLoadedMods(Set<String> hits, String haystack) {
        if (haystack.isBlank()) {
            return;
        }
        String lower = haystack.toLowerCase(Locale.ROOT);
        try {
            for (IModInfo mod : ModList.get().getMods()) {
                String id = mod.getModId();
                if (NON_MOD_IDS.contains(id.toLowerCase(Locale.ROOT))) {
                    continue;
                }
                String idNeedle = id.toLowerCase(Locale.ROOT).replace('-', '_');
                String display = mod.getDisplayName() == null ? "" : mod.getDisplayName().trim();
                boolean idHit = lower.contains(idNeedle);
                boolean displayHit = display.length() >= 3 && lower.contains(display.toLowerCase(Locale.ROOT));
                if (idHit || displayHit) {
                    hits.add(pretty(mod));
                }
            }
        } catch (Throwable ignored) {
            // ModList not ready or broken - detection simply yields fewer results.
        }
    }

    private static String pretty(IModInfo mod) {
        String display = mod.getDisplayName();
        if (display == null || display.isBlank()) {
            return mod.getModId();
        }
        return display + " (" + mod.getModId() + ")";
    }

    private static String prettyById(String id) {
        try {
            return ModList.get().getModContainerById(id)
                    .map(container -> pretty(container.getModInfo()))
                    .orElse(id);
        } catch (Throwable ignored) {
            return id;
        }
    }

    private static String safeSystemDetails(CrashReport report) {
        try {
            String s = report.getSystemDetails();
            return s == null ? "" : s;
        } catch (Throwable ignored) {
            return "";
        }
    }

    private static String stackTraceOf(Throwable t) {
        StringWriter sw = new StringWriter();
        t.printStackTrace(new PrintWriter(sw));
        return sw.toString();
    }
}