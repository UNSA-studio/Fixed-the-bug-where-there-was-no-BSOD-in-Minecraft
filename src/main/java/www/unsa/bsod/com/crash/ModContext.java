package www.unsa.bsod.com.crash;

import java.util.List;

/**
 * Immutable snapshot of everything the BSOD pipeline needs to know about one crash.
 */
public final class ModContext {

    private final String crashCode;
    private final List<String> involvedMods;
    private final String stacktrace;
    private final long timestampMillis;
    private final String gameVersion;

    public ModContext(String crashCode, List<String> involvedMods, String stacktrace,
                      long timestampMillis, String gameVersion) {
        this.crashCode = crashCode;
        this.involvedMods = List.copyOf(involvedMods);
        this.stacktrace = stacktrace == null ? "" : stacktrace;
        this.timestampMillis = timestampMillis;
        this.gameVersion = gameVersion;
    }

    public static ModContext from(CrashInfo info) {
        return new ModContext(
                info.crashCode(),
                info.involvedModsOrUnknown(),
                info.stacktrace(),
                System.currentTimeMillis(),
                net.minecraft.SharedConstants.getCurrentVersion().getName());
    }

    public String crashCode() {
        return crashCode;
    }

    public List<String> involvedMods() {
        return involvedMods;
    }

    /** Comma-joined mod list for compact display. */
    public String involvedModsJoined() {
        return String.join(", ", involvedMods);
    }

    public String stacktrace() {
        return stacktrace;
    }

    public long timestampMillis() {
        return timestampMillis;
    }

    public String gameVersion() {
        return gameVersion;
    }
}