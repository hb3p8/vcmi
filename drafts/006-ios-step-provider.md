# 006 iOS Step Input Provider

## Objective
Deliver real step counts from iPhone/Apple Watch using `CMPedometer`, bridged into Qt as a `StepInputProvider` that emits cumulative step totals.

## Task Restatement
Add an iOS pedometer bridge under `ios/` and an iOS-specific Qt provider under `vcmiqt/` that forwards cumulative steps via `stepsUpdated(qint64)`, while keeping the debug provider for non-iOS builds.

## Why This Next
Step movement and HUD are in place, but they currently rely on desktop debug input. The MVP needs real pedometer data on iOS to validate the core experience.

## Scope
- Native bridge in `ios/` that wraps `CMPedometer` and exposes callbacks.
- Qt-facing provider in `vcmiqt/` that subscribes to native updates and emits `stepsUpdated`.
- Update `clientapp/ios/Info.plist` with `NSMotionUsageDescription`.
- Keep a debug/mock provider for non-iOS builds.

## Non-Goals
- Background step delivery or health/fitness data sync.
- Apple Watch-only delivery paths.
- UI changes beyond the existing HUD.

## Implementation Plan
1. Add native pedometer wrapper.
   - Create `ios/StepPedometer.h/.mm` with a small Objective-C++ class that:
     - Starts pedometer updates with `CMPedometer`.
     - Delivers cumulative step totals via a C++ callback or Objective-C block.
     - Supports `start()`, `stop()`, and `reset()` (reset redefines the start date/offset).
   - Link `CoreMotion` in `ios/CMakeLists.txt` alongside Foundation/UIKit.

2. Add iOS Qt provider.
   - Create `vcmiqt/IosStepInputProvider.h/.mm` (Objective-C++) that derives from `StepInputProvider`.
   - Own a `StepPedometer` instance and forward updates to Qt:
     - Use `QMetaObject::invokeMethod` or `QTimer` to emit on the Qt thread.
     - Call `emitSteps(total)` to respect pause logic.
   - Implement `start/stop/reset/setPaused` overrides to delegate to the native wrapper.

3. Wire factory and build.
   - Update `vcmiqt/StepInputProviderFactory.cpp`:
     - On `VCMI_IOS`, return `std::make_unique<IosStepInputProvider>(parent)`.
     - Otherwise return `DebugStepInputProvider` as today.
   - Add the new source files to `vcmiqt/CMakeLists.txt`.
   - Ensure `vcmiqt` links against `iOS_utils` (or the new pedometer lib) on iOS.

4. Add motion usage description.
   - Edit `clientapp/ios/Info.plist` to add:
     - `NSMotionUsageDescription`: "Steps are used to advance your hero on the adventure map."

5. iOS debug toggle (optional but recommended).
   - Add a setting such as `settings["adventure"]["useDebugStepsOnIos"]` that switches the factory to return `DebugStepInputProvider`.
   - Keep default to pedometer in shipping builds.

## Sample Snippet
Objective-C++ callback into Qt:
```cpp
void IosStepInputProvider::onNativeSteps(qint64 total)
{
	QMetaObject::invokeMethod(this, [this, total]()
	{
		emitSteps(total);
	}, Qt::QueuedConnection);
}
```

## Validation Plan
- On device, launch with Step Mode enabled and verify `stepsUpdated` increments while walking.
- Toggle pause/resume and confirm updates stop/resume without resetting totals.
- On macOS, ensure the debug provider still compiles and emits steps (no iOS headers pulled in).

## Risks and Notes
- `CMPedometer` availability varies by device; handle `isStepCountingAvailable` and fail gracefully.
- Pedometer updates may arrive on a background thread; always forward to the Qt thread.
- Decide whether totals should be "since app start" or "since reset"; document behavior.

## Deviations During Implementation
None.
