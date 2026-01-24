# 003 Step Counter HUD

## Objective
Show a lightweight on-screen HUD with the current step total and debug status (timer on/off, paused) while Step Mode is active.

## Task Restatement
Add an adventure-map overlay that displays cumulative steps from the step provider, with a toggle to show/hide the HUD and a debug indicator for timer mode.

## Why This Next
We need immediate visual feedback for step input while tuning StepMovementController and pacing.

## Scope
- Create a small HUD widget in `client/` that renders step count text.
- Hook it into the adventure map UI update cycle.
- Add a setting and hotkey to show/hide the HUD.

## Non-Goals
- No movement logic yet (handled in StepMovementController task).
- No persistence across sessions.
- No final UI polish; keep it functional and minimal.

## Implementation Plan
1. Add a HUD widget.
   - Implement `client/adventureMap/StepCounterHud.h/.cpp`.
   - Draw a small panel (or text-only) in a fixed corner.
   - Render: `Steps: <total>` and status flags (`Timer`, `Paused`).

2. Wire data source.
   - Read totals from `GameInstance::stepDebugProvider()` on desktop for now.
   - Leave a TODO hook for `StepInputProvider` once iOS is wired.

3. Add toggle controls.
   - New shortcut `stepHudToggle`.
   - New setting `stepHudVisible` under `settings["interface"]` or `settings["adventure"]`.
   - Default visible when `STEP_MODE_ENABLED` is true.

4. Integrate into adventure map.
   - Create HUD in `AdventureMapInterface` or `AdventureMapWidget`.
   - Update text during `onUpdate()` or map redraw.

## Sample Snippets

HUD update sketch:
```cpp
void StepCounterHud::updateFromProvider(const StepDebugProvider & provider)
{
	setText(vstd::format("Steps: %lld%s",
		static_cast<long long>(provider.totalSteps()),
		provider.isTimerEnabled() ? " (Timer)" : ""));
}
```

## Validation Plan
- Start a map with Step Mode enabled; HUD appears.
- Press debug hotkeys; HUD updates step total and timer indicator.
- Toggle HUD visibility; state persists for session.

## Risks and Notes
- Keep HUD rendering lightweight; avoid per-frame allocations.
- Don’t block Step Mode if HUD is disabled.

## Deviations During Implementation
- Defaulted `stepHudVisible` to `true` in schema and gated rendering on `STEP_MODE_ENABLED` instead of conditional defaults.
- "Paused" currently reflects `!CPlayerInterface::makingTurn` until StepMovementController defines explicit pause semantics.
