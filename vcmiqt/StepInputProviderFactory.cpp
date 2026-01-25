/*
 * StepInputProviderFactory.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "StepInputProviderFactory.h"

#include "DebugStepInputProvider.h"
#include "StepInputProvider.h"
#ifdef VCMI_IOS
#include "IosStepInputProvider.h"
#endif

std::unique_ptr<StepInputProvider> createStepInputProvider(QObject * parent)
{
#ifdef VCMI_IOS
	return std::make_unique<IosStepInputProvider>(parent);
#else
	return std::make_unique<DebugStepInputProvider>(parent);
#endif
}
