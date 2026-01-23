# 001 Step Mode Flag + Single-Hero Lock

## Objective
Enable a Step Mode toggle that locks gameplay to a single hero per player and hides economy UI via a lightweight mod.

## Task Restatement
Implement the first MVP feature by introducing a Step Mode configuration flag, forcing a single-hero cap, restricting lobby players to one human, and hiding economy UI/buildings when the flag is active.

## Why This First
The step-driven prototype needs a stable single-hero experience and reduced economy noise before step input and movement logic can be tested reliably.

## Scope
- Add `stepModeEnabled` to game settings so engine, server, and client can gate behavior.
- Create a mod that overrides hero caps and disables economy content.
- Enforce the cap in recruitment and lobby assignment flows.

## Non-Goals
- No step input, HUD, or movement controller yet.
- No day/turn scheduler changes yet.
- No AI behavior changes beyond optional slot folding.

## Implementation Plan
1. Mod skeleton and settings override.
   - Create `Mods/step-prototype/mod.json`.
   - Add `Mods/step-prototype/Content/config/gameConfig.json` with:
     - `settings.heroes.perPlayerOnMapCap = 1`
     - `settings.heroes.perPlayerTotalCap = 1`
     - `settings.modules.stepModeEnabled = true` (or a new `settings.stepMode.enabled` group if preferred).
   - Add `Mods/step-prototype/Content/config/factions/` overrides that remove economy-heavy buildings (marketplace, resource silo, mage guilds, etc.).

2. Settings plumbing for Step Mode.
   - Add `STEP_MODE_ENABLED` to `EGameSettings` in `lib/IGameSettings.h`.
   - Map it in `lib/GameSettings.cpp`:
     - Group: `modules`, Key: `stepModeEnabled` (keeps JSON tidy without a new group).
   - Extend `config/schemas/gameSettings.json`:
     - Under `modules.properties`, add `"stepModeEnabled": { "type": "boolean" }`.
   - Set default value in `config/gameConfig.json` under `settings.modules` as `false`.

3. Enforce single-hero cap when Step Mode is enabled.
   - `server/processors/HeroPoolProcessor.cpp`: block hero recruitment if cap reached.
   - `server/CGameHandler.cpp`: gate any hero creation/garrison swap flows that bypass the tavern.
   - `lib/mapObjects/CGHeroInstance.cpp`: prevent hiring or exchange actions when cap is exceeded.

4. Lock lobby to one human player.
   - `client/lobby/OptionsTab.cpp`: short-circuit `OptionsTab::onSetPlayerClicked` to allow only one human in Step Mode.
   - `server/CVCMIServer.cpp`: reject additional human slots in `setPlayer` when Step Mode is active.
   - `server/CVCMIServer.cpp:updateStartInfoOnMapChange`: fold extra `playerInfos` to AI if Step Mode is enabled.

5. Hide economy UI elements.
   - `client/windows/CCastleInterface.cpp`: filter out marketplace/resource building slots in the hall grid.
   - `client/adventureMap/CResDataBar.cpp`: skip rendering the resource bar when Step Mode is active.

## Sample Snippets

Settings override in mod:
```json
{
  "settings": {
    "heroes": {
      "perPlayerOnMapCap": 1,
      "perPlayerTotalCap": 1
    },
    "modules": {
      "stepModeEnabled": true
    }
  }
}
```

Server-side gate:
```cpp
const bool stepMode = gameHandler->gameInfo().getSettings()
    .getBoolean(EGameSettings::STEP_MODE_ENABLED);
if (stepMode && gameHandler->gameInfo().getHeroCount(player, true) >= 1)
{
    complain("Step Mode: only one hero allowed.");
    return;
}
```

## Validation Plan
- Launch with `Mods/step-prototype` enabled; confirm `stepModeEnabled` is `true` in settings.
- Attempt to hire a second hero; verify the action is denied with a clear message.
- Open lobby and confirm only one human slot is allowed.
- Confirm resource bar and marketplace UI are hidden.

## Risks and Notes
- Ensure `stepModeEnabled` is available to both server and client settings objects.
- Verify mod override precedence so base config defaults remain intact when mod is disabled.

## Deviations During Implementation
- Inlined `settings` in `Mods/step-prototype/mod.json` because `CModHandler::initializeConfig` reads the `settings` node directly (arrays are not assembled into JSON here).
- Faction overrides only strip economy buildings from `hallSlots`; if a map ships with those buildings prebuilt, their town-background structures may still appear until we add explicit structure hiding.
