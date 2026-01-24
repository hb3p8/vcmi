/*
 * StepInputProvider.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "vcmiqt.h"

#include <QObject>

class VCMIQT_LINKAGE StepInputProvider : public QObject
{
	Q_OBJECT

public:
	explicit StepInputProvider(QObject * parent = nullptr);
	~StepInputProvider() override = default;

	virtual void start();
	virtual void stop();
	virtual void reset();
	virtual void setPaused(bool paused);

	bool isPaused() const;

signals:
	void stepsUpdated(qint64 totalSteps);

protected:
	void emitSteps(qint64 totalSteps);

private:
	bool paused = false;
};
