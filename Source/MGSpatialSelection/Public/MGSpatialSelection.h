// MG Spatial Selection System by Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "Modules/ModuleManager.h"

class FMGSpatialSelectionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
