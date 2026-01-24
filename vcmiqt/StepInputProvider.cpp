/*
 * StepInputProvider.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "StepInputProvider.h"

StepInputProvider::StepInputProvider(QObject * parent)
	: QObject(parent)
{
}

void StepInputProvider::start()
{
}

void StepInputProvider::stop()
{
}

void StepInputProvider::reset()
{
}

void StepInputProvider::setPaused(bool pausedValue)
{
	paused = pausedValue;
}

bool StepInputProvider::isPaused() const
{
	return paused;
}

void StepInputProvider::emitSteps(qint64 totalSteps)
{
	if (paused)
		return;

	emit stepsUpdated(totalSteps);
}
