/*
 * StepPedometer.mm, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StepPedometer.h"

#import <CoreMotion/CoreMotion.h>

#include <mutex>

namespace
{
struct CallbackState
{
	std::mutex mutex;
	iOS_utils::StepPedometer::StepsCallback callback;
	bool active = true;
};
}

namespace iOS_utils
{
struct StepPedometer::Impl
{
	CMPedometer * pedometer = nil;
	NSDate * startDate = nil;
	bool running = false;
	bool available = false;
	std::shared_ptr<CallbackState> callbackState = std::make_shared<CallbackState>();
};

StepPedometer::StepPedometer()
	: impl(std::make_unique<Impl>())
{
	impl->pedometer = [[CMPedometer alloc] init];
	impl->available = [CMPedometer isStepCountingAvailable];
}

StepPedometer::~StepPedometer()
{
	if (impl && impl->callbackState)
	{
		std::lock_guard<std::mutex> lock(impl->callbackState->mutex);
		impl->callbackState->active = false;
		impl->callbackState->callback = {};
	}
	stop();
}

bool StepPedometer::isAvailable() const
{
	return impl->available;
}

void StepPedometer::start()
{
	if (!impl->available || impl->running)
		return;

	if (!impl->startDate)
		impl->startDate = [NSDate date];

	auto callbackState = impl->callbackState;
	[impl->pedometer startPedometerUpdatesFromDate:impl->startDate
		withHandler:^(CMPedometerData * data, NSError * error)
	{
		if (error || !data || !callbackState)
			return;

		StepPedometer::StepsCallback callback;
		{
			std::lock_guard<std::mutex> lock(callbackState->mutex);
			if (!callbackState->active)
				return;
			callback = callbackState->callback;
		}

		if (!callback)
			return;

		callback(static_cast<int64_t>(data.numberOfSteps.longLongValue));
	}];

	impl->running = true;
}

void StepPedometer::stop()
{
	if (!impl->running)
		return;

	[impl->pedometer stopPedometerUpdates];
	impl->running = false;
}

void StepPedometer::reset()
{
	impl->startDate = [NSDate date];

	if (impl->running)
	{
		stop();
		start();
	}
}

void StepPedometer::setCallback(StepsCallback callback)
{
	std::lock_guard<std::mutex> lock(impl->callbackState->mutex);
	impl->callbackState->callback = std::move(callback);
}
}
