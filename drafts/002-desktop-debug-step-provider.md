# 002 Desktop Debug Step Provider

## Objective
Add a desktop-only step input provider that can simulate pedometer updates via hotkey or timer so StepMovementController can be tested without iOS hardware.

## Task Restatement
Implement a debug step source for non-iOS builds that emits cumulative step counts to the Qt layer and can be controlled by a hotkey or periodic timer.

## Why This Next
StepMovementController and HUD work will be blocked without a reliable way to generate step input on macOS/Windows/Linux.

## Scope
- Provide a Qt-facing step provider class that emits `stepsUpdated(totalSteps)`.
- Add a debug implementation for non-iOS builds with hotkey and timer modes.
- Wire the provider into the client layer so StepMovementController can consume it.

## Non-Goals
- No iOS pedometer bridge yet (handled in a later task).
- No final HUD or movement consumption logic.
- No persistence of step history between sessions.

## Implementation Plan
1. Define a common interface in `vcmiqt`.
   - Add `vcmiqt/StepInputProvider.h/.cpp` as a `QObject`.
   - Signal: `void stepsUpdated(qint64 totalSteps)`.
   - Methods: `start()`, `stop()`, `reset()`, `setPaused(bool)`.
   - Keep it minimal and match the future iOS pedometer bridge.

2. Implement a desktop debug provider.
   - Add `vcmiqt/DebugStepInputProvider.h/.cpp`.
   - Maintain `qint64 totalSteps`.
   - Support:
     - Manual increments via `addSteps(int steps)`.
     - Timer mode via `QTimer` with `stepsPerTick` and `intervalMs`.
   - Defaults: `stepsPerTick = 25`, `intervalMs = 1000`.

3. Add a small debug control surface.
   - Register new shortcuts:
     - `stepDebugAdd` (adds +100 steps).
     - `stepDebugToggleTimer` (start/stop timer).
   - Extend `client/gui/Shortcut.h` and `client/gui/ShortcutHandler.cpp`.
   - Add defaults in `config/keyBindingsConfig.json`.
   - In the SDL client, when these shortcuts fire, call the debug provider’s methods.

4. Wire into the client.
   - Create a tiny factory in `vcmiqt` to return:
     - iOS provider on iOS builds (stub for now).
     - Debug provider on desktop.
   - Expose the provider instance to `StepMovementController` (constructor injection or a singleton owned by the Qt launcher).

## Sample Snippets

Debug provider signal usage:
```cpp
connect(stepProvider, &StepInputProvider::stepsUpdated,
        this, &StepMovementController::onStepsUpdated);
```

Hotkey hook sketch:
```cpp
case EShortcut::STEP_DEBUG_ADD:
    stepProvider->addSteps(100);
    break;
case EShortcut::STEP_DEBUG_TOGGLE_TIMER:
    stepProvider->toggleTimer();
    break;
```

## Validation Plan
- Launch on desktop, press `stepDebugAdd`, and verify log shows updated cumulative steps.
- Toggle timer mode and confirm periodic `stepsUpdated` signals.
- Ensure no pedometer code compiles on non-iOS builds.

## Risks and Notes
- Keep the debug provider isolated behind `#if !defined(VCMI_IOS)` to avoid shipping it on iOS.
- Avoid hard dependencies on SDL inside `vcmiqt`; keep the hotkey wiring in `client/`.

## Deviations During Implementation
- Implemented a lightweight `client/step/StepDebugProvider` driven by `GameInstance::onUpdate` so timer mode works without a Qt event loop in the SDL client.
- Still added `vcmiqt` `StepInputProvider` and `DebugStepInputProvider` for the future iOS bridge and Qt-side consumers.
