package www.unsa.bsod.com.report;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;

import www.unsa.bsod.com.BsodMod;
import www.unsa.bsod.com.crash.BsodState;

/**
 * Builds the human-readable plain text crash report and places the copies:
 * one inside {@code <game dir>/BSOD/}, one on the user's desktop (optional).
 * The AI analysis, when available, is appended to both.
 */
public final class ReportBuilder {

    private static final DateTimeFormatter FILE_STAMP =
            DateTimeFormatter.ofPattern("yyyyMMdd-HHmmss").withZone(ZoneId.systemDefault());
    private static final DateTimeFormatter HUMAN_STAMP =
            DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss (zzzz)").withZone(ZoneId.systemDefault());

    private static final String HEADER = """

            ==============================================================
             Fixed the bug where there was no BSOD in Minecraft
             UNSA Studio - We've fixed the missing BSOD in Minecraft! XD
            ==============================================================
            """;

    private ReportBuilder() {
    }

    public static String baseName(BsodState state) {
        return "bsod-report-" + FILE_STAMP.format(Instant.ofEpochMilli(state.context.timestampMillis()));
    }

    public static String buildReportText(BsodState state) {
        var ctx = state.context;
        StringBuilder sb = new StringBuilder(4096);
        sb.append(HEADER).append('\n');
        sb.append("Crash code   : ").append(ctx.crashCode()).append('\n');
        sb.append("Time         : ")
          .append(HUMAN_STAMP.format(Instant.ofEpochMilli(ctx.timestampMillis()))).append('\n');
        sb.append("Game version : ").append(ctx.gameVersion()).append('\n');
        sb.append("Involved mods: ").append('\n');
        for (String mod : ctx.involvedMods()) {
            sb.append("  - ").append(mod).append('\n');
        }
        sb.append("\nStack trace:\n").append(ctx.stacktrace()).append('\n');
        return sb.toString();
    }

    public static Path writeMainCopy(Path dumpFolder, BsodState state, String report) throws IOException {
        Files.createDirectories(dumpFolder);
        Path target = dumpFolder.resolve(baseName(state) + ".txt");
        Files.writeString(target, report, StandardCharsets.UTF_8,
                StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING);
        BsodMod.LOGGER.info("[BSOD] Report written to {}", target);
        return target;
    }

    public static void writeDesktopCopy(Path desktop, BsodState state, String report) throws IOException {
        Path target = desktop.resolve(baseName(state) + ".txt");
        Files.writeString(target, report, StandardCharsets.UTF_8,
                StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING);
        BsodMod.LOGGER.info("[BSOD] Desktop copy written to {}", target);
    }

    public static void appendAnalysis(Path reportFile, String analysis) {
        if (analysis == null || analysis.isBlank()) {
            return;
        }
        try {
            Files.writeString(reportFile, "\nAI analysis\n------------\n" + analysis.strip() + "\n",
                    StandardCharsets.UTF_8, StandardOpenOption.APPEND);
        } catch (IOException e) {
            BsodMod.LOGGER.warn("[BSOD] Could not append AI analysis to {}", reportFile, e);
        }
    }

    public static void writeDesktopAnalysisCopy(Path desktop, BsodState state,
                                                String originalReport, String analysis) throws IOException {
        Path target = desktop.resolve(baseName(state) + ".txt");
        String combined = originalReport
                + "\nAI analysis\n------------\n" + (analysis == null ? "" : analysis.strip()) + '\n';
        Files.writeString(target, combined, StandardCharsets.UTF_8,
                StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING);
    }
}