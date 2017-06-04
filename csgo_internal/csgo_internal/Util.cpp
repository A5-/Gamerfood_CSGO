#include "sdk.h"

MsgFn U::PrintMessage = ( MsgFn )GetProcAddress( GetModuleHandleA( charenc( "tier0.dll" ) ), charenc( "Msg" ) );
ServerRankRevealAllFn U::ServerRankRevealAllEx;
InitKeyValuesFn U::InitKeyValuesEx;
LoadFromBufferFn U::LoadFromBufferEx;
IsReadyFn U::IsReady;

void U::SetupInterfaces()
{
	I::Client = U::CaptureInterface< IBaseClientDll >( strenc( "client.dll" ), strenc( "VClient018" ) );
	I::ClientMode = **( IClientModeShared*** ) ( ( *( DWORD** ) I::Client )[ 10 ] + 0x5 );
	I::ClientEntList = U::CaptureInterface< IClientEntityList >( strenc( "client.dll" ), strenc( "VClientEntityList003" ) );
	I::Cvar = U::CaptureInterface< ICVar >( strenc( "vstdlib.dll" ), strenc( "VEngineCvar007" ) );
	I::Engine = U::CaptureInterface< IEngineClient >( strenc( "engine.dll" ), strenc( "VEngineClient014" ) );
	I::EngineTrace = U::CaptureInterface< IEngineTrace >( strenc( "engine.dll" ), strenc( "EngineTraceClient004" ) );
	I::InputSystem = U::CaptureInterface< IInputSystem >( strenc( "inputsystem.dll" ), strenc( "InputSystemVersion001" ) );
	I::Globals = **( IGlobalVarsBase*** ) ( ( *( DWORD** ) I::Client )[0] + 0x1B );
	I::Surface = U::CaptureInterface< ISurface >( strenc( "vguimatsurface.dll" ), strenc( "VGUI_Surface031" ) );
	I::VPanel = U::CaptureInterface< IVPanel >( strenc( "vgui2.dll" ), strenc( "VGUI_Panel009" ) );
	I::RenderView = U::CaptureInterface< IVRenderView >( strenc( "engine.dll" ), strenc( "VEngineRenderView014" ) );
	I::ModelRender = U::CaptureInterface< IVModelRender >( strenc( "engine.dll" ), strenc( "VEngineModel016" ) );
	I::MaterialSystem = U::CaptureInterface< IMaterialSystem >( strenc( "materialsystem.dll" ), strenc( "VMaterialSystem080" ) );
	I::vrtEffects = U::CaptureInterface< IVEffects >( strenc("engine.dll"), strenc("VEngineEffects001") );
	I::ModelInfo = U::CaptureInterface< IVModelInfo >( strenc( "engine.dll" ), strenc( "VModelInfoClient004" ) );
	I::Prediction = U::CaptureInterface< IPrediction >( strenc( "client.dll" ), strenc( "VClientPrediction001" ) );
	I::Physprops = U::CaptureInterface< IPhysicsSurfaceProps >( strenc( "vphysics.dll" ), strenc( "VPhysicsSurfaceProps001" ) );
	I::DebugOverlay = U::CaptureInterface< IVDebugOverlay >( strenc( "engine.dll" ), strenc( "VDebugOverlay004" ) );
	I::StudioRender = U::CaptureInterface< IStudioRender >( strenc( "studiorender.dll" ), strenc( "VStudioRender026" ) );
}

// learn_more
DWORD U::FindPattern( std::string moduleName, std::string pattern )
{
	const char* pat = pattern.c_str();
	DWORD firstMatch = 0;
	DWORD rangeStart = ( DWORD )GetModuleHandleA( moduleName.c_str() );
	MODULEINFO miModInfo;
	GetModuleInformation( GetCurrentProcess(), ( HMODULE )rangeStart, &miModInfo, sizeof( MODULEINFO) );
	DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
	for( DWORD pCur = rangeStart; pCur < rangeEnd; pCur++ )
	{
		if( !*pat )
			return firstMatch;

		if( *( PBYTE )pat == '\?' || *( BYTE* )pCur == getByte( pat ) )
		{
			if( !firstMatch )
				firstMatch = pCur;

			if( !pat[ 2 ] )
				return firstMatch;

			if( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' )
				pat += 3;

			else
				pat += 2; //one ?
		}
		else
		{
			pat = pattern.c_str();
			firstMatch = 0;
		}
	}
	return NULL;
}

CBaseEntity* U::GetLocalPlayer()
{
	return I::ClientEntList->GetClientEntity( I::Engine->GetLocalPlayer() );
}

CNetVarManager* U::NetVars = new CNetVarManager;

wchar_t* U::ConvertCharArrayToLPCWSTR( const char* charArray )
{
	wchar_t* wString = new wchar_t[ 4096 ];
	MultiByteToWideChar( CP_ACP, 0, charArray, -1, wString, 4096 );
	return wString;
}

void U::TraceLine( const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, trace_t* ptr )
{
	Ray_t ray;
	ray.Init( vecAbsStart, vecAbsEnd );
	CTraceFilter filter;
	filter.pSkip = ignore;

	I::EngineTrace->TraceRay( ray, mask, &filter, ptr );
}

void U::ServerRankRevealAll()
{
	static float fArray[ 3 ] = { 0.f, 0.f, 0.f };

	U::ServerRankRevealAllEx = ( ServerRankRevealAllFn )( offsets.ServerRankRevealAllEx );
	U::ServerRankRevealAllEx( fArray );
}

void U::InitKeyValues( KeyValues* pKeyValues, const char* name )
{
	U::InitKeyValuesEx = ( InitKeyValuesFn )( offsets.InitKeyValuesEx );
	U::InitKeyValuesEx( pKeyValues, name );
}

void U::LoadFromBuffer( KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc )
{
	U::LoadFromBufferEx = ( LoadFromBufferFn )( offsets.LoadFromBufferEx );
	U::LoadFromBufferEx( pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc );
}

void U::SendPacket( byte toggle )
{
	*( byte* )( offsets.SendPacket ) = toggle;
}

float U::RandomFloat( float min, float max )
{
	assert( max > min );
	float random = ( ( float )rand() ) / ( float )RAND_MAX;
	float range = max - min;
	return ( random * range ) + min;
}

void U::SetupHooks()
{
	H::VPanel = new VTHook( ( DWORD** )I::VPanel );
	H::ClientMode = new VTHook( ( DWORD** )I::ClientMode );
	H::Client = new VTHook( ( DWORD** )I::Client );
	H::ModelRender = new VTHook( ( DWORD** )I::ModelRender );
	H::Surface = new VTHook( ( DWORD** )I::Surface );
	H::D3D9 = new VTHook( ( DWORD** )offsets.d3d9Device );

	oPaintTraverse = ( PaintTraverseFn )H::VPanel->HookFunction( ( DWORD )Hooks::PaintTraverse, 41 );
	oCreateMove = ( CreateMoveFn )H::ClientMode->HookFunction( ( DWORD )Hooks::CreateMove, 24 );
	oOverrideView = ( OverrideViewFn )H::ClientMode->HookFunction( ( DWORD )Hooks::OverrideView, 18 );
	oFrameStageNotify = ( FrameStageNotifyFn )H::Client->HookFunction( ( DWORD )Hooks::FrameStageNotify, 36 );
	oDrawModelExecute = ( DrawModelExecuteFn )H::ModelRender->HookFunction( ( DWORD )Hooks::DrawModelExecute, 21 );
	oPlaySound = ( PlaySoundFn )H::Surface->HookFunction( ( DWORD )Hooks::PlaySound_CSGO, 82 );
	oReset = ( ResetFn )H::D3D9->HookFunction( ( DWORD )Hooks::Reset, 16 );
	oEndScene = ( EndSceneFn )H::D3D9->HookFunction( ( DWORD )Hooks::EndScene, 42 );
}

void U::SetupOffsets()
{
	U::NetVars->Initialize();
	Offsets::GrabOffsets();
}

void U::SetupTextures()
{
	visible_flat = I::MaterialSystem->CreateMaterial( true, false, false );
	visible_tex = I::MaterialSystem->CreateMaterial( false, false, false );
	hidden_flat = I::MaterialSystem->CreateMaterial( true, true, false );
	hidden_tex = I::MaterialSystem->CreateMaterial( false, true, false );
}

void U::Setup()
{
	U::SetupInterfaces();

    LOG("Interfaces setup!");

	U::SetupOffsets();

    LOG("Offsets setup!");

	D::SetupFonts();

    LOG("Fonts setup!");

	U::SetupTextures();

    LOG("Textures setup!");

	U::SetupHooks();

    LOG("Hooks setup!");

	Config->Load();

    LOG("Config Loaded!");
}
