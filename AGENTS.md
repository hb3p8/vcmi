# Step-Driven Adventure Prototype

## Scope
- Guide for agents building a local prototype that layers step-driven exploration onto the VCMI engine.
- Focus: iOS first, single hero, retain HoMM3-style turn-based battles, advance days via real time.

## Prototype Concept
- Replace adventure map movement points with real-world steps captured on iPhone or Apple Watch.
- Keep the world exploration- and story-oriented; hide or minimize strategy economy systems and towns.
- Queue a destination normally, confirm movement, then spend a step budget so every N steps advances the hero one tile.
- Keep tactical battles unchanged for now, triggered by map interactions.

## Guardrails
- Add native bridges under `ios/` and client logic under `vcmiqt/`; avoid sweeping engine refactors.
- Restrict to single-player; no lobby or AI adjustments required for the MVP.
- Feel free to tear apart original game logic if it is needed to make new features work.

## Architecture Touchpoints
### Step input (iOS)
- Use `CMPedometer` for step counts and bridge via Objective-C++ into Qt.
- Place native code in `ios/`, expose a Qt-facing class in `vcmiqt/`, and emit a signal with cumulative steps.
- Update `Info.plist` with the motion usage description; add a debug mock for non-iOS builds.

### Qt helper layer (vcmiqt on iOS)
- `vcmiqt` is the shared Qt utility library that bridges VCMI core data to Qt types (JSON, paths, dialogs) and links against QtCore/QtWidgets (`vcmiqt/CMakeLists.txt:1`).
- iOS builds force a single-app bundle and link the SDL client with the Qt launcher, so `vcmiqt` ships inside the mobile binary (`CMakeLists.txt:83`, `clientapp/CMakeLists.txt:38`).
- The Qt launcher consumes `vcmiqt` helpers and layers Objective-C++ shims for platform services like directory pickers and orientation handling (`launcher/CMakeLists.txt:1`, `launcher/prepare_ios.mm:1`, `ios/iOS_utils.mm:1`).
- Message boxes and similar Qt affordances already include iOS-safe async wrappers in `vcmiqt`; mirror that approach for any new dialogs or prompts (`vcmiqt/MessageBox.h:18`).
- Route pedometer updates through Qt signals/slots here and forward them to the SDL client so gameplay logic continues to live in `client/` and `server/`.

### Client controller
- Implement `StepMovementController` in the Qt client to track steps and compute tiles available.
- Listen for player path selections, advance one tile whenever the step budget allows, and respect animation timing.
- Throttle advances (for example, at least 1 second between tiles) to smooth bursty step delivery.
- Expose settings: steps-per-tile ratio, pause/resume tracking, HUD visibility.

### Movement rules
- Scale required steps by terrain cost for variety.

### Turn and day system
- Keep the same player active continuously; auto end/start the turn internally if needed.
- Tie day rollover to real time (midnight) and trigger day change programmatically.

### Encounters and content
- Use the map editor and Lua scripts for POIs, dialogs, and linear quests.
- Ship a lightweight mod under `Mods/` to hide economy UI and additional heroes.

### Code locations
- Native bridge: `ios/`
- Qt helper and launcher UI: `vcmiqt/`, `launcher/`, `client/`
- Hero movement requests: `client/` to `server/`
- Config and mods: `Mods/`, `config/`
- Build documentation: `docs/developers/Building_iOS.md`

## Defaults and Tunables
- `stepsPerTile`: start at 100 on grass; adjust for play feel or terrain costs.
- `minTileIntervalMs`: 1000 to 1500 to smooth movement pacing.
- Day boundary: local midnight; use six-hour intervals during testing if convenient.

## Risks
- Engine expectation of day/turn cycles; rely on automated turn resets to stay stable.
- Movement animation sync; ensure tile advances wait for completion and cancel cleanly.
- Pedometer data limits; accept foreground-only updates for MVP and add background later if needed.

## Testing Notes
- Provide mock step input and an iOS debug toggle for repeatable tests.
- Log tile consumption, day transitions, and pauses triggered by dialogs or battles.

## MVP Plan for humans
1. Prepare a focused demo map with two or three POIs, a dialog quest, and one battle encounter.
2. Test step throttling, dialog/battle pauses, and interrupted paths; iterate on tuning.

## MVP Plan for agents
1. Lock gameplay to one hero per player, keep one human player, and allow a second non-human slot for autonomous neutral/hostile actors if needed; hide economy through a mod.
   - Create `Mods/step-prototype/mod.json` and populate overrides under `Mods/step-prototype/Content/config/` so `gameConfig.json` sets `settings.heroes.perPlayerOnMapCap = 1`, `settings.heroes.perPlayerTotalCap = 1`, and introduces a boolean `stepModeEnabled` that core code can read.
   - Register the new flag in settings plumbing: add `STEP_MODE_ENABLED` to `EGameSettings` (`lib/IGameSettings.h:55`), map it in `lib/GameSettings.cpp:78` and default it in the constructor, and extend the schema at `config/schemas/gameSettings.json:41` so mods validate.
   - Enforce the single-hero cap wherever heroes leave or enter play by checking the flag in `server/processors/HeroPoolProcessor.cpp:165`, `server/CGameHandler.cpp:2503`, and `lib/mapObjects/CGHeroInstance.cpp:565` before allowing recruitment, garrison swaps, or similar flows.
   - Lock the lobby to one human but allow a second non-human slot if needed: short-circuit `OptionsTab::onSetPlayerClicked` (`client/lobby/OptionsTab.cpp:1154`) so only one human can be assigned, tighten `CVCMIServer::setPlayer` (`server/CVCMIServer.cpp:599`), and fold any extra `playerInfos` to AI inside `updateStartInfoOnMapChange` (`server/CVCMIServer.cpp:487`) when Step mode is active.
   - Hide the economy layer when the flag is set: filter out marketplace/resource slots while building the hall grid in `client/windows/CCastleInterface.cpp:1738`, skip drawing the adventure-map resource bar in `client/adventureMap/CResDataBar.cpp:35`, and drop unused buildings via faction overrides under `Mods/step-prototype/Content/config/factions/`.
   - Convert turn/day flow into an endless turn: guard the daily start/end-turn cycle in `server/CGameHandler.cpp` with `stepModeEnabled` so the single player never auto-ends; move daily/weekly resource processing into a real-time scheduler in `server/processors/NewTurnProcessor.cpp`; and treat network turn notifications as no-ops on the client (`client/GameInstance.cpp`, `client/GameEngine.cpp`) while the flag is active.
2. Add a desktop debug provider that increments steps via hotkey or timer for development.
3. Build `StepMovementController` that consumes steps, advances tiles, and handles path cancelation.
4. Create HUD for step counter.
5. Implement the iOS step provider with `CMPedometer`, exposing `stepsUpdated(totalSteps)` to Qt.
6. Tie day advancement to real time by ending and restarting the player turn on schedule.

## Local build setup (macOS + iOS)
- Keep separate Conan output folders to avoid toolchain clashes: `conan-generated-macos/` and `conan-generated-ios/` (do not reuse `conan-generated/` across platforms).
- Regenerate toolchains when dependencies change:
  - macOS: `conan install . --output-folder=conan-generated-macos --profile=dependencies/conan_profiles/macos-arm --profile=dependencies/conan_profiles/base/apple-system --build=never`
  - iOS: `conan install . --output-folder=conan-generated-ios --profile=dependencies/conan_profiles/ios-arm64 --profile=dependencies/conan_profiles/base/apple-system --build=never`
- Use local presets that pin each toolchain and build dir (see `CMakeUserPresets.json`):
  - macOS configure/build: `cmake --preset macos-conan-ninja-release-local` then `cmake --build --preset macos-conan-ninja-release-local`
  - iOS configure/build: `cmake --preset ios-device-conan-local` then `cmake --build --preset ios-device-conan-local`
- Keep build directories distinct: `out/build/macos-conan-ninja-release-local` for macOS, `out/build/build-ios-device` for iOS (change in `CMakeUserPresets.json` if you want a different path).
- Avoid adding Conan-generated preset includes to `CMakeUserPresets.json` to prevent duplicate preset names (like `conan-release`).

# Workflow
You need to think through, analyse and draft a spec for implementing a feature. Put it into `drafts/###-objective-description.md`, where number is incrementing from `001`. Make sure to restate the task and outline steps for implementing it. Provide sample code snippets if needed to demonstrate. When choosing what to draft next, think what is necessary to do next within the MVP scope outlined in this document. When implementing the task reflect what did not go as planned in respective draft file (in section Deviations During Implementation).

# Testing
After implementation verify that macos build compiles.
