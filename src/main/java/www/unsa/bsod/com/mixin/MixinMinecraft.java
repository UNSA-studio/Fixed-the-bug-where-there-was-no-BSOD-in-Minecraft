package www.unsa.bsod.com.mixin;

import com.llamalad7.mixinextras.injector.wrapmethod.WrapMethod;
import com.llamalad7.mixinextras.injector.wrapoperation.Operation;
import net.minecraft.CrashReport;
import net.minecraft.client.Minecraft;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import www.unsa.bsod.com.crash.CrashCoordinator;

/**
 * The takeover core, targeting the real 1.21.1 crash pipeline (verified
 * against Mojang's official 1.21.1 mappings):
 *
 *   emergencySaveAndCrash(CrashReport)  - the main "we are crashing now" entry
 *   delayCrash / delayCrashRaw          - deferred crash screen scheduling
 *   runTick(boolean)                    - per-frame work, wrapped for escapes
 *   run()                               - whole game loop, wrapped last
 */
@Mixin(Minecraft.class)
public abstract class MixinMinecraft {

    @Inject(method = "emergencySaveAndCrash", at = @At("HEAD"), cancellable = true)
    private void bsod$interceptEmergencySaveAndCrash(CrashReport report, CallbackInfo ci) {
        if (CrashCoordinator.onVanillaCrash(report)) {
            ci.cancel();
        }
    }

    @Inject(method = "delayCrash", at = @At("HEAD"), cancellable = true)
    private void bsod$interceptDelayCrash(CrashReport report, CallbackInfo ci) {
        if (CrashCoordinator.onVanillaCrash(report)) {
            ci.cancel();
        }
    }

    @Inject(method = "delayCrashRaw", at = @At("HEAD"), cancellable = true)
    private void bsod$interceptDelayCrashRaw(CrashReport report, CallbackInfo ci) {
        if (CrashCoordinator.onVanillaCrash(report)) {
            ci.cancel();
        }
    }

    @WrapMethod(method = "runTick")
    private void bsod$swallowTickCrash(boolean renderLevel, Operation<Void> original) {
        try {
            original.call(renderLevel);
        } catch (Throwable throwable) {
            if (!CrashCoordinator.onEscapedThrowable(throwable)) {
                throw throwable;
            }
        }
    }

    /**
     * Defense line: exceptions that escape the per-frame tick wrap entirely -
     * e.g. raised in the parts of the game loop outside {@code runTick}. The
     * normal loop is dead at that point, so we hand over to a minimal
     * self-driven fallback loop that keeps the window alive and blue.
     */
    @WrapMethod(method = "run")
    private void bsod$guardWholeGameLoop(Operation<Void> original) {
        try {
            original.call();
        } catch (Throwable throwable) {
            if (CrashCoordinator.onEscapedThrowable(throwable)) {
                CrashCoordinator.runFallbackLoop();
                return;
            }
            throw throwable;
        }
    }
}