/*
 * StepDebugProvider.h, part of VCMI engine
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
#include <functional>
#include <vector>

class StepDebugProvider
{
public:
	using StepsListener = std::function<void(int64_t)>;

	StepDebugProvider();

	void addSteps(int64_t steps);
	void reset();
	void setTimerEnabled(bool enabled);
	void toggleTimer();
	bool isTimerEnabled() const;
	void setStepsPerTick(int steps);
	void setIntervalMs(int intervalMs);
	int64_t totalSteps() const;
	int64_t consumedSteps() const;
	int64_t availableSteps() const;
	void consumeSteps(int64_t steps);
	void resetConsumption();

	void addListener(const StepsListener & listener);
	void update();

private:
	void addStepsInternal(int64_t steps, bool fromTimer);
	void notifyListeners();
	void updateTimer(std::chrono::steady_clock::time_point now);

	std::vector<StepsListener> listeners;
	int64_t totalStepsValue = 0;
	int64_t consumedStepsValue = 0;
	int stepsPerTick = 25;
	int intervalMsValue = 1000;
	bool timerEnabled = false;
	bool hasLastTick = false;
	std::chrono::steady_clock::time_point lastTick;
};
