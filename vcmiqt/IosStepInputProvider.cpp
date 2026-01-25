/*
 * IosStepInputProvider.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "IosStepInputProvider.h"

#include "StepPedometer.h"

#include <QMetaObject>

IosStepInputProvider::IosStepInputProvider(QObject * parent)
	: StepInputProvider(parent)
	, pedometer(std::make_unique<iOS_utils::StepPedometer>())
{
	pedometer->setCallback([this](int64_t totalSteps) {
		handleSteps(static_cast<qint64>(totalSteps));
	});
}

IosStepInputProvider::~IosStepInputProvider()
{
	if (pedometer)
	{
		pedometer->setCallback({});
		pedometer->stop();
	}
}

void IosStepInputProvider::start()
{
	if (pedometer)
		pedometer->start();
}

void IosStepInputProvider::stop()
{
	if (pedometer)
		pedometer->stop();
}

void IosStepInputProvider::reset()
{
	if (pedometer)
		pedometer->reset();

	emitSteps(0);
}

void IosStepInputProvider::handleSteps(qint64 totalSteps)
{
	QMetaObject::invokeMethod(this, [this, totalSteps]() {
		emitSteps(totalSteps);
	}, Qt::QueuedConnection);
}
