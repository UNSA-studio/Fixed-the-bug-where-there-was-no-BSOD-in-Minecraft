package www.unsa.bsod.com.mixin;

import java.util.function.Supplier;

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
 * The takeover core.
 *
 * Defense line 1: {@code Minecraft#reportCrash} is the vanilla "we are crashing,
 * print the report, kill the process" routine. We intercept it before it gets
 * the chance to tear anything down.
 *
 * Defense line 2: even with reportCrash neutralised, the original exception may
 * still escape through the tick loop and end the game loop. We wrap the whole
 * per-frame tick so any escaped throwable is swallowed once we have taken over,
 * keeping the render loop alive to display the BSOD.
 */
@Mixin(Minecraft.class)
public abstract class MixinMinecraft {

    @Inject(method = "reportCrash", at = @At("HEAD"), cancellable = true)
    private void bsod$interceptReportCrash(CrashReport report, Supplier<String> detail, CallbackInfo ci) {
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
}