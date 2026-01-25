/*
 * StepMovementController.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "StepMovementController.h"

#include "../CPlayerInterface.h"
#include "../GameEngine.h"
#include "../GameInstance.h"
#include "../HeroMovementController.h"
#include "../PlayerLocalState.h"
#include "../step/StepDebugProvider.h"
#include "../adventureMap/AdventureMapInterface.h"
#include "../gui/WindowHandler.h"
#include "../mapView/mapHandler.h"

#include "../../lib/CConfigHandler.h"
#include "../../lib/ConditionalWait.h"
#include "../../lib/IGameSettings.h"
#include "../../lib/callback/CCallback.h"
#include "../../lib/mapObjects/CGHeroInstance.h"
#include "../../lib/pathfinder/CGPathNode.h"

StepMovementController::StepMovementController(CPlayerInterface & owner, HeroMovementController & movementController)
	: owner(owner)
	, movementController(movementController)
{
}

void StepMovementController::onStepsUpdated(int64_t totalSteps)
{
	if (!isStepModeEnabled())
		return;

	if (totalSteps < totalStepsValue)
	{
		totalStepsValue = totalSteps;
		stepBudget = 0;
		if (GAME)
			GAME->stepDebugProvider().resetConsumption();
		return;
	}

	const int64_t delta = totalSteps - totalStepsValue;
	totalStepsValue = totalSteps;
	if (delta > 0)
		stepBudget += delta;
}

void StepMovementController::queueMovement(const CGHeroInstance * hero, const CGPath & path)
{
	if (!isStepModeEnabled())
		return;

	if (!hero || path.nodes.size() < 2)
		return;

	queuedHero = hero;
	queuedPathEnd = path.endPos();
	movementQueued = true;
}

void StepMovementController::clearMovement()
{
	queuedHero = nullptr;
	queuedPathEnd = int3(-1, -1, -1);
	movementQueued = false;
}

void StepMovementController::tick()
{
	tryAdvance();
}

bool StepMovementController::tryConsumeForNextNode(const CGHeroInstance * hero, const CGPath & path)
{
	if (!isStepModeEnabled())
		return false;

	if (!movementQueued || !queuedHero || hero != queuedHero)
		return false;

	if (path.endPos() != queuedPathEnd)
		return false;

	if (path.nodes.size() < 2)
		return false;

	int64_t stepsRequired = stepsRequiredForNextNode(path);
	if (stepBudget < stepsRequired)
		return false;

	stepBudget -= stepsRequired;
	if (GAME)
		GAME->stepDebugProvider().consumeSteps(stepsRequired);

	return true;
}

bool StepMovementController::isStepModeEnabled() const
{
	if (!owner.cb)
		return false;

	return owner.cb->getSettings().getBoolean(EGameSettings::STEP_MODE_ENABLED);
}

bool StepMovementController::canAdvance() const
{
	if (!owner.makingTurn)
		return false;

	if (owner.showingDialog && owner.showingDialog->isBusy())
		return false;

	if (owner.castleInt != nullptr)
		return false;

	if (CPlayerInterface::battleInt)
		return false;

	if (!ENGINE->windows().topWindow<AdventureMapInterface>())
		return false;

	if (movementController.isHeroMoving())
		return false;

	if (GAME->map().hasOngoingAnimations())
		return false;

	return true;
}

bool StepMovementController::isMinIntervalSatisfied(std::chrono::steady_clock::time_point now) const
{
	int minIntervalMs = settings["adventure"]["minTileIntervalMs"].Integer();
	if (minIntervalMs <= 0)
		return true;

	if (!hasLastMoveTime)
		return true;

	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMoveTime).count();
	return elapsedMs >= minIntervalMs;
}

int64_t StepMovementController::stepsRequiredForNextNode(const CGPath & path) const
{
	int stepsPerTile = settings["adventure"]["stepsPerTile"].Integer();
	if (stepsPerTile <= 0)
		stepsPerTile = 1;

	(void)path;
	return stepsPerTile;
}

void StepMovementController::tryAdvance()
{
	if (!isStepModeEnabled())
		return;

	if (!movementQueued || !queuedHero)
		return;

	if (!owner.localState->hasPath(queuedHero))
	{
		clearMovement();
		return;
	}

	const CGPath & path = owner.localState->getPath(queuedHero);
	if (path.endPos() != queuedPathEnd)
	{
		clearMovement();
		return;
	}

	if (path.nodes.size() < 2)
	{
		clearMovement();
		return;
	}

	if (!canAdvance())
		return;

	auto now = std::chrono::steady_clock::now();
	if (!isMinIntervalSatisfied(now))
		return;

	if (!tryConsumeForNextNode(queuedHero, path))
		return;

	CGPath stepPath;
	stepPath.nodes.push_back(path.nodes[path.nodes.size() - 2]);
	stepPath.nodes.push_back(path.nodes[path.nodes.size() - 1]);

	movementController.requestMovementStart(queuedHero, stepPath);
	lastMoveTime = now;
	hasLastMoveTime = true;
}
