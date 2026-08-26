package www.unsa.bsod.com.crash;

import com.mojang.logging.LogUtils;
import net.minecraft.CrashReport;
import net.minecraft.client.Minecraft;
import net.neoforged.api.distmarker.Dist;
import net.neoforged.fml.loading.FMLEnvironment;
import net.neoforged.fml.loading.FMLPaths;
import org.lwjgl.glfw.GLFW;
import org.lwjgl.opengl.GL11;
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
     * -> desktop copy -> optional AI analysis -> schedule restart.
     *
     * When AI analysis is enabled the percentage deliberately refuses to reach
     * 100 until the AI has actually answered: a daemon crawler creeps the bar
     * up towards 99% for as long as the request is in flight. Runs on the
     * dedicated worker thread only.
     */
    private static void runPipeline(BsodState state) {
        Thread aiProgressCrawler = null;
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
            state.collectPercent = 55;

            if (Config.SAVE_TO_DESKTOP.get()) {
                state.phase = "Copying report to your desktop";
                var desktop = DesktopPaths.desktopDirectory();
                if (desktop != null) {
                    ReportBuilder.writeDesktopCopy(desktop, state, report);
                }
                state.collectPercent = 60;
            }

            if (Config.aiEnabled()) {
                state.phase = "Waiting for the AI to finish its analysis";

                // The bar must stay below 100 until the AI has spoken.
                aiProgressCrawler = new Thread(() -> {
                    int virtual = state.collectPercent;
                    while (virtual < 99 && !state.pipelineDone) {
                        try {
                            Thread.sleep(1500);
                        } catch (InterruptedException e) {
                            return;
                        }
                        virtual++;
                        state.collectPercent = Math.min(virtual, 99);
                    }
                }, "BSOD-ProgressCrawler");
                aiProgressCrawler.setDaemon(true);
                aiProgressCrawler.start();

                try {
                    String analysis = AiAnalyzer.analyze(state.context, report);
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
            state.phase = Config.aiEnabled() ? "AI analysis complete - restarting soon" : "Done";
            state.pipelineDone = true;

            if (Config.autoRestart()) {
                state.restartAtMillis = System.currentTimeMillis()
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
            }
        } catch (Throwable t) {
            LOGGER.error("[BSOD] The crash handler itself crashed (ironic)", t);
            state.phase = "Failed to finish collecting data";
            state.pipelineDone = true;
            state.collectPercent = 100;
            if (Config.autoRestart()) {
                state.restartAtMillis = System.currentTimeMillis()
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
            }
        } finally {
            if (aiProgressCrawler != null) {
                aiProgressCrawler.interrupt();
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

        /**
     * Minimal self-driven render loop used when even the vanilla game loop is
     * dead. Draws nothing but a flat Windows-blue clear color and keeps the
     * GLFW window responsive until the user closes it or the restart fires.
     * Deliberately touches no Minecraft rendering code - only raw GLFW/GL.
     */
    public static void runFallbackLoop() {
        try {
            long window = 0;
            try {
                var win = Minecraft.getInstance().getWindow();
                if (win != null) {
                    window = win.getWindow();
                }
            } catch (Throwable ignored) {
            }
            if (window == 0) {
                return;
            }
            LOGGER.info("[BSOD] Entering fallback blue loop (vanilla game loop is dead)");

            // Windows blue #0078D7 as normalized RGB.
            GL11.glClearColor(0.0F, 0x78 / 255.0F, 0xD7 / 255.0F, 1.0F);
            while (!GLFW.glfwWindowShouldClose(window)) {
                GL11.glClear(GL11.GL_COLOR_BUFFER_BIT | GL11.GL_DEPTH_BUFFER_BIT);
                GLFW.glfwSwapBuffers(window);
                GLFW.glfwPollEvents();

                BsodState s = activeState;
                if (s != null && s.restartAtMillis > 0 && !s.fallbackRestartSpawned
                        && System.currentTimeMillis() >= s.restartAtMillis) {
                    s.fallbackRestartSpawned = www.unsa.bsod.com.client.GameRestarter.restart();
                }
                try {
                    Thread.sleep(16);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        } catch (Throwable t) {
            LOGGER.error("[BSOD] Fallback loop died", t);
        }
    }

    /**
     * Shutdown hook: if something calls {@code System.exit} while the artifact
     * pipeline is still running, give the worker a short grace period so the
     * report/dump actually land on disk before the JVM dies.
     */
    public static void installShutdownHook() {
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            BsodState s = activeState;
            if (s != null && !s.pipelineDone) {
                try {
                    Thread.sleep(8000); // grace period for the daemon worker
                } catch (InterruptedException ignored) {
                }
            }
        }, "BSOD-ShutdownWaiter"));
    }

    public static BsodState activeState() {
        return activeState;
    }
}