#pragma once

#include "Entity.h"
#include "Cheat.h"
#include "Enhancements.h"

namespace G // Global Stuff
{
	extern bool Aimbotting;
	extern bool InAntiAim;
	extern QAngle LastAngle;
	extern QAngle LastAngle2;
	extern CBaseEntity* LocalPlayer;
	extern bool Return;
	extern CUserCmd* UserCmd;
	extern HMODULE Dll;
	extern HWND Window;
	extern bool PressedKeys[ 256 ];
	extern bool d3dinit;
	extern float FOV;
	extern int ChamMode;
	extern bool SendPacket;
	extern int BestTarget;
}

namespace H // Global Hooks
{
	extern VTHook* VPanel;
	extern VTHook* ClientMode;
	extern VTHook* Client;
	extern VTHook* Prediction;
	extern VTHook* ModelRender;
	extern VTHook* Surface;
	extern VTHook* D3D9;
}

namespace E
{
	extern CLegitBot* LegitBot;
	extern CRageBot* RageBot;
	extern CVisuals* Visuals;
	extern CMiscellaneous* Misc;
}
