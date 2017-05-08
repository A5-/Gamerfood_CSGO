#pragma once

#include "IBaseClientDLL.h"
#include "IClientEntityList.h"
#include "IClientModeShared.h"
#include "ICvar.h"
#include "IEngineClient.h"
#include "IEngineTrace.h"
#include "IGlobalVarsBase.h"
#include "IInputSystem.h"
#include "Interfaces.h"
#include "ISurface.h"
#include "IVPanel.h"
#include "IVModelInfo.h"
#include "IVModelRender.h"
#include "IMaterialSystem.h"
#include "IMaterial.h"
#include "IRenderView.h"
#include "IPrediction.h"
#include "fukyou.h"
#include "VPhysics.h"
#include "IVDebugOverlay.h"
#include "IStudioRender.h"

namespace I // Interfaces
{
	extern IBaseClientDll* Client;
	extern IClientModeShared* ClientMode;
	extern IClientEntityList* ClientEntList;
	extern ICVar* Cvar;
	extern IInputSystem* InputSystem;
	extern IEngineClient* Engine;
	extern IEngineTrace* EngineTrace;
	extern IGlobalVarsBase* Globals;
	extern IVEffects* vrtEffects;
	extern ISurface* Surface;
	extern IVPanel* VPanel;
	extern IVModelRender* ModelRender;
	extern IVModelInfo* ModelInfo;
	extern IMaterialSystem* MaterialSystem;
	extern IMaterial* Material;
	extern IVRenderView* RenderView;
	extern IPrediction* Prediction;
	extern IPhysicsSurfaceProps* Physprops;
	extern IVDebugOverlay* DebugOverlay;
	extern IStudioRender* StudioRender;
}
