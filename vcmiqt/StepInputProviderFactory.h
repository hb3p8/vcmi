/*
 * StepInputProviderFactory.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

#include "vcmiqt.h"

#include <memory>

class QObject;
class StepInputProvider;

VCMIQT_LINKAGE std::unique_ptr<StepInputProvider> createStepInputProvider(QObject * parent = nullptr);
