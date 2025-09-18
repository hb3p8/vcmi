# Step-Driven Adventure Ideas

## Vision
- Transform the HoMM3 adventure layer into a single-hero story campaign that advances through real-world steps counted on iOS devices.
- Preserve tactical turn-based combat and classic town/hero mechanics where they reinforce narrative progression.
- Deliver a walk-powered RPG loop: every walk session unlocks new map tiles, story beats, and skill mastery.

## Core Loop
1. **Queue Route** - Player taps a destination on the overworld; VCMI pathfinder returns a path.
2. **Walk & Bank Steps** - CMPedometer feeds cumulative steps. A configurable ratio (default 100 steps per tile) converts steps into tile credits.
3. **Advance Tile** - When credits meet or exceed terrain cost, the hero walks one tile. Movement animations stay intact; the engine ends the tile move before consuming more steps.
4. **Trigger Events** - On entering nodes, kick off story dialogues, resource pickups, or combat. Steps pause during dialogs/battles.
5. **Reset Goal** - Upon finishing the path, the player selects a new goal or spends time on local activities (crafting, quest decisions, map interactions).

## Step Economy & Scaling
- **Terrain Multipliers** - Grass 1.0x, road 0.75x, difficult terrain 1.5-2.0x to mimic HoMM fatigue while rewarding route planning.
- **Gear Buffs** - Equipment from quests provides bonuses (boots -10 percent step cost, ranger cloak negates swamp penalty).
- **Daily Cap** - Optional soft cap (for example 10k steps) after which tile cost gradually increases to encourage rest while allowing hardcore play.
- **Offline Queue** - Allow pre-planned routes so morning walks automatically advance the hero and surface notifications when events fire.

## World Structure
- **Story Regions** - Reuse HoMM tilesets but focus on handcrafted quest chains (Haunted Forest, Sunken Coast, Mountain Pass).
- **Safe Hubs** - Town screens act as story hubs with NPCs, crafting, and long-form quests; economy buildings mainly provide buffs/resources rather than empire management.
- **Dynamic Hazards** - Weather fronts or magical anomalies block routes until certain skills/items are unlocked (mirrors Walkscape hazards).

## Skills & Progression
- **Exploration Skills** - Cartography (reveals map sectors), Survival (reduces terrain costs), Navigation (unlock ferries/portals).
- **Story Skills** - Diplomacy (alternate quest resolutions), Lore (unlocks backstory codex entries), Artificing (craft unique artifacts).
- **Combat Tie-ins** - Steps grant Adventure XP while combats provide classic HoMM experience and spell points. Level-ups can award passive step perks.
- **Daily Tasks** - Three rotating objectives (for example Walk 1500 steps on roads, Visit two shrines) granting consumables or step efficiency boosts.

## Activities Between Walks
- **Crafting & Preparation** - Use gathered resources at towns/camps to cook rations (temporary speed buffs), craft talismans (hazard immunity), brew potions (battle buffs).
- **Quest Decision Windows** - Some story nodes require player choice via desktop UI; decisions can unlock new step goals.
- **Fast-time Tasks** - When stationary, allow short training timers that convert spare artifacts or gold into buffs, mirroring Walkscape idle actions.

## Day & Timekeeping
- **Real-Time Calendar** - Day flips at local midnight regardless of in-game movement. Each new day refreshes objectives, resource nodes, and limited shops.
- **Weekly Events** - Every 7 days introduce map changes (new enemy patrols, market discounts) to keep long walks meaningful.
- **Seasonal Arcs** - Four-week cycles with themed quests and cosmetic map swaps, encouraging sustained engagement.

## Encounter Design
- **Story Battles** - Key encounters remain tactical HoMM fights with curated armies; reward victory with artifacts and lore rather than territory control.
- **Ambient Creatures** - Optional fights triggered on certain tiles (for example high grass spawn rate) with chance to drop crafting materials.
- **Non-Combat Challenges** - Skill checks (Survival 5, Diplomacy 4) that consume crafted items instead of forcing combat, inspired by Walkscape event checks.

## UX Considerations
- **HUD Overlay** - Always-visible step counter, steps-to-next-tile meter, queued destination preview, and pause toggle.
- **Notifications** - iOS local notifications when a queued path finishes, when a day/week rolls over, or when an ongoing task completes.
- **Accessibility** - Manual step input for testing/low-mobility players, plus adjustable step-to-tile ratios and hazard toggles.

## Content Pipeline
- **Map Editor** - Designers author narrative regions with VCMI map editor, tagging tiles with step costs, hazards, and encounter scripts.
- **Lua Narrative Scripts** - Dialogue trees, branching quests, and daily refresh logic scripted in Lua for quick iteration.
- **Data Tables** - CSV/JSON configs for step ratios, gear perks, and daily task pools to support rapid tuning.

## Future Extensions
- **Companion App** - Watch-first UI to set destinations and track progress without opening the main client.
- **Community Routes** - Allow players to publish scenic walking routes that map to in-game expeditions with unique rewards.
- **Cooperative Expeditions** - Later add asynchronous co-op where friends pool steps to clear large story portals while still playing single-hero locally.

## Immediate Prototyping Targets
- Implement step budget controller and HUD.
- Build a three-region demo map featuring: one safe hub, one hazard-ridden route, one story dungeon.
- Script three daily task templates and weekly refresh logic.
- Integrate crafting prototype (rations, talismans) to test non-combat rewards for walking.
