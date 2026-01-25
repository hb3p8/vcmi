/*
 * StepPedometer.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#pragma GCC visibility push(default)
namespace iOS_utils
{
class StepPedometer
{
public:
	using StepsCallback = std::function<void(int64_t totalSteps)>;

	StepPedometer();
	~StepPedometer();

	StepPedometer(const StepPedometer &) = delete;
	StepPedometer & operator=(const StepPedometer &) = delete;

	bool isAvailable() const;
	void start();
	void stop();
	void reset();
	void setCallback(StepsCallback callback);

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};
}
#pragma GCC visibility pop
