/*
 * StepMovementController.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <chrono>
#include <cstdint>

#include "../../lib/int3.h"

VCMI_LIB_NAMESPACE_BEGIN
class CGHeroInstance;
struct CGPath;
VCMI_LIB_NAMESPACE_END

class CPlayerInterface;
class HeroMovementController;

class StepMovementController
{
public:
	explicit StepMovementController(CPlayerInterface & owner, HeroMovementController & movementController);

	void onStepsUpdated(int64_t totalSteps);
	void queueMovement(const CGHeroInstance * hero, const CGPath & path);
	void clearMovement();
	void tick();

private:
	bool isStepModeEnabled() const;
	bool canAdvance() const;
	bool isMinIntervalSatisfied(std::chrono::steady_clock::time_point now) const;
	int64_t stepsRequiredForNextNode(const CGPath & path) const;
	void tryAdvance();

	CPlayerInterface & owner;
	HeroMovementController & movementController;

	const CGHeroInstance * queuedHero = nullptr;
	int3 queuedPathEnd = int3(-1, -1, -1);
	bool movementQueued = false;

	int64_t totalStepsValue = 0;
	int64_t stepBudget = 0;
	bool hasLastMoveTime = false;
	std::chrono::steady_clock::time_point lastMoveTime;
};
