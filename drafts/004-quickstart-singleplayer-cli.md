# 004 Quickstart Single Player Scenario CLI

## Objective
Add a command-line path that launches `vcmiclient` straight into a single-player scenario (map or save) without stopping in the main menu.

## Task Restatement
Provide a CLI option that bypasses main menu navigation, auto-selects a scenario, and starts the game with one human player and AI/default for the rest.

## Why This Next
We need fast iteration loops for Step Mode tuning and map scripting without manual menu clicks.

## Scope
- New CLI args for quickstart map or save.
- A helper path to start a local single-player game without opening the menu.
- Reuse existing lobby/server start flows to avoid deep refactors.

## Non-Goals
- No UI changes to the main menu or lobby screens.
- No new multiplayer flows.
- No automation of campaign selection.

## Implementation Plan
1. Add CLI args in `clientapp/EntryPoint.cpp`.
   - Proposed flags:
     - `--quickstart-map <resourcePath>` (e.g., `Maps/MyMap.h3m`)
     - `--quickstart-save <resourcePath>` (e.g., `Saves/slot1.vgm1`)
     - Optional: `--quickstart-last` to use `settings["general"]["lastMap"]`.
   - Store parsed values in local variables or `settings["session"]` (e.g., `session["quickStartMap"]`).

2. Add a quickstart helper in `CServerHandler`.
   - Create `CServerHandler::quickStartSinglePlayer(...)` or similar.
   - Use existing `debugStartTest` flow as a template, but:
     - Do not set `session["onlyai"] = true`.
     - Do not “click off” the human color (`setPlayer(myFirstColor())` is for AI tests).
     - Ensure one human is assigned (player 0 / first color) and other slots stay AI/default.
   - Sequence:
     - `resetStateForLobby(EStartMode::NEW_GAME, ESelectionScreen::newGame, EServerMode::LOCAL, {})`
     - Start local server and connect.
     - Wait for lobby state, then call `setMapInfo(mapInfo)` until accepted.
     - Call `sendStartGame()` once map and players are valid.

3. Skip main menu when quickstart is requested.
   - In `clientapp/EntryPoint.cpp`, after initialization and before `makeActiveInterface()`:
     - If quickstart requested, call `CMainMenu::openLobby(...)` and then the new quickstart helper.
     - Otherwise, keep current menu flow.
   - Keep intro videos disabled for quickstart (use existing `--nointro` or force it only for this path).

4. Handle error reporting.
   - If map or save resource doesn’t exist, show `CInfoWindow::showInfoDialog` and fall back to the main menu.

## Sample Usage
```
./vcmiclient --quickstart-map Maps/StepDemo.h3m
./vcmiclient --quickstart-save Saves/AutoSave_01.vgm1
```

## Validation Plan
- Launch with `--quickstart-map` and verify the game loads into the adventure map with a human player.
- Verify `--quickstart-save` loads the save without menus.
- Provide invalid path and confirm error + menu fallback.

## Risks and Notes
- `debugStartTest` uses blocking loops; if reused, ensure it doesn’t freeze the UI and only runs before the main loop.
- Map resource names must match VCMI’s resource loader (`Maps/...`), not OS paths.

## Deviations During Implementation
- Implemented `--quickstart-map` and `--quickstart-save` only (no `--quickstart-last`).
- Used the existing `mapToStart` auto-start path in `NetPacksLobbyClient` to avoid lobby UI and avoid extra polling loops.
