/*
 * StepDebugProvider.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "StepDebugProvider.h"

StepDebugProvider::StepDebugProvider() = default;

void StepDebugProvider::addSteps(int64_t steps)
{
	addStepsInternal(steps, false);
}

void StepDebugProvider::reset()
{
	totalStepsValue = 0;
	consumedStepsValue = 0;
	notifyListeners();
}

void StepDebugProvider::setTimerEnabled(bool enabled)
{
	timerEnabled = enabled;
	hasLastTick = false;
}

void StepDebugProvider::toggleTimer()
{
	setTimerEnabled(!timerEnabled);
	logGlobal->info("Step debug timer %s", timerEnabled ? "enabled" : "disabled");
}

bool StepDebugProvider::isTimerEnabled() const
{
	return timerEnabled;
}

void StepDebugProvider::setStepsPerTick(int steps)
{
	if (steps <= 0)
		return;

	stepsPerTick = steps;
}

void StepDebugProvider::setIntervalMs(int intervalMs)
{
	if (intervalMs <= 0)
		return;

	intervalMsValue = intervalMs;
}

int64_t StepDebugProvider::totalSteps() const
{
	return totalStepsValue;
}

int64_t StepDebugProvider::consumedSteps() const
{
	return consumedStepsValue;
}

int64_t StepDebugProvider::availableSteps() const
{
	return std::max<int64_t>(totalStepsValue - consumedStepsValue, 0);
}

void StepDebugProvider::consumeSteps(int64_t steps)
{
	if (steps <= 0 || totalStepsValue <= 0)
		return;

	consumedStepsValue = std::min<int64_t>(consumedStepsValue + steps, totalStepsValue);
}

void StepDebugProvider::resetConsumption()
{
	consumedStepsValue = 0;
}

void StepDebugProvider::addListener(const StepsListener & listener)
{
	listeners.push_back(listener);
}

void StepDebugProvider::update()
{
	if (!timerEnabled)
		return;

	updateTimer(std::chrono::steady_clock::now());
}

void StepDebugProvider::addStepsInternal(int64_t steps, bool fromTimer)
{
	if (steps <= 0)
		return;

	totalStepsValue += steps;
	notifyListeners();

	if (fromTimer)
		logGlobal->trace("Step debug: +%lld (timer) total=%lld", static_cast<long long>(steps), static_cast<long long>(totalStepsValue));
	else
		logGlobal->info("Step debug: +%lld total=%lld", static_cast<long long>(steps), static_cast<long long>(totalStepsValue));
}

void StepDebugProvider::notifyListeners()
{
	for (const auto & listener : listeners)
		listener(totalStepsValue);
}

void StepDebugProvider::updateTimer(std::chrono::steady_clock::time_point now)
{
	const auto interval = std::chrono::milliseconds(intervalMsValue);

	if (!hasLastTick)
	{
		lastTick = now;
		hasLastTick = true;
		return;
	}

	while (now - lastTick >= interval)
	{
		lastTick += interval;
		addStepsInternal(stepsPerTick, true);
	}
}
