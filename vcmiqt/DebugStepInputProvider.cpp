/*
 * DebugStepInputProvider.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "DebugStepInputProvider.h"

DebugStepInputProvider::DebugStepInputProvider(QObject * parent)
	: StepInputProvider(parent)
{
	timer.setInterval(intervalMsValue);
	connect(&timer, &QTimer::timeout, this, [this]() {
		addStepsInternal(stepsPerTick);
	});
}

void DebugStepInputProvider::start()
{
	updateTimerState();
}

void DebugStepInputProvider::stop()
{
	timer.stop();
}

void DebugStepInputProvider::reset()
{
	totalStepsValue = 0;
	emitSteps(totalStepsValue);
}

void DebugStepInputProvider::setPaused(bool paused)
{
	StepInputProvider::setPaused(paused);
	updateTimerState();
}

void DebugStepInputProvider::addSteps(int steps)
{
	addStepsInternal(steps);
}

void DebugStepInputProvider::setTimerEnabled(bool enabled)
{
	timerEnabled = enabled;
	updateTimerState();
}

void DebugStepInputProvider::toggleTimer()
{
	setTimerEnabled(!timerEnabled);
}

void DebugStepInputProvider::setStepsPerTick(int steps)
{
	stepsPerTick = steps;
}

void DebugStepInputProvider::setIntervalMs(int intervalMs)
{
	intervalMsValue = intervalMs;
	timer.setInterval(intervalMsValue);
	updateTimerState();
}

bool DebugStepInputProvider::isTimerEnabled() const
{
	return timerEnabled;
}

qint64 DebugStepInputProvider::totalSteps() const
{
	return totalStepsValue;
}

void DebugStepInputProvider::addStepsInternal(int steps)
{
	if (steps <= 0)
		return;

	totalStepsValue += steps;
	emitSteps(totalStepsValue);
}

void DebugStepInputProvider::updateTimerState()
{
	if (!timerEnabled || isPaused())
	{
		timer.stop();
		return;
	}

	if (!timer.isActive())
		timer.start();
}
