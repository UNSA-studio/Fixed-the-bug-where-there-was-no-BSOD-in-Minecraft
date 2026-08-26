package www.unsa.bsod.com;

import net.neoforged.bus.api.IEventBus;
import net.neoforged.fml.ModContainer;
import net.neoforged.fml.common.Mod;
import net.neoforged.fml.config.ModConfig;
import net.neoforged.fml.event.lifecycle.FMLClientSetupEvent;
import net.neoforged.fml.event.lifecycle.FMLCommonSetupEvent;
import net.neoforged.fml.loading.FMLEnvironment;
import net.neoforged.api.distmarker.Dist;
import org.slf4j.Logger;
import com.mojang.logging.LogUtils;
import www.unsa.bsod.com.crash.CrashCoordinator;
import www.unsa.bsod.com.crash.NativeCrashHook;

/**
 * Fixed the bug where there was no BSOD in Minecraft.
 *
 * "We've fixed the missing BSOD in Minecraft!
 *  Now you can feast on tasty BSOD errors! XD"
 *
 * UNSA Studio - MIT licensed.
 */
@Mod(BsodMod.MOD_ID)
public final class BsodMod {

    public static final String MOD_ID = "bsod";
    public static final Logger LOGGER = LogUtils.getLogger();

    public BsodMod(IEventBus modEventBus, ModContainer modContainer) {
        modContainer.registerConfig(ModConfig.Type.STARTUP, Config.SPEC);

        modEventBus.addListener(this::onCommonSetup);

        if (FMLEnvironment.dist == Dist.CLIENT) {
            modEventBus.addListener(this::onClientSetup);
            NativeCrashHook.install();
        }

        LOGGER.info("[BSOD] Fixed the bug where there was no BSOD in Minecraft - loaded. XD");
    }

    private void onCommonSetup(final FMLCommonSetupEvent event) {
        CrashCoordinator.installGlobalFallbackHandler();
        CrashCoordinator.installShutdownHook();
    }

    private void onClientSetup(final FMLClientSetupEvent event) {
        CrashCoordinator.markClientReady();
    }
}