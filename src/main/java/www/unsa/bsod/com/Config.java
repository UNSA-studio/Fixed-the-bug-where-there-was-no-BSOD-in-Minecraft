package www.unsa.bsod.com;

import net.neoforged.neoforge.common.ModConfigSpec;

/**
 * Startup-scope configuration. Because it lives in the STARTUP config file it
 * is editable from the in-game Mods -> bsod -> Config screen, including the
 * AI API key.
 */
public final class Config {

    private static final ModConfigSpec.Builder BUILDER = new ModConfigSpec.Builder();

    public static final ModConfigSpec.BooleanValue AI_ENABLED = BUILDER
            .comment("When enabled and an API key is provided, crash reports are sent to an OpenAI-compatible chat API for analysis.")
            .define("ai.enabled", false);

    public static final ModConfigSpec.ConfigValue<String> AI_BASE_URL = BUILDER
            .comment("Root URL of an OpenAI-compatible chat completions endpoint.",
                    "Examples: https://api.openai.com/v1 , https://api.deepseek.com/v1 , https://api.moonshot.cn/v1 , http://localhost:11434/v1")
            .define("ai.baseUrl", "https://api.openai.com/v1");

    public static final ModConfigSpec.ConfigValue<String> AI_API_KEY = BUILDER
            .comment("Your secret API key. Keep it yours - it is only sent to the endpoint above.")
            .define("ai.apiKey", "");

    public static final ModConfigSpec.ConfigValue<String> AI_MODEL = BUILDER
            .comment("Model identifier used for the analysis request, e.g. gpt-4o-mini, deepseek-chat, moonshot-v1-8k, llama3.1.")
            .define("ai.model", "gpt-4o-mini");

    public static final ModConfigSpec.IntValue AI_TIMEOUT_SECONDS = BUILDER
            .comment("Per-request timeout for AI calls.")
            .defineInRange("ai.timeoutSeconds", 60, 5, 600);

    public static final ModConfigSpec.BooleanValue AUTO_RESTART = BUILDER
            .comment("After the report has been collected (and AI analysis finished, if enabled), relaunch the game instead of leaving a dead window.")
            .define("restart.autoRestart", true);

    public static final ModConfigSpec.IntValue RESTART_DELAY_SECONDS = BUILDER
            .comment("Seconds between takeover completion and the automatic relaunch. Shown as a countdown on the BSOD.")
            .defineInRange("restart.delaySeconds", 10, 1, 3600);

    public static final ModConfigSpec.BooleanValue SAVE_TO_DESKTOP = BUILDER
            .comment("Also drop a copy of the plain-text report onto the user's desktop.")
            .define("report.saveToDesktop", true);

    public static final ModConfigSpec.ConfigValue<String> DUMP_FOLDER_NAME = BUILDER
            .comment("Folder (inside the game root directory) that receives reports and dumps.")
            .define("report.dumpFolderName", "BSOD");

    public static final ModConfigSpec.ConfigValue<String> QR_TARGET_URL = BUILDER
            .comment("The QR code in the corner of the BSOD points here. Defaults to the official Minecraft website.")
            .define("display.qrTargetUrl", "https://www.minecraft.net");

    public static final ModConfigSpec SPEC = BUILDER.build();

    private Config() {
    }

    public static boolean aiEnabled() {
        return AI_ENABLED.get() && !AI_API_KEY.get().isBlank();
    }

    public static boolean autoRestart() {
        return AUTO_RESTART.get();
    }
}