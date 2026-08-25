package www.unsa.bsod.com.crash;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;

import net.minecraft.CrashReport;
import net.minecraft.CrashReportCategory;

/**
 * Extracts the interesting bits out of a {@link CrashReport}:
 * the crash code (exception class + message) and every mod that appears
 * in the stack trace. Mods that cannot be determined are reported as "Unknown",
 * as requested.
 */
public final class CrashInfo {

    private final String title;
    private final String description;
    private final String exceptionClass;
    private final String exceptionMessage;
    private final List<String> involvedMods = new ArrayList<>();
    private final String fullStacktrace;

    private CrashInfo(CrashReport report) {
        this.title = report.getTitle();
        this.description = report.getDescription();

        Throwable t = report.getException();
        this.exceptionClass = t == null ? "Unknown" : t.getClass().getName();
        this.exceptionMessage = (t == null || t.getMessage() == null) ? "" : t.getMessage();

        StringWriter sw = new StringWriter();
        if (t != null) {
            t.printStackTrace(new PrintWriter(sw));
        }
        this.fullStacktrace = sw.toString();

        for (String mod : ModListDetector.modsInStackTrace(fullStacktrace)) {
            if (!involvedMods.contains(mod)) {
                involvedMods.add(mod);
            }
        }
    }

    public static CrashInfo of(CrashReport report) {
        return new CrashInfo(report);
    }

    public static CrashInfo raw(Throwable throwable, String description) {
        CrashReportCategory dummy = null; // not used, kept simple
        return new CrashInfo(wrap(throwable, description));
    }

    private static CrashReport wrap(Throwable t, String description) {
        // A minimal fake report so we can reuse the same extraction path.
        try {
            CrashReport report = CrashReport.forThrowable(t, description);
            return report;
        } catch (Throwable ignored) {
            // Extremely early crashes before bootstrap - build a shell object via reflection-free path.
            return new EmptyReport(t, description).toCrashReport();
        }
    }

    public String title() {
        return title;
    }

    public String description() {
        return description;
    }

    /** One-line crash code, e.g. {@code java.lang.NullPointerException: Cannot invoke ...} */
    public String crashCode() {
        if (exceptionMessage == null || exceptionMessage.isBlank()) {
            return exceptionClass;
        }
        return exceptionClass + ": " + exceptionMessage;
    }

    public String exceptionClass() {
        return exceptionClass;
    }

    /** Never empty: falls back to a single "Unknown" entry. */
    public List<String> involvedModsOrUnknown() {
        if (involvedMods.isEmpty()) {
            return List.of("Unknown");
        }
        return List.copyOf(involvedMods);
    }

    public String stacktrace() {
        return fullStacktrace.isBlank()
                ? "(stack trace unavailable)"
                : fullStacktrace;
    }

    /** Tiny adapter used only when even CrashReport cannot be constructed yet. */
    private record EmptyReport(Throwable cause, String text) {
        CrashReport toCrashReport() {
            return CrashReport.forThrowable(cause, text);
        }
    }
}
