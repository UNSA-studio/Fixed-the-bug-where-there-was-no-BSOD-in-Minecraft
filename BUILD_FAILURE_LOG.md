# Build failure log

```text
To honour the JVM settings for this build a single-use Daemon process will be forked. For more on this, please refer to https://docs.gradle.org/8.12/userguide/gradle_daemon.html#sec:disabling_the_daemon in the Gradle documentation.
Daemon will be stopped at the end of the build 

> Configure project :
Creating Minecraft artifacts without recompilation.

> Task :createMinecraftArtifacts
Loaded 131 artifacts from /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/tmp/createMinecraftArtifacts/nfrt_artifact_manifest.properties
[1m*** Started working on [4mdownloadJson[0m[0m
 [1m[92m✓[0m Completed [4mdownloadJson[0m in 0.02s
[1m*** Started working on [4mdownloadServer[0m[0m
[1m*** Started working on [4mdownloadClient[0m[0m
[1m*** Started working on [4mdownloadClientMappings[0m[0m
 [1m[92m✓[0m Completed [4mdownloadClientMappings[0m in 0.03s
 [1m[92m✓[0m Completed [4mdownloadServer[0m in 0.03s
 [1m[92m✓[0m Completed [4mdownloadClient[0m in 0.03s
[1m*** Started working on [4mstripClient[0m[0m
 [1m[92m♻[0m Used cache of [4mstripClient[0m in 0.00s
[1m*** Started working on [4mmergeMappings[0m[0m
 [1m[92m♻[0m Used cache of [4mmergeMappings[0m in 0.00s
[1m*** Started working on [4mextractServer[0m[0m
 [1m[92m♻[0m Used cache of [4mextractServer[0m in 0.00s
[1m*** Started working on [4mstripServer[0m[0m
 [1m[92m♻[0m Used cache of [4mstripServer[0m in 0.00s
[1m*** Started working on [4mmerge[0m[0m
 [1m[92m♻[0m Used cache of [4mmerge[0m in 0.00s
[1m*** Started working on [4mrename[0m[0m
 [1m[92m♻[0m Used cache of [4mrename[0m in 0.00s
[1m*** Started working on [4mbinaryPatch[0m[0m
 [1m[92m♻[0m Used cache of [4mbinaryPatch[0m in 0.00s
[1m*** Started working on [4mcopyUnpatchedClasses[0m[0m
 [1m[92m♻[0m Used cache of [4mcopyUnpatchedClasses[0m in 0.00s
[1m*** Started working on [4mapplyDevTransforms[0m[0m
 [1m[92m♻[0m Used cache of [4mapplyDevTransforms[0m in 0.00s
[1m*** Started working on [4mbinaryWithNeoForge[0m[0m
 [1m[92m♻[0m Used cache of [4mbinaryWithNeoForge[0m in 0.00s
Total runtime: 0.95s


> Task :compileJava
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:9: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:10: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:13: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java:102: error: incompatible types: possible lossy conversion from long to int
            blob.u32(m.size);
                      ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:28: error: cannot find symbol
        this.description = report.getDescription();
                                 ^
  symbol:   method getDescription()
  location: variable report of type CrashReport
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:40: error: cannot find symbol
        for (String mod : ModListDetector.modsInStackTrace(fullStacktrace)) {
                                         ^
  symbol:   method modsInStackTrace(String)
  location: class ModListDetector
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/BsodMod.java:30: error: cannot find symbol
        modContainer.registerConfig(ModConfig.Type.STARTUP, Config.SPEC);
                                                            ^
  symbol:   variable Config
  location: class BsodMod
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:51: error: package Config does not exist
            String url = normalizeUrl(Config.QR_TARGET_URL.get());
                                            ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:112: error: package Config does not exist
            var dumpFolder = FMLPaths.GAMEDIR.get().resolve(Config.DUMP_FOLDER_NAME.get());
                                                                  ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:121: error: package Config does not exist
            if (Config.SAVE_TO_DESKTOP.get()) {
                      ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:130: error: cannot find symbol
            if (Config.aiEnabled()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:136: error: package Config does not exist
                    if (Config.SAVE_TO_DESKTOP.get()) {
                              ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:152: error: cannot find symbol
            if (Config.autoRestart()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:154: error: package Config does not exist
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:160: error: cannot find symbol
            if (Config.autoRestart()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:162: error: package Config does not exist
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:28: error: package Config does not exist
        String baseUrl = Config.AI_BASE_URL.get().trim();
                               ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:37: error: package Config does not exist
                "model", Config.AI_MODEL.get(),
                               ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:52: error: package Config does not exist
                .header("Authorization", "Bearer " + Config.AI_API_KEY.get())
                                                           ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:50: error: package Config does not exist
                .timeout(Duration.ofSeconds(Config.AI_TIMEOUT_SECONDS.get()))
                                                  ^
Note: /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java uses or overrides a deprecated API.
Note: Recompile with -Xlint:deprecation for details.
Note: Some messages have been simplified; recompile with -Xdiags:verbose to get full output
20 errors

> Task :compileJava FAILED
gradle/actions: Writing build results to /home/runner/work/_temp/.gradle-actions/build-results/__run_2-1787643623442.json

[Incubating] Problems report is available at: file:///home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/reports/problems/problems-report.html

FAILURE: Build failed with an exception.

* What went wrong:
Execution failed for task ':compileJava'.
> Compilation failed; see the compiler output below.
  Note: Some messages have been simplified; recompile with -Xdiags:verbose to get full outputNote: Recompile with -Xlint:deprecation for details./home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:51: error: package Config does not exist
              String url = normalizeUrl(Config.QR_TARGET_URL.get());
                                              ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:112: error: package Config does not exist
              var dumpFolder = FMLPaths.GAMEDIR.get().resolve(Config.DUMP_FOLDER_NAME.get());
                                                                    ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:121: error: package Config does not exist
              if (Config.SAVE_TO_DESKTOP.get()) {
                        ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:136: error: package Config does not exist
                      if (Config.SAVE_TO_DESKTOP.get()) {
                                ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:154: error: package Config does not exist
                          + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                  ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:162: error: package Config does not exist
                          + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                  ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:28: error: package Config does not exist
          String baseUrl = Config.AI_BASE_URL.get().trim();
                                 ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:37: error: package Config does not exist
                  "model", Config.AI_MODEL.get(),
                                 ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:52: error: package Config does not exist
                  .header("Authorization", "Bearer " + Config.AI_API_KEY.get())
                                                             ^
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:50: error: package Config does not exist
                  .timeout(Duration.ofSeconds(Config.AI_TIMEOUT_SECONDS.get()))
                                                    ^/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:9: error: cannot find symbol
  import www.unsa.bsod.com.Config;
                          ^
    symbol:   class Config
    location: package www.unsa.bsod.com
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:10: error: cannot find symbol
  import www.unsa.bsod.com.Config;
                          ^
    symbol:   class Config
    location: package www.unsa.bsod.com
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:13: error: cannot find symbol
  import www.unsa.bsod.com.Config;
                          ^
    symbol:   class Config
    location: package www.unsa.bsod.com
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/BsodMod.java:30: error: cannot find symbol
          modContainer.registerConfig(ModConfig.Type.STARTUP, Config.SPEC);
                                                              ^
    symbol:   variable Config
    location: class BsodMod
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:130: error: cannot find symbol
              if (Config.aiEnabled()) {
                  ^
    symbol:   variable Config
    location: class CrashCoordinator
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:152: error: cannot find symbol
              if (Config.autoRestart()) {
                  ^
    symbol:   variable Config
    location: class CrashCoordinator
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:160: error: cannot find symbol
              if (Config.autoRestart()) {
                  ^
    symbol:   variable Config
    location: class CrashCoordinator/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:28: error: cannot find symbol
          this.description = report.getDescription();
                                   ^
    symbol:   method getDescription()
    location: variable report of type CrashReport
  /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:40: error: cannot find symbol
          for (String mod : ModListDetector.modsInStackTrace(fullStacktrace)) {
                                           ^
    symbol:   method modsInStackTrace(String)
    location: class ModListDetector/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java:102: error: incompatible types: possible lossy conversion from long to int
              blob.u32(m.size);
                        ^Note: /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java uses or overrides a deprecated API.
  20 errors

* Try:
> Check your code and dependencies to fix the compilation error(s)
> Run with --scan to get full insights.

* Exception is:
org.gradle.api.tasks.TaskExecutionException: Execution failed for task ':compileJava'.
	at org.gradle.api.internal.tasks.execution.ExecuteActionsTaskExecuter.lambda$executeIfValid$1(ExecuteActionsTaskExecuter.java:130)
	at org.gradle.internal.Try$Failure.ifSuccessfulOrElse(Try.java:293)
	at org.gradle.api.internal.tasks.execution.ExecuteActionsTaskExecuter.executeIfValid(ExecuteActionsTaskExecuter.java:128)
	at org.gradle.api.internal.tasks.execution.ExecuteActionsTaskExecuter.execute(ExecuteActionsTaskExecuter.java:116)
	at org.gradle.api.internal.tasks.execution.ProblemsTaskPathTrackingTaskExecuter.execute(ProblemsTaskPathTrackingTaskExecuter.java:40)
	at org.gradle.api.internal.tasks.execution.FinalizePropertiesTaskExecuter.execute(FinalizePropertiesTaskExecuter.java:46)
	at org.gradle.api.internal.tasks.execution.ResolveTaskExecutionModeExecuter.execute(ResolveTaskExecutionModeExecuter.java:51)
	at org.gradle.api.internal.tasks.execution.SkipTaskWithNoActionsExecuter.execute(SkipTaskWithNoActionsExecuter.java:57)
	at org.gradle.api.internal.tasks.execution.SkipOnlyIfTaskExecuter.execute(SkipOnlyIfTaskExecuter.java:74)
	at org.gradle.api.internal.tasks.execution.CatchExceptionTaskExecuter.execute(CatchExceptionTaskExecuter.java:36)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.executeTask(EventFiringTaskExecuter.java:77)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.call(EventFiringTaskExecuter.java:55)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.call(EventFiringTaskExecuter.java:52)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:209)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:204)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.call(DefaultBuildOperationRunner.java:53)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter.execute(EventFiringTaskExecuter.java:52)
	at org.gradle.execution.plan.LocalTaskNodeExecutor.execute(LocalTaskNodeExecutor.java:42)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$InvokeNodeExecutorsAction.execute(DefaultTaskExecutionGraph.java:331)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$InvokeNodeExecutorsAction.execute(DefaultTaskExecutionGraph.java:318)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.lambda$execute$0(DefaultTaskExecutionGraph.java:314)
	at org.gradle.internal.operations.CurrentBuildOperationRef.with(CurrentBuildOperationRef.java:85)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.execute(DefaultTaskExecutionGraph.java:314)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.execute(DefaultTaskExecutionGraph.java:303)
	at org.gradle.execution.plan.DefaultPlanExecutor$ExecutorWorker.execute(DefaultPlanExecutor.java:459)
	at org.gradle.execution.plan.DefaultPlanExecutor$ExecutorWorker.run(DefaultPlanExecutor.java:376)
	at org.gradle.internal.concurrent.ExecutorPolicy$CatchAndRecordFailures.onExecute(ExecutorPolicy.java:64)
	at org.gradle.internal.concurrent.AbstractManagedExecutor$1.run(AbstractManagedExecutor.java:48)
Caused by: org.gradle.api.internal.tasks.compile.CompilationFailedException: Compilation failed; see the compiler output below.
Note: Some messages have been simplified; recompile with -Xdiags:verbose to get full outputNote: Recompile with -Xlint:deprecation for details./home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:51: error: package Config does not exist
            String url = normalizeUrl(Config.QR_TARGET_URL.get());
                                            ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:112: error: package Config does not exist
            var dumpFolder = FMLPaths.GAMEDIR.get().resolve(Config.DUMP_FOLDER_NAME.get());
                                                                  ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:121: error: package Config does not exist
            if (Config.SAVE_TO_DESKTOP.get()) {
                      ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:136: error: package Config does not exist
                    if (Config.SAVE_TO_DESKTOP.get()) {
                              ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:154: error: package Config does not exist
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:162: error: package Config does not exist
                        + Config.RESTART_DELAY_SECONDS.get() * 1000L;
                                ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:28: error: package Config does not exist
        String baseUrl = Config.AI_BASE_URL.get().trim();
                               ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:37: error: package Config does not exist
                "model", Config.AI_MODEL.get(),
                               ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:52: error: package Config does not exist
                .header("Authorization", "Bearer " + Config.AI_API_KEY.get())
                                                           ^
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:50: error: package Config does not exist
                .timeout(Duration.ofSeconds(Config.AI_TIMEOUT_SECONDS.get()))
                                                  ^/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/client/BsodScreen.java:9: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:10: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/ai/AiAnalyzer.java:13: error: cannot find symbol
import www.unsa.bsod.com.Config;
                        ^
  symbol:   class Config
  location: package www.unsa.bsod.com
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/BsodMod.java:30: error: cannot find symbol
        modContainer.registerConfig(ModConfig.Type.STARTUP, Config.SPEC);
                                                            ^
  symbol:   variable Config
  location: class BsodMod
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:130: error: cannot find symbol
            if (Config.aiEnabled()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:152: error: cannot find symbol
            if (Config.autoRestart()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashCoordinator.java:160: error: cannot find symbol
            if (Config.autoRestart()) {
                ^
  symbol:   variable Config
  location: class CrashCoordinator/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:28: error: cannot find symbol
        this.description = report.getDescription();
                                 ^
  symbol:   method getDescription()
  location: variable report of type CrashReport
/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/crash/CrashInfo.java:40: error: cannot find symbol
        for (String mod : ModListDetector.modsInStackTrace(fullStacktrace)) {
                                         ^
  symbol:   method modsInStackTrace(String)
  location: class ModListDetector/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java:102: error: incompatible types: possible lossy conversion from long to int
            blob.u32(m.size);
                      ^Note: /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java uses or overrides a deprecated API.
20 errors
	at org.gradle.api.internal.tasks.compile.JdkJavaCompiler.execute(JdkJavaCompiler.java:84)
	at org.gradle.api.internal.tasks.compile.JdkJavaCompiler.execute(JdkJavaCompiler.java:46)
	at org.gradle.api.internal.tasks.compile.NormalizingJavaCompiler.delegateAndHandleErrors(NormalizingJavaCompiler.java:98)
	at org.gradle.api.internal.tasks.compile.NormalizingJavaCompiler.execute(NormalizingJavaCompiler.java:52)
	at org.gradle.api.internal.tasks.compile.NormalizingJavaCompiler.execute(NormalizingJavaCompiler.java:38)
	at org.gradle.api.internal.tasks.compile.AnnotationProcessorDiscoveringCompiler.execute(AnnotationProcessorDiscoveringCompiler.java:52)
	at org.gradle.api.internal.tasks.compile.AnnotationProcessorDiscoveringCompiler.execute(AnnotationProcessorDiscoveringCompiler.java:38)
	at org.gradle.api.internal.tasks.compile.ModuleApplicationNameWritingCompiler.execute(ModuleApplicationNameWritingCompiler.java:46)
	at org.gradle.api.internal.tasks.compile.ModuleApplicationNameWritingCompiler.execute(ModuleApplicationNameWritingCompiler.java:36)
	at org.gradle.jvm.toolchain.internal.DefaultToolchainJavaCompiler.execute(DefaultToolchainJavaCompiler.java:57)
	at org.gradle.api.tasks.compile.JavaCompile.lambda$createToolchainCompiler$3(JavaCompile.java:205)
	at org.gradle.api.internal.tasks.compile.CleaningJavaCompiler.execute(CleaningJavaCompiler.java:53)
	at org.gradle.api.internal.tasks.compile.incremental.IncrementalCompilerFactory.lambda$createRebuildAllCompiler$0(IncrementalCompilerFactory.java:52)
	at org.gradle.api.internal.tasks.compile.incremental.SelectiveCompiler.execute(SelectiveCompiler.java:70)
	at org.gradle.api.internal.tasks.compile.incremental.SelectiveCompiler.execute(SelectiveCompiler.java:44)
	at org.gradle.api.internal.tasks.compile.incremental.IncrementalResultStoringCompiler.execute(IncrementalResultStoringCompiler.java:66)
	at org.gradle.api.internal.tasks.compile.incremental.IncrementalResultStoringCompiler.execute(IncrementalResultStoringCompiler.java:52)
	at org.gradle.api.internal.tasks.compile.CompileJavaBuildOperationReportingCompiler$1.call(CompileJavaBuildOperationReportingCompiler.java:64)
	at org.gradle.api.internal.tasks.compile.CompileJavaBuildOperationReportingCompiler$1.call(CompileJavaBuildOperationReportingCompiler.java:48)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:209)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:204)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.call(DefaultBuildOperationRunner.java:53)
	at org.gradle.api.internal.tasks.compile.CompileJavaBuildOperationReportingCompiler.execute(CompileJavaBuildOperationReportingCompiler.java:48)
	at org.gradle.api.tasks.compile.JavaCompile.performCompilation(JavaCompile.java:223)
	at org.gradle.api.tasks.compile.JavaCompile.performIncrementalCompilation(JavaCompile.java:164)
	at org.gradle.api.tasks.compile.JavaCompile.compile(JavaCompile.java:149)
	at java.base/jdk.internal.reflect.DirectMethodHandleAccessor.invoke(DirectMethodHandleAccessor.java:103)
	at org.gradle.internal.reflect.JavaMethod.invoke(JavaMethod.java:125)
	at org.gradle.api.internal.project.taskfactory.IncrementalTaskAction.doExecute(IncrementalTaskAction.java:45)
	at org.gradle.api.internal.project.taskfactory.StandardTaskAction.execute(StandardTaskAction.java:51)
	at org.gradle.api.internal.project.taskfactory.IncrementalTaskAction.execute(IncrementalTaskAction.java:26)
	at org.gradle.api.internal.project.taskfactory.StandardTaskAction.execute(StandardTaskAction.java:29)
	at org.gradle.api.internal.tasks.execution.TaskExecution$3.run(TaskExecution.java:244)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$1.execute(DefaultBuildOperationRunner.java:29)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$1.execute(DefaultBuildOperationRunner.java:26)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.run(DefaultBuildOperationRunner.java:47)
	at org.gradle.api.internal.tasks.execution.TaskExecution.executeAction(TaskExecution.java:229)
	at org.gradle.api.internal.tasks.execution.TaskExecution.executeActions(TaskExecution.java:212)
	at org.gradle.api.internal.tasks.execution.TaskExecution.executeWithPreviousOutputFiles(TaskExecution.java:195)
	at org.gradle.api.internal.tasks.execution.TaskExecution.execute(TaskExecution.java:162)
	at org.gradle.internal.execution.steps.ExecuteStep.executeInternal(ExecuteStep.java:105)
	at org.gradle.internal.execution.steps.ExecuteStep.access$000(ExecuteStep.java:44)
	at org.gradle.internal.execution.steps.ExecuteStep$1.call(ExecuteStep.java:59)
	at org.gradle.internal.execution.steps.ExecuteStep$1.call(ExecuteStep.java:56)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:209)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:204)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.call(DefaultBuildOperationRunner.java:53)
	at org.gradle.internal.execution.steps.ExecuteStep.execute(ExecuteStep.java:56)
	at org.gradle.internal.execution.steps.ExecuteStep.execute(ExecuteStep.java:44)
	at org.gradle.internal.execution.steps.CancelExecutionStep.execute(CancelExecutionStep.java:42)
	at org.gradle.internal.execution.steps.TimeoutStep.executeWithoutTimeout(TimeoutStep.java:75)
	at org.gradle.internal.execution.steps.TimeoutStep.execute(TimeoutStep.java:55)
	at org.gradle.internal.execution.steps.PreCreateOutputParentsStep.execute(PreCreateOutputParentsStep.java:50)
	at org.gradle.internal.execution.steps.PreCreateOutputParentsStep.execute(PreCreateOutputParentsStep.java:28)
	at org.gradle.internal.execution.steps.RemovePreviousOutputsStep.execute(RemovePreviousOutputsStep.java:67)
	at org.gradle.internal.execution.steps.RemovePreviousOutputsStep.execute(RemovePreviousOutputsStep.java:37)
	at org.gradle.internal.execution.steps.BroadcastChangingOutputsStep.execute(BroadcastChangingOutputsStep.java:61)
	at org.gradle.internal.execution.steps.BroadcastChangingOutputsStep.execute(BroadcastChangingOutputsStep.java:26)
	at org.gradle.internal.execution.steps.CaptureOutputsAfterExecutionStep.execute(CaptureOutputsAfterExecutionStep.java:69)
	at org.gradle.internal.execution.steps.CaptureOutputsAfterExecutionStep.execute(CaptureOutputsAfterExecutionStep.java:46)
	at org.gradle.internal.execution.steps.ResolveInputChangesStep.execute(ResolveInputChangesStep.java:40)
	at org.gradle.internal.execution.steps.ResolveInputChangesStep.execute(ResolveInputChangesStep.java:29)
	at org.gradle.internal.execution.steps.BuildCacheStep.executeWithoutCache(BuildCacheStep.java:189)
	at org.gradle.internal.execution.steps.BuildCacheStep.executeAndStoreInCache(BuildCacheStep.java:145)
	at org.gradle.internal.execution.steps.BuildCacheStep.lambda$executeWithCache$4(BuildCacheStep.java:101)
	at org.gradle.internal.execution.steps.BuildCacheStep.lambda$executeWithCache$5(BuildCacheStep.java:101)
	at org.gradle.internal.Try$Success.map(Try.java:175)
	at org.gradle.internal.execution.steps.BuildCacheStep.executeWithCache(BuildCacheStep.java:85)
	at org.gradle.internal.execution.steps.BuildCacheStep.lambda$execute$0(BuildCacheStep.java:74)
	at org.gradle.internal.Either$Left.fold(Either.java:115)
	at org.gradle.internal.execution.caching.CachingState.fold(CachingState.java:62)
	at org.gradle.internal.execution.steps.BuildCacheStep.execute(BuildCacheStep.java:73)
	at org.gradle.internal.execution.steps.BuildCacheStep.execute(BuildCacheStep.java:48)
	at org.gradle.internal.execution.steps.StoreExecutionStateStep.execute(StoreExecutionStateStep.java:46)
	at org.gradle.internal.execution.steps.StoreExecutionStateStep.execute(StoreExecutionStateStep.java:35)
	at org.gradle.internal.execution.steps.SkipUpToDateStep.executeBecause(SkipUpToDateStep.java:75)
	at org.gradle.internal.execution.steps.SkipUpToDateStep.lambda$execute$2(SkipUpToDateStep.java:53)
	at org.gradle.internal.execution.steps.SkipUpToDateStep.execute(SkipUpToDateStep.java:53)
	at org.gradle.internal.execution.steps.SkipUpToDateStep.execute(SkipUpToDateStep.java:35)
	at org.gradle.internal.execution.steps.legacy.MarkSnapshottingInputsFinishedStep.execute(MarkSnapshottingInputsFinishedStep.java:37)
	at org.gradle.internal.execution.steps.legacy.MarkSnapshottingInputsFinishedStep.execute(MarkSnapshottingInputsFinishedStep.java:27)
	at org.gradle.internal.execution.steps.ResolveIncrementalCachingStateStep.executeDelegate(ResolveIncrementalCachingStateStep.java:49)
	at org.gradle.internal.execution.steps.ResolveIncrementalCachingStateStep.executeDelegate(ResolveIncrementalCachingStateStep.java:27)
	at org.gradle.internal.execution.steps.AbstractResolveCachingStateStep.execute(AbstractResolveCachingStateStep.java:71)
	at org.gradle.internal.execution.steps.AbstractResolveCachingStateStep.execute(AbstractResolveCachingStateStep.java:39)
	at org.gradle.internal.execution.steps.ResolveChangesStep.execute(ResolveChangesStep.java:65)
	at org.gradle.internal.execution.steps.ResolveChangesStep.execute(ResolveChangesStep.java:36)
	at org.gradle.internal.execution.steps.ValidateStep.execute(ValidateStep.java:107)
	at org.gradle.internal.execution.steps.ValidateStep.execute(ValidateStep.java:56)
	at org.gradle.internal.execution.steps.AbstractCaptureStateBeforeExecutionStep.execute(AbstractCaptureStateBeforeExecutionStep.java:64)
	at org.gradle.internal.execution.steps.AbstractCaptureStateBeforeExecutionStep.execute(AbstractCaptureStateBeforeExecutionStep.java:43)
	at org.gradle.internal.execution.steps.AbstractSkipEmptyWorkStep.executeWithNonEmptySources(AbstractSkipEmptyWorkStep.java:125)
	at org.gradle.internal.execution.steps.AbstractSkipEmptyWorkStep.execute(AbstractSkipEmptyWorkStep.java:61)
	at org.gradle.internal.execution.steps.AbstractSkipEmptyWorkStep.execute(AbstractSkipEmptyWorkStep.java:36)
	at org.gradle.internal.execution.steps.legacy.MarkSnapshottingInputsStartedStep.execute(MarkSnapshottingInputsStartedStep.java:38)
	at org.gradle.internal.execution.steps.LoadPreviousExecutionStateStep.execute(LoadPreviousExecutionStateStep.java:36)
	at org.gradle.internal.execution.steps.LoadPreviousExecutionStateStep.execute(LoadPreviousExecutionStateStep.java:23)
	at org.gradle.internal.execution.steps.HandleStaleOutputsStep.execute(HandleStaleOutputsStep.java:75)
	at org.gradle.internal.execution.steps.HandleStaleOutputsStep.execute(HandleStaleOutputsStep.java:41)
	at org.gradle.internal.execution.steps.AssignMutableWorkspaceStep.lambda$execute$0(AssignMutableWorkspaceStep.java:35)
	at org.gradle.api.internal.tasks.execution.TaskExecution$4.withWorkspace(TaskExecution.java:289)
	at org.gradle.internal.execution.steps.AssignMutableWorkspaceStep.execute(AssignMutableWorkspaceStep.java:31)
	at org.gradle.internal.execution.steps.AssignMutableWorkspaceStep.execute(AssignMutableWorkspaceStep.java:22)
	at org.gradle.internal.execution.steps.ChoosePipelineStep.execute(ChoosePipelineStep.java:40)
	at org.gradle.internal.execution.steps.ChoosePipelineStep.execute(ChoosePipelineStep.java:23)
	at org.gradle.internal.execution.steps.ExecuteWorkBuildOperationFiringStep.lambda$execute$2(ExecuteWorkBuildOperationFiringStep.java:67)
	at org.gradle.internal.execution.steps.ExecuteWorkBuildOperationFiringStep.execute(ExecuteWorkBuildOperationFiringStep.java:67)
	at org.gradle.internal.execution.steps.ExecuteWorkBuildOperationFiringStep.execute(ExecuteWorkBuildOperationFiringStep.java:39)
	at org.gradle.internal.execution.steps.IdentityCacheStep.execute(IdentityCacheStep.java:46)
	at org.gradle.internal.execution.steps.IdentityCacheStep.execute(IdentityCacheStep.java:34)
	at org.gradle.internal.execution.steps.IdentifyStep.execute(IdentifyStep.java:48)
	at org.gradle.internal.execution.steps.IdentifyStep.execute(IdentifyStep.java:35)
	at org.gradle.internal.execution.impl.DefaultExecutionEngine$1.execute(DefaultExecutionEngine.java:61)
	at org.gradle.api.internal.tasks.execution.ExecuteActionsTaskExecuter.executeIfValid(ExecuteActionsTaskExecuter.java:127)
	at org.gradle.api.internal.tasks.execution.ExecuteActionsTaskExecuter.execute(ExecuteActionsTaskExecuter.java:116)
	at org.gradle.api.internal.tasks.execution.ProblemsTaskPathTrackingTaskExecuter.execute(ProblemsTaskPathTrackingTaskExecuter.java:40)
	at org.gradle.api.internal.tasks.execution.FinalizePropertiesTaskExecuter.execute(FinalizePropertiesTaskExecuter.java:46)
	at org.gradle.api.internal.tasks.execution.ResolveTaskExecutionModeExecuter.execute(ResolveTaskExecutionModeExecuter.java:51)
	at org.gradle.api.internal.tasks.execution.SkipTaskWithNoActionsExecuter.execute(SkipTaskWithNoActionsExecuter.java:57)
	at org.gradle.api.internal.tasks.execution.SkipOnlyIfTaskExecuter.execute(SkipOnlyIfTaskExecuter.java:74)
	at org.gradle.api.internal.tasks.execution.CatchExceptionTaskExecuter.execute(CatchExceptionTaskExecuter.java:36)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.executeTask(EventFiringTaskExecuter.java:77)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.call(EventFiringTaskExecuter.java:55)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter$1.call(EventFiringTaskExecuter.java:52)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:209)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:204)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.call(DefaultBuildOperationRunner.java:53)
	at org.gradle.api.internal.tasks.execution.EventFiringTaskExecuter.execute(EventFiringTaskExecuter.java:52)
	at org.gradle.execution.plan.LocalTaskNodeExecutor.execute(LocalTaskNodeExecutor.java:42)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$InvokeNodeExecutorsAction.execute(DefaultTaskExecutionGraph.java:331)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$InvokeNodeExecutorsAction.execute(DefaultTaskExecutionGraph.java:318)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.lambda$execute$0(DefaultTaskExecutionGraph.java:314)
	at org.gradle.internal.operations.CurrentBuildOperationRef.with(CurrentBuildOperationRef.java:85)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.execute(DefaultTaskExecutionGraph.java:314)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph$BuildOperationAwareExecutionAction.execute(DefaultTaskExecutionGraph.java:303)
	at org.gradle.execution.plan.DefaultPlanExecutor$ExecutorWorker.execute(DefaultPlanExecutor.java:459)
	at org.gradle.execution.plan.DefaultPlanExecutor$ExecutorWorker.run(DefaultPlanExecutor.java:376)
	at org.gradle.internal.concurrent.ExecutorPolicy$CatchAndRecordFailures.onExecute(ExecutorPolicy.java:64)
	at org.gradle.internal.concurrent.AbstractManagedExecutor$1.run(AbstractManagedExecutor.java:48)


BUILD FAILED in 17s
2 actionable tasks: 2 executed
```
