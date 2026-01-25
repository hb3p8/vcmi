/*
 * StepCounterHud.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "StepCounterHud.h"

#include "../CPlayerInterface.h"
#include "../GameInstance.h"
#include "../render/Colors.h"
#include "../step/StepDebugProvider.h"
#include "../widgets/GraphicalPrimitiveCanvas.h"
#include "../widgets/TextControls.h"

#include "../../lib/CConfigHandler.h"
#include "../../lib/callback/CCallback.h"
#include "../../lib/IGameSettings.h"

StepCounterHud::StepCounterHud(const Rect & area)
	: CIntObject(TIME)
	, lastSteps(-1)
	, lastConsumed(-1)
	, lastTimerEnabled(false)
	, lastPaused(false)
{
	OBJECT_CONSTRUCTION;
	setRedrawParent(true);
	pos = area;

	auto backgroundRect = Rect(Point(0, 0), pos.dimensions());
	background = std::make_shared<TransparentFilledRectangle>(backgroundRect, ColorRGBA(0, 0, 0, 128), Colors::BRIGHT_YELLOW);

	stepsLabel = std::make_shared<CLabel>(6, 4, FONT_SMALL, ETextAlignment::TOPLEFT, Colors::WHITE, "");
	statusLabel = std::make_shared<CLabel>(6, 18, FONT_TINY, ETextAlignment::TOPLEFT, Colors::WHITE, "");

	updateText(true);
}

void StepCounterHud::show(Canvas & to)
{
	showAll(to);
}

void StepCounterHud::showAll(Canvas & to)
{
	if (!shouldShow())
		return;

	updateText(false);
	CIntObject::showAll(to);
}

void StepCounterHud::tick(uint32_t)
{
	updateText(false);
}

bool StepCounterHud::shouldShow() const
{
	if (!GAME || !GAME->interface())
		return false;

	if (!GAME->interface()->cb)
		return false;

	if (!GAME->interface()->cb->getSettings().getBoolean(EGameSettings::STEP_MODE_ENABLED))
		return false;

	return settings["adventure"]["stepHudVisible"].Bool();
}

void StepCounterHud::updateText(bool force)
{
	if (!shouldShow())
		return;

	const auto & provider = GAME->stepDebugProvider();
	int64_t steps = provider.totalSteps();
	int64_t consumed = provider.consumedSteps();
	bool timerEnabled = provider.isTimerEnabled();
	bool paused = !GAME->interface()->makingTurn;

	if (!force && steps == lastSteps && consumed == lastConsumed && timerEnabled == lastTimerEnabled && paused == lastPaused)
		return;

	std::string stepText = "Steps: " + std::to_string(steps);
	if (consumed > 0)
		stepText += " (spent " + std::to_string(consumed) + ")";
	stepsLabel->setText(stepText);

	std::string status = timerEnabled ? "Timer" : "Manual";
	if (paused)
		status += " | Paused";
	statusLabel->setText(status);

	lastSteps = steps;
	lastConsumed = consumed;
	lastTimerEnabled = timerEnabled;
	lastPaused = paused;
}
