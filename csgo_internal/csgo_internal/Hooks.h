#pragma once

using CreateMoveFn = bool( __stdcall*)( float, CUserCmd* );
extern CreateMoveFn oCreateMove;

using FrameStageNotifyFn = void( __stdcall*)( ClientFrameStage_t );
extern FrameStageNotifyFn oFrameStageNotify;

using PaintTraverseFn = void( __stdcall*)( unsigned int, bool, bool );
extern PaintTraverseFn oPaintTraverse;

using OverrideViewFn = void( __stdcall*)( CViewSetup* );
extern OverrideViewFn oOverrideView;

using DrawModelExecuteFn = void*( __stdcall*)( void*, void*, const ModelRenderInfo_t&, matrix3x4_t* );
extern DrawModelExecuteFn oDrawModelExecute;

using InPredictionFn = bool( __stdcall*)();
extern InPredictionFn oInPrediction;

using PlaySoundFn = void( __stdcall*)( const char* );
extern PlaySoundFn oPlaySound;

using EndSceneFn = long( __stdcall*)( IDirect3DDevice9* device );
extern EndSceneFn oEndScene;

using ResetFn = long( __stdcall*)( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp );
extern ResetFn oReset;

using OverrideMouseInputFn = void( __stdcall*)( float* pX, float* pY );
extern OverrideMouseInputFn oOverrideMouseInput;

namespace Hooks
{
	extern bool __stdcall CreateMove( float flInputSampleTime, CUserCmd* cmd );
	extern void __stdcall FrameStageNotify( ClientFrameStage_t stage );
	extern void __stdcall PaintTraverse( unsigned int panel, bool forceRepaint, bool allowForce );
	extern void __stdcall OverrideView( CViewSetup* vsView );
	extern void __stdcall DrawModelExecute( void* context, void* state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld );
	extern void __stdcall PlaySound_CSGO( const char* fileName );
	extern long __stdcall EndScene( IDirect3DDevice9* pDevice );
	extern long __stdcall Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters );

	extern WNDPROC oldWindowProc;
	extern LRESULT __stdcall WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
}
