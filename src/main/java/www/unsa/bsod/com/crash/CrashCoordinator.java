package www.unsa.bsod.com.crash;

import com.mojang.logging.LogUtils;
import net.minecraft.CrashReport;
import net.minecraft.client.Minecraft;
import net.neoforged.api.distmarker.Dist;
import net.neoforged.fml.loading.FMLEnvironment;
import net.neoforged.fml.loading.FMLPaths;
import org.slf4j.Logger;
import www.unsa.bsod.com.Config;
import www.unsa.bsod.com.ai.AiAnalyzer;
import www.unsa.bsod.com.client.BsodScreen;
import www.unsa.bsod.com.dump.MinidumpGenerator;
import www.unsa.bsod.com.report.DesktopPaths;
import www.unsa.bsod.com.report.ReportBuilder;

/**
 * The conductor of the whole takeover.
 *
 * Flow: crash happens -> {@link #onVanillaCrash} or {@link #onEscapedThrowable}
 * -> build the {@link ModContext} once (single-flight guard) -> spawn a daemon
 * worker running the artifact pipeline -> flip the client over to the BSOD
 * screen immediately so the user sees Windows' finest.
 */
public final class CrashCoordinator {

    private static final Logger LOGGER = LogUtils.getLogger();

    private static volatile BsodState activeState;
    private static volatile boolean globalHandlerInstalled;

    private CrashCoordinator() {
    }

    /** Entry point from the {@code reportCrash} mixin. Returns true when we take over. */
    public static boolean onVanillaCrash(CrashReport report) {
        try {
            ModContext context = ModContext.from(CrashInfo.of(report));
            return takeOver(context);
        } catch (Throwable t) {
            LOGGER.error("[BSOD] Failed while preparing takeover", t);
            return false;
        }
    }

    /**
     * Entry point for throwables that escaped without ever reaching vanilla's
     * CrashReport machinery (or after our mixin cancelled it).
     */
    public static boolean onEscapedThrowable(Throwable throwable) {
        try {
            ModContext context = ModContext.from(
                    CrashInfo.raw(throwable, "Uncaught exception in game tick"));
            return takeOver(context);
        } catch (Throwable t) {
            LOGGER.error("[BSOD] Failed while preparing takeover", t);
            return false;
        }
    }

    private static boolean takeOver(ModContext context) {
        // Dedicated servers have neither a window nor a BSOD - stay out of the way.
        if (FMLEnvironment.dist != Dist.CLIENT) {
            return false;
        }

        // Single-flight: one crash is enough for everyone.
        if (activeState != null) {
            return true;
        }

        BsodState state = new BsodState(context);
        activeState = state;

        installGlobalFallbackHandler();

        Thread worker = new Thread(() -> runPipeline(state), "BSOD-Worker");
        worker.setDaemon(true);
        worker.start();

        Minecraft mc = Minecraft.getInstance();
        if (mc != null) {
            Runnable switcher = () -> {
                try {
                    if (!(mc.screen instanceof BsodScreen)) {
                        mc.setScreen(new BsodScreen(state));
                    }
                } catch (Throwable t) {
                    LOGGER.error("[BSOD] Failed to display the BSOD screen", t);
                }
            };
            if (mc.isSameThread()) {
                switcher.run();
            } else {
                mc.execute(switcher);
            }
        }
        return true;
    }

    /**
     * The artifact pipeline: plain-text report -> game folder copy -> minidump
     * -> desktop copy -> optional AI analysis (appended everywhere) ->
     * schedule restart. Runs on the dedicated worker thread only.
     */
    private static void runPipeline(BsodState state) {
        try {
            state.phase = "Collecting error data";
            String report = ReportBuilder.buildReportText(state);

            state.collectPercent = 25;
            var dumpFolder = FMLPaths.GAMEDIR.get().resolve(Config.DUMP_FOLDER_NAME.get());
            var reportFile = ReportBuilder.writeMainCopy(dumpFolder, state, report);
            state.collectPercent = 45;

            state.phase = "Writing memory dump";
            MinidumpGenerator.writeDump(
                    dumpFolder.resolve(ReportBuilder.baseName(state) + ".dmp"), state.context);
            state.collectPercent = 60;

            if (Config.SAVE_TO_DESKTOP.get()) {
                state.phase = "Copying report to your desktop";
                var desktop = DesktopPaths.desktopDirectory();
                if (desktop != null) {
                    ReportBuilder.writeDesktopCopy(desktop, state, report);
                }
                state.collectPercent = 70;
            }

            if (Config.aiEnabled()) {
                state.phase = "Asking the AI what went wrong";
                try {
                    String analysis = AiAnalyzer.analyze(state.context);
                    state.aiAnalysis = analysis;
                    ReportBuilder.appendAnalysis(reportFile, analysis);
                    if (Config.SAVE_TO_DESKTOP.get()) {
                        var desktop = DesktopPaths.desktopDirectory();
                        if (desktop != null) {
                            ReportBuilder.writeDesktopAnalysisCopy(desktop, state, report, analysis);
                        }
                    }
                } catch (Throwable t) {
                    LOGGER.warn("[BSOD] AI analysis failed", t);
                    state.aiFailed = true;
                }
            }

            state.collectPercent = 100;
            state.phase = "Done";
            state.pipelineDone = true;

            if (Config.autoRestart()) {
                state.restartAtMillis = System.currentTimeMillis()
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
            }
        } catch (Throwable t) {
            LOGGER.error("[BSOD] The crash handler itself crashed (ironic)", t);
            state.phase = "Failed to finish collecting data";
            state.pipelineDone = true;
            if (Config.autoRestart()) {
                state.restartAtMillis = System.currentTimeMillis()
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
            }
        }
    }

    /**
     * Last line of defense: anything that dies without going through either
     * vanilla's crash path or our tick wrap still lands here and triggers a
     * full takeover instead of silently killing the process.
     */
    public static synchronized void installGlobalFallbackHandler() {
        if (globalHandlerInstalled) {
            return;
        }
        Thread.setDefaultUncaughtExceptionHandler((thread, throwable) -> {
            if (thread != null && thread.getName().startsWith("BSOD-")) {
                return; // never recurse into ourselves
            }
            onEscapedThrowable(throwable);
        });
        globalHandlerInstalled = true;
    }

    /** Lifecycle hook reserved for the mod constructor's client-setup event. */
    public static void markClientReady() {
        // Nothing to do today; kept as a deliberate seam in the lifecycle.
    }

    public static BsodState activeState() {
        return activeState;
    }
}