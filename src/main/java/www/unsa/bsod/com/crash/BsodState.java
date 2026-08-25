package www.unsa.bsod.com.crash;

/**
 * Live, cross-thread view of the takeover pipeline.
 * The render thread only ever reads these fields; the worker thread writes them.
 */
public final class BsodState {

    /** Human readable pipeline phase, shown under the spinner. */
    public volatile String phase = "Collecting error data";

    /** Fake-but-honest collection progress, 0..100. */
    public volatile int collectPercent = 0;

    /** Result of the optional AI analysis; {@code null} until available. */
    public volatile String aiAnalysis;

    /** Set when the AI call was enabled but failed. */
    public volatile boolean aiFailed;

    /** True once every artifact has been written and the AI step finished. */
    public volatile boolean pipelineDone;

    /** Wall-clock time at which the restart countdown hits zero; -1 until scheduled. */
    public volatile long restartAtMillis = -1L;

    public final ModContext context;
    public final long takenOverAtMillis;

    public BsodState(ModContext context) {
        this.context = context;
        this.takenOverAtMillis = context.timestampMillis();
    }
}