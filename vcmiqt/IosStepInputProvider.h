/*
 * IosStepInputProvider.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "StepInputProvider.h"

#include <memory>

namespace iOS_utils
{
class StepPedometer;
}

class VCMIQT_LINKAGE IosStepInputProvider final : public StepInputProvider
{
public:
	explicit IosStepInputProvider(QObject * parent = nullptr);
	~IosStepInputProvider() override;

	void start() override;
	void stop() override;
	void reset() override;

private:
	void handleSteps(qint64 totalSteps);

	std::unique_ptr<iOS_utils::StepPedometer> pedometer;
};
