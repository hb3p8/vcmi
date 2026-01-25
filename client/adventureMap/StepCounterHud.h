/*
 * StepCounterHud.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "../gui/CIntObject.h"

#include <cstdint>
#include <memory>

class CLabel;
class TransparentFilledRectangle;

class StepCounterHud : public CIntObject
{
public:
	explicit StepCounterHud(const Rect & area);

private:
	void show(Canvas & to) override;
	void showAll(Canvas & to) override;
	void tick(uint32_t msPassed) override;

	bool shouldShow() const;
	void updateText(bool force);

	std::shared_ptr<TransparentFilledRectangle> background;
	std::shared_ptr<CLabel> stepsLabel;
	std::shared_ptr<CLabel> statusLabel;
	int64_t lastSteps;
	int64_t lastConsumed;
	bool lastTimerEnabled;
	bool lastPaused;
};
