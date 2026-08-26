package www.unsa.bsod.com.client;

import com.mojang.blaze3d.systems.RenderSystem;

import io.nayuki.qrcodegen.QrCode;
import net.minecraft.client.gui.GuiGraphics;
import net.minecraft.client.gui.screens.Screen;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.FormattedText;
import www.unsa.bsod.com.Config;
import www.unsa.bsod.com.crash.BsodState;

/**
 * The star of the show: a faithful Windows 10 blue screen of death,
 * rendered live inside the still-running game window.
 *
 * Layout mirrors the real thing:
 *   :(                       (huge)
 *   Your PC ran into a problem...
 *   XX%                      (live collection progress)
 *   [QR] For more information about this issue and possible fixes,
 *        visit https://www.minecraft.net
 *
 *   Stop code: <crash code>
 */
public final class BsodScreen extends Screen {

    private static final int BLUE = 0xFF0078D7;
    private static final int WHITE = 0xFFFFFFFF;

    private static final String QR_TARGET_DEFAULT = "https://www.minecraft.net";

    private final BsodState state;

    private QrCode qrCode;
    private boolean restartTriggered;
    private boolean restartSpawned;

    /** Simple spinner frames for the collection phase. */
    private static final char[] SPINNER = { '|', '/', '-', '\\' };

    public BsodScreen(BsodState state) {
        super(Component.literal("BlueScreenOfDeath"));
        this.state = state;
    }

    @Override
    protected void init() {
        // No buttons. No escape hatch. This is a BSOD.
        clearWidgets();
        try {
            String url = normalizeUrl(Config.QR_TARGET_URL.get());
            this.qrCode = QrCode.encodeText(url, QrCode.Ecc.MEDIUM);
        } catch (Throwable t) {
            this.qrCode = null;
        }
    }

    @Override
    public void render(final GuiGraphics graphics, int mouseX, int mouseY, float partialTick) {
        final int w = this.width;
        final int h = this.height;
        final int left = Math.max(w / 6, 48);

        // Solid Windows-blue background.
        graphics.fill(0, 0, w, h, BLUE);

        var font = this.font;
        var pose = graphics.pose();
        int y = Math.max(h / 6, 40);

        // ---- :( ----
        pose.pushPose();
        pose.scale(3.0F, 3.0F, 1.0F);
        graphics.drawString(font, ":(", (int) (left / 3.0F), (int) (y / 3.0F), WHITE, false);
        pose.popPose();
        y += font.lineHeight * 4;

        // ---- main copy ----
        y += 12;
        graphics.drawString(font,
                "Your PC ran into a problem and needs to restart.", left, y, WHITE, false);
        y += font.lineHeight + 2;
        graphics.drawString(font,
                "We're just collecting some error info, and then we'll restart for you.",
                left, y, WHITE, false);
        y += font.lineHeight + 6;

        // ---- progress percentage ----
        int percent = Math.min(100, Math.max(0, state.collectPercent));
        String percentLine = percent + "% complete";
        if (percent < 100) {
            long elapsed = System.currentTimeMillis() - state.takenOverAtMillis;
            char spin = SPINNER[(int) ((elapsed / 250L) % SPINNER.length)];
            percentLine = percent + "% " + spin;
        }
        graphics.drawString(font, percentLine, left, y, WHITE, false);
        y += font.lineHeight * 2;

        // ---- current phase ----
        if (!state.pipelineDone) {
            graphics.drawString(font, state.phase, left, y, 0xCCFFFFFF, false);
            y += font.lineHeight + 2;
        }

        // ---- AI analysis panel: shows what the model actually said ----
        boolean showQr = true;
        if (state.aiAnalysis != null) {
            int panelWidth = Math.min(w - left - 24, 420);
            java.util.List<FormattedText> lines =
                    font.split(FormattedText.of(state.aiAnalysis.strip()), panelWidth);
            int maxLines = Math.min(lines.size(), 8);

            graphics.drawString(font, "AI analysis:", left, y, WHITE, false);
            y += font.lineHeight + 2;

            int i = 0;
            for (; i < maxLines; i++) {
                graphics.drawString(font, lines.get(i), left, y, 0xE6FFFFFF, false);
                y += font.lineHeight;
            }
            if (lines.size() > maxLines) {
                graphics.drawString(font,
                        "... full analysis appended to the saved report",
                        left, y, 0xB3FFFFFF, false);
                y += font.lineHeight;
            }
            showQr = false;
        } else if (state.aiFailed) {
            graphics.drawString(font, "AI analysis unavailable - report saved locally.",
                    left, y, 0xB3FFFFFF, false);
        }

        // ---- QR code block (hidden while the AI panel is up, space is tight) ----
        if (qrCode != null && showQr) {
            int module = Math.max(2, h / 72);
            int qrPixels = qrCode.size * module;
            int qrY = y + 8;
            if (qrY + qrPixels < h - 64) {
                RenderSystem.disableBlend();
                for (int qx = 0; qx < qrCode.size; qx++) {
                    for (int qy = 0; qy < qrCode.size; qy++) {
                        if (qrCode.getModule(qx, qy)) {
                            int px = left + qx * module;
                            int py = qrY + qy * module;
                            graphics.fill(px, py, px + module, py + module, WHITE);
                        }
                    }
                }
                int textX = left + qrPixels + 12;
                int textY = qrY + qrPixels / 2 - font.lineHeight;
                graphics.drawString(font, "For more information about this issue",
                        textX, textY, WHITE, false);
                graphics.drawString(font, "and possible fixes, visit",
                        textX, textY + font.lineHeight, WHITE, false);
                graphics.drawString(font, "https://www.minecraft.net",
                        textX, textY + font.lineHeight * 2 + 2, WHITE, false);
            }
        }

        // ---- bottom area: stop code + mods ----
        int bottom = h - 24;
        String stopCode = truncate(state.context.crashCode(), Math.max(20, (w - left - 16) / 6));
        graphics.drawString(font, "Stop code: ", left, bottom, WHITE, false);
        graphics.drawString(font, stopCode, left + font.width("Stop code: "), bottom, WHITE, false);
        bottom -= font.lineHeight + 2;
        graphics.drawString(font,
                "What failed: " + truncate(state.context.involvedModsJoined(),
                        Math.max(16, (w - left - 16) / 7)),
                left, bottom, WHITE, false);

        // ---- status line: AI + restart countdown ----
        String status;
        if (state.aiFailed) {
            status = "AI analysis unavailable (see latest.log). Report saved locally.";
        } else if (state.aiAnalysis != null) {
            status = "AI analysis finished and appended to the saved report.";
        } else if (state.pipelineDone) {
            status = "Report saved.";
        } else {
            status = "";
        }

        if (state.restartAtMillis > 0) {
            long remainingMs = state.restartAtMillis - System.currentTimeMillis();
            if (remainingMs <= 0) {
                status = "Restarting...";
                if (!restartSpawned) {
                    restartSpawned = GameRestarter.restart();
                    restartTriggered = true;
                    if (!restartSpawned) {
                        status = "Automatic restart failed - you can close this window.";
                    }
                } else if (restartTriggered) {
                    status = "New instance launched - you can close this window now.";
                }
            } else {
                long secs = (remainingMs + 999) / 1000;
                status = (status.isBlank() ? "" : status + " ") + "Restarting in " + secs + "s...";
            }
        }

        if (!status.isBlank()) {
            graphics.drawString(font, truncate(status, Math.max(24, (w - left - 16) / 5)),
                    left, h - 40, 0xB3FFFFFF, false);
        }
    }

    @Override
    public void renderBackground(final GuiGraphics graphics, int mouseX, int mouseY, float partialTick) {
        // The blue wall of sadness IS the background.
    }

    @Override
    public boolean isPauseScreen() {
        return false;
    }

    @Override
    public void onClose() {
        // A BSOD does not close. Nice try.
    }

    @Override
    public boolean keyPressed(int keyCode, int scanCode, int modifiers) {
        return true; // swallow everything, including ESC
    }

    @Override
    public boolean keyReleased(int keyCode, int scanCode, int modifiers) {
        return true;
    }

    @Override
    public boolean mouseClicked(double mouseX, double mouseY, int button) {
        return true;
    }

    private static String normalizeUrl(String url) {
        String trimmed = url == null ? "" : url.trim();
        return trimmed.isBlank() ? QR_TARGET_DEFAULT : trimmed;
    }

    private static String truncate(String s, int maxChars) {
        if (s == null) {
            return "";
        }
        return s.length() <= maxChars ? s : s.substring(0, maxChars - 3) + "...";
    }
}