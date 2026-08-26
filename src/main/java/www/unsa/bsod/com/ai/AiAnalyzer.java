package www.unsa.bsod.com.ai;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.util.List;
import java.util.Map;

import com.google.gson.Gson;

import www.unsa.bsod.com.Config;
import www.unsa.bsod.com.crash.ModContext;

/**
 * Talks to any OpenAI-compatible chat completions endpoint.
 * Works with OpenAI, DeepSeek, Moonshot, local Ollama, llama.cpp server, ...
 */
public final class AiAnalyzer {

    private static final Gson GSON = new Gson();

    private AiAnalyzer() {
    }

    /**
     * Sends the FULL crash report to an OpenAI-compatible chat endpoint and
     * returns its analysis.
     */
    public static String analyze(ModContext context, String fullReport) throws Exception {
        String baseUrl = Config.AI_BASE_URL.get().trim();
        if (baseUrl.endsWith("/")) {
            baseUrl = baseUrl.substring(0, baseUrl.length() - 1);
        }
        String url = baseUrl + "/chat/completions";

        Map<String, Object> payload = Map.of(
                "model", Config.AI_MODEL.get(),
                "messages", List.of(
                        Map.of("role", "system",
                                "content", "You are a senior Minecraft modding engineer. A full crash report follows. Analyse it and answer concisely in English: likely root cause, involved mods, and concrete fix suggestions. Plain text only."),
                        Map.of("role", "user", "content", truncate(fullReport, 30000))),
                "temperature", 0.2);

        HttpClient client = HttpClient.newBuilder()
                .connectTimeout(Duration.ofSeconds(15))
                .build();

        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(url))
                .timeout(Duration.ofSeconds(Config.AI_TIMEOUT_SECONDS.get()))
                .header("Content-Type", "application/json")
                .header("Authorization", "Bearer " + Config.AI_API_KEY.get())
                .POST(HttpRequest.BodyPublishers.ofString(GSON.toJson(payload)))
                .build();

        HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());

        if (response.statusCode() < 200 || response.statusCode() >= 300) {
            throw new IllegalStateException("AI endpoint returned HTTP " + response.statusCode()
                    + ": " + truncate(response.body(), 500));
        }

        var tree = GSON.fromJson(response.body(), Map.class);
        if (tree == null || !(tree.get("choices") instanceof List<?> choices) || choices.isEmpty()) {
            throw new IllegalStateException("AI response did not contain choices");
        }
        if (!(choices.get(0) instanceof Map<?, ?> first)
                || !(first.get("message") instanceof Map<?, ?> message)) {
            throw new IllegalStateException("AI response shape unexpected");
        }
        Object content = message.get("content");
        if (content == null || content.toString().isBlank()) {
            throw new IllegalStateException("AI returned an empty analysis");
        }
        return content.toString().strip();
    }

    private static String truncate(String s, int max) {
        if (s == null) {
            return "";
        }
        return s.length() <= max ? s : s.substring(0, max) + "\n... (truncated)";
    }
}