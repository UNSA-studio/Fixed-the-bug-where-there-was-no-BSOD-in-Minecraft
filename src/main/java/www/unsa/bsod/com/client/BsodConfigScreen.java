package www.unsa.bsod.com.client;

import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.components.AbstractWidget;
import net.minecraft.client.gui.components.Button;
import net.minecraft.client.gui.components.EditBox;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.CommonComponents;
import net.minecraft.network.chat.Component;
import www.unsa.bsod.com.Config;

/**
 * In-game configuration screen so users can actually set the AI API key and
 * friends from the Mods menu (the vanilla config button would otherwise be a
 * dead, black do-nothing stub).
 */
public final class BsodConfigScreen extends Screen {

    private final Screen parent;

    private EditBox baseUrlBox;
    private EditBox apiKeyBox;
    private EditBox modelBox;
    private Button aiEnabledButton;
    private Button restartButton;

    private boolean aiEnabled;
    private boolean autoRestart;

    public BsodConfigScreen(Screen parent) {
        super(Component.literal("BSOD Config"));
        this.parent = parent;
        this.aiEnabled = Config.AI_ENABLED.get();
        this.autoRestart = Config.AUTO_RESTART.get();
    }

    @Override
    protected void init() {
        int centerX = this.width / 2;
        int fieldWidth = 260;
        int left = centerX - fieldWidth / 2;
        int y = Math.max(40, this.height / 6);

        addRenderableWidget(makeLabel("AI analysis (OpenAI-compatible)", left, y));
        y += 14;

        aiEnabledButton = addRenderableWidget(Button.builder(
                        toggleLabel("Enabled", aiEnabled), b -> {
                            aiEnabled = !aiEnabled;
                            aiEnabledButton.setMessage(toggleLabel("Enabled", aiEnabled));
                        })
                .bounds(left, y, 120, 20)
                .build());
        y += 26;

        addRenderableWidget(makeLabel("Base URL", left, y));
        y += 12;
        baseUrlBox = addRenderableWidget(new EditBox(this.font, left, y, fieldWidth, 18,
                Component.literal("Base URL")));
        baseUrlBox.setMaxLength(256);
        baseUrlBox.setValue(Config.AI_BASE_URL.get());
        y += 24;

        addRenderableWidget(makeLabel("API key", left, y));
        y += 12;
        apiKeyBox = addRenderableWidget(new EditBox(this.font, left, y, fieldWidth, 18,
                Component.literal("API key")));
        apiKeyBox.setMaxLength(256);
        apiKeyBox.setValue(Config.AI_API_KEY.get());
        y += 24;

        addRenderableWidget(makeLabel("Model", left, y));
        y += 12;
        modelBox = addRenderableWidget(new EditBox(this.font, left, y, fieldWidth, 18,
                Component.literal("Model")));
        modelBox.setMaxLength(128);
        modelBox.setValue(Config.AI_MODEL.get());
        y += 28;

        restartButton = addRenderableWidget(Button.builder(
                        toggleLabel("Auto restart after crash", autoRestart), b -> {
                            autoRestart = !autoRestart;
                            restartButton.setMessage(toggleLabel("Auto restart after crash", autoRestart));
                        })
                .bounds(left, y, fieldWidth, 20)
                .build());

        addRenderableWidget(Button.builder(CommonComponents.GUI_DONE, b -> onSave())
                .bounds(centerX - 100, this.height - 28, 200, 20)
                .build());
    }

    private AbstractWidget makeLabel(String text, int x, int y) {
        return new LabelStub(x, y, text);
    }

    private static Component toggleLabel(String base, boolean value) {
        return Component.literal(base + ": " + (value ? "ON" : "OFF"));
    }

    private void onSave() {
        Config.AI_ENABLED.set(aiEnabled);
        Config.AI_BASE_URL.set(baseUrlBox.getValue().trim());
        Config.AI_API_KEY.set(apiKeyBox.getValue().trim());
        Config.AI_MODEL.set(modelBox.getValue().trim());
        Config.AUTO_RESTART.set(autoRestart);
        onClose();
    }

    @Override
    public void onClose() {
        if (this.minecraft != null && parent != null) {
            this.minecraft.setScreen(parent);
        } else {
            super.onClose();
        }
    }

    @Override
    public void render(GuiGraphics graphics, int mouseX, int mouseY, float partialTick) {
        super.render(graphics, mouseX, mouseY, partialTick);
        graphics.drawCenteredString(this.font, "Fixed the bug where there was no BSOD in Minecraft",
                this.width / 2, 16, 0xFFFFFF);
    }

    /** Minimal non-editable label widget. */
    private static final class LabelStub extends AbstractWidget {
        private final String text;

        LabelStub(int x, int y, String text) {
            super(x, y, 240, 10, Component.literal(text));
            this.text = text;
        }

        @Override
        protected void renderWidget(GuiGraphics graphics, int mouseX, int mouseY, float partialTick) {
            graphics.drawString(net.minecraft.client.Minecraft.getInstance().font,
                    text, getX(), getY(), 0xA0A0A0);
        }

        @Override
        protected void updateWidgetNarration(net.minecraft.client.gui.narration.NarrationElementOutput output) {
        }
    }
}