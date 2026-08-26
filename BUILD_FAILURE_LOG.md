# Build failure log

```text
To honour the JVM settings for this build a single-use Daemon process will be forked. For more on this, please refer to https://docs.gradle.org/8.12/userguide/gradle_daemon.html#sec:disabling_the_daemon in the Gradle documentation.
Daemon will be stopped at the end of the build 

> Configure project :
Creating Minecraft artifacts without recompilation.

> Task :createMinecraftArtifacts
Loaded 131 artifacts from /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/tmp/createMinecraftArtifacts/nfrt_artifact_manifest.properties
[1m*** Started working on [4mdownloadJson[0m[0m
 [1m[92m✓[0m Completed [4mdownloadJson[0m in 0.01s
[1m*** Started working on [4mdownloadServer[0m[0m
[1m*** Started working on [4mdownloadClient[0m[0m
[1m*** Started working on [4mdownloadClientMappings[0m[0m
 [1m[92m✓[0m Completed [4mdownloadClient[0m in 0.02s
 [1m[92m✓[0m Completed [4mdownloadClientMappings[0m in 0.02s
 [1m[92m✓[0m Completed [4mdownloadServer[0m in 0.02s
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
Performing periodic cache maintenance on /home/runner/.gradle/caches/neoformruntime
Cleaning intermediate results cache in /home/runner/.gradle/caches/neoformruntime/intermediate_results
 Maximum age: 744h
 Maximum cache size: 10 GiB
 [0;2;3m22 files found[0m
 [0;2;3m155 MiB overall size[0m
 [0;2;3m0 expired keys found[0m
[0;2;3m Deleted 19 outdated lock files
Total runtime: 0.78s


> Task :compileJava
Note: /home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/src/main/java/www/unsa/bsod/com/dump/MinidumpGenerator.java uses or overrides a deprecated API.
Note: Recompile with -Xlint:deprecation for details.

> Task :compileNativeHook
> Task :processResources
> Task :classes
> Task :jarJar
> Task :jar
> Task :assemble
> Task :compileTestJava NO-SOURCE
> Task :processTestResources NO-SOURCE
> Task :testClasses UP-TO-DATE
> Task :test NO-SOURCE
> Task :check UP-TO-DATE
> Task :compileNativeHookLinux FAILED
gradle/actions: Writing build results to /home/runner/work/_temp/.gradle-actions/build-results/__run_3-1787734695315.json

[Incubating] Problems report is available at: file:///home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/reports/problems/problems-report.html

FAILURE: Build failed with an exception.

* What went wrong:
A problem was found with the configuration of task ':compileNativeHookLinux' (type 'DefaultTask').
  - Gradle detected a problem with the following location: '/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/native/bsod_crash_hook.so'.
    
    Reason: Task ':processResources' uses this output of task ':compileNativeHookLinux' without declaring an explicit or implicit dependency. This can lead to incorrect results being produced, depending on what order the tasks are executed.
    
    Possible solutions:
      1. Declare task ':compileNativeHookLinux' as an input of ':processResources'.
      2. Declare an explicit dependency on ':compileNativeHookLinux' from ':processResources' using Task#dependsOn.
      3. Declare an explicit dependency on ':compileNativeHookLinux' from ':processResources' using Task#mustRunAfter.
    
    For more information, please refer to https://docs.gradle.org/8.12/userguide/validation_problems.html#implicit_dependency in the Gradle documentation.

* Try:
> Run with --info or --debug option to get more log output.
> Run with --scan to get full insights.
> Get more help at https://help.gradle.org.

* Exception is:
org.gradle.internal.execution.WorkValidationException: A problem was found with the configuration of task ':compileNativeHookLinux' (type 'DefaultTask').
  - Gradle detected a problem with the following location: '/home/runner/work/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/Fixed-the-bug-where-there-was-no-BSOD-in-Minecraft/build/native/bsod_crash_hook.so'.
    
    Reason: Task ':processResources' uses this output of task ':compileNativeHookLinux' without declaring an explicit or implicit dependency. This can lead to incorrect results being produced, depending on what order the tasks are executed.
    
    Possible solutions:
      1. Declare task ':compileNativeHookLinux' as an input of ':processResources'.
      2. Declare an explicit dependency on ':compileNativeHookLinux' from ':processResources' using Task#dependsOn.
      3. Declare an explicit dependency on ':compileNativeHookLinux' from ':processResources' using Task#mustRunAfter.
    
    For more information, please refer to https://docs.gradle.org/8.12/userguide/validation_problems.html#implicit_dependency in the Gradle documentation.
	at org.gradle.internal.execution.WorkValidationException$BuilderWithSummary.build(WorkValidationException.java:137)
	at org.gradle.internal.execution.WorkValidationException$BuilderWithSummary.get(WorkValidationException.java:119)
	at org.gradle.internal.execution.steps.ValidateStep.throwValidationException(ValidateStep.java:174)
	at org.gradle.internal.execution.steps.ValidateStep.execute(ValidateStep.java:99)
	at org.gradle.internal.execution.steps.ValidateStep.execute(ValidateStep.java:56)
	at org.gradle.internal.execution.steps.AbstractCaptureStateBeforeExecutionStep.execute(AbstractCaptureStateBeforeExecutionStep.java:64)
	at org.gradle.internal.execution.steps.AbstractCaptureStateBeforeExecutionStep.execute(AbstractCaptureStateBeforeExecutionStep.java:43)
	at org.gradle.internal.execution.steps.AbstractSkipEmptyWorkStep.executeWithNonEmptySources(AbstractSkipEmptyWorkStep.java:125)
	at org.gradle.internal.execution.steps.AbstractSkipEmptyWorkStep.execute(AbstractSkipEmptyWorkStep.java:56)
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
	at org.gradle.execution.plan.DefaultPlanExecutor.process(DefaultPlanExecutor.java:111)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph.executeWithServices(DefaultTaskExecutionGraph.java:138)
	at org.gradle.execution.taskgraph.DefaultTaskExecutionGraph.execute(DefaultTaskExecutionGraph.java:123)
	at org.gradle.execution.SelectedTaskExecutionAction.execute(SelectedTaskExecutionAction.java:35)
	at org.gradle.execution.DryRunBuildExecutionAction.execute(DryRunBuildExecutionAction.java:51)
	at org.gradle.execution.BuildOperationFiringBuildWorkerExecutor$ExecuteTasks.call(BuildOperationFiringBuildWorkerExecutor.java:54)
	at org.gradle.execution.BuildOperationFiringBuildWorkerExecutor$ExecuteTasks.call(BuildOperationFiringBuildWorkerExecutor.java:43)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:209)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$CallableBuildOperationWorker.execute(DefaultBuildOperationRunner.java:204)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:66)
	at org.gradle.internal.operations.DefaultBuildOperationRunner$2.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:166)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.execute(DefaultBuildOperationRunner.java:59)
	at org.gradle.internal.operations.DefaultBuildOperationRunner.call(DefaultBuildOperationRunner.java:53)
	at org.gradle.execution.BuildOperationFiringBuildWorkerExecutor.execute(BuildOperationFiringBuildWorkerExecutor.java:40)
	at org.gradle.internal.build.DefaultBuildLifecycleController.lambda$executeTasks$10(DefaultBuildLifecycleController.java:313)
	at org.gradle.internal.model.StateTransitionController.doTransition(StateTransitionController.java:266)
	at org.gradle.internal.model.StateTransitionController.lambda$tryTransition$8(StateTransitionController.java:177)
	at org.gradle.internal.work.DefaultSynchronizer.withLock(DefaultSynchronizer.java:46)
	at org.gradle.internal.model.StateTransitionController.tryTransition(StateTransitionController.java:177)
	at org.gradle.internal.build.DefaultBuildLifecycleController.executeTasks(DefaultBuildLifecycleController.java:304)
	at org.gradle.internal.build.DefaultBuildWorkGraphController$DefaultBuildWorkGraph.runWork(DefaultBuildWorkGraphController.java:220)
	at org.gradle.internal.work.DefaultWorkerLeaseService.withLocks(DefaultWorkerLeaseService.java:263)
	at org.gradle.internal.work.DefaultWorkerLeaseService.runAsWorkerThread(DefaultWorkerLeaseService.java:127)
	at org.gradle.composite.internal.DefaultBuildController.doRun(DefaultBuildController.java:181)
	at org.gradle.composite.internal.DefaultBuildController.access$000(DefaultBuildController.java:50)
	at org.gradle.composite.internal.DefaultBuildController$BuildOpRunnable.lambda$run$0(DefaultBuildController.java:198)
	at org.gradle.internal.operations.CurrentBuildOperationRef.with(CurrentBuildOperationRef.java:85)
	at org.gradle.composite.internal.DefaultBuildController$BuildOpRunnable.run(DefaultBuildController.java:198)
	at org.gradle.internal.concurrent.ExecutorPolicy$CatchAndRecordFailures.onExecute(ExecutorPolicy.java:64)
	at org.gradle.internal.concurrent.AbstractManagedExecutor$1.run(AbstractManagedExecutor.java:48)


Deprecated Gradle features were used in this build, making it incompatible with Gradle 9.0.

You can use '--warning-mode all' to show the individual deprecation warnings and determine if they come from your own scripts or plugins.

For more on this, please refer to https://docs.gradle.org/8.12/userguide/command_line_interface.html#sec:command_line_warnings in the Gradle documentation.

BUILD FAILED in 17s
7 actionable tasks: 7 executed
```
