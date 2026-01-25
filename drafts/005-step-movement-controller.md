# 005 Step Movement Controller

## Objective
Advance hero movement along a queued path based on real-world steps, consuming a step budget to move one tile at a time.

## Task Restatement
Implement a `StepMovementController` in the client that listens for step updates, tracks a step budget, and issues one-tile movement requests at a controlled pace, pausing during dialogs and battles.

## Why This Next
We already have step input (debug provider) and UI feedback. The next MVP milestone is to connect steps to actual hero movement.

## Scope
- New controller in `client/` that:
  - Subscribes to step totals.
  - Computes available tiles from steps-per-tile and terrain costs.
  - Advances hero one tile per budget consumption, throttled by a minimum interval.
- Hook the controller into the adventure map flow (path selection, movement confirmation, cancelation).
- Configurable settings: steps-per-tile and min tile interval.

## Non-Goals
- No new server-side pathing logic.
- No AI changes or multiplayer support.
- No iOS pedometer integration (handled later).

## Implementation Plan
1. Add a controller class.
   - Create `client/step/StepMovementController.h/.cpp`.
   - Maintain:
     - `int64_t totalSteps`
     - `int64_t lastConsumedSteps`
     - `int64_t stepBudget`
     - `uint32_t lastMoveMs`
     - current hero/path references (or IDs)
   - Provide methods:
     - `onStepsUpdated(int64_t total)`
     - `onPathQueued(const CGHeroInstance *, const CGPath &)`
     - `onPathCleared()`
     - `tick(uint32_t msPassed)`

2. Compute steps-per-tile with terrain cost.
   - Base ratio: `stepsPerTile` (default 100).
   - Multiply by terrain movement cost or path node movement cost.
   - Use current node’s terrain cost to compute `stepsRequiredForNextTile`.

3. Movement pacing and throttling.
   - Require `minTileIntervalMs` between tile moves.
   - Only advance if:
     - Step budget >= required steps
     - No blocking UI (dialogs, battle, town screen)
     - Hero is not already moving / animation in progress

4. Integrate with client flow.
   - Hook into hero path selection in `client/` (likely `PlayerLocalState` or `AdventureMapInterface` callbacks).
   - Forward steps from `StepDebugProvider` (and later Qt provider) into the controller.
   - When a tile should advance, issue a move request through existing client->server movement calls (same path as normal “move hero along path”).

5. Settings and toggles.
   - Add settings:
     - `settings["adventure"]["stepsPerTile"]` (number)
     - `settings["adventure"]["minTileIntervalMs"]` (number)
   - Optional: `settings["adventure"]["stepMovementPaused"]` boolean.

## Sample Snippet
```cpp
if (canAdvance && stepBudget >= stepsRequired && msSinceLastMove >= minTileIntervalMs)
{
	stepBudget -= stepsRequired;
	requestMoveNextTile();
	lastMoveMs = nowMs;
}
```

## Validation Plan
- Queue a hero path, then add steps via debug hotkey.
- Verify one tile advances per threshold and respects the min interval.
- Open a dialog/battle; confirm movement pauses and resumes afterward.

## Risks and Notes
- Ensure movement requests do not overlap with existing animation state.
- Terrain costs might be encoded differently across path nodes; verify with real map data.

## Deviations During Implementation
- Wired path queuing through `CPlayerInterface::moveHero` instead of AdventureMap/PlayerLocalState hooks.
- Polled the debug step total from `CPlayerInterface::update` rather than registering a listener.
- `tick()` now uses `std::chrono::steady_clock` internally instead of passing elapsed ms.
