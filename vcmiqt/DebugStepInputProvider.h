/*
 * DebugStepInputProvider.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "StepInputProvider.h"

#include <QTimer>

class VCMIQT_LINKAGE DebugStepInputProvider final : public StepInputProvider
{
	Q_OBJECT

public:
	explicit DebugStepInputProvider(QObject * parent = nullptr);
	~DebugStepInputProvider() override = default;

	void start() override;
	void stop() override;
	void reset() override;
	void setPaused(bool paused) override;

	void addSteps(int steps);
	void setTimerEnabled(bool enabled);
	void toggleTimer();
	void setStepsPerTick(int steps);
	void setIntervalMs(int intervalMs);
	bool isTimerEnabled() const;
	qint64 totalSteps() const;

private:
	void addStepsInternal(int steps);
	void updateTimerState();

	QTimer timer;
	qint64 totalStepsValue = 0;
	int stepsPerTick = 25;
	int intervalMsValue = 1000;
	bool timerEnabled = false;
};
