#pragma once

#define RandomInt(min, max) (rand() % (max - min + 1) + min)

#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

#define MakePtr (cast, ptr, addValue) ( cast )( (DWORD)( ptr ) + ( DWORD )( addValue ) )

class KeyValues;

using MsgFn = void( __cdecl*)( char const*, ... );
using ServerRankRevealAllFn = bool( __cdecl*)( float* );
using InitKeyValuesFn = void( __thiscall*)( void* thisptr, const char* name );
using LoadFromBufferFn = void( __thiscall*)( void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc );
using IsReadyFn = void( __cdecl*)();

namespace U
{
	extern MsgFn PrintMessage;
	extern ServerRankRevealAllFn ServerRankRevealAllEx;
	extern InitKeyValuesFn InitKeyValuesEx;
	extern LoadFromBufferFn LoadFromBufferEx;
	extern IsReadyFn IsReady;

	extern DWORD FindPattern( std::string moduleName, std::string pattern );
	extern CBaseEntity* GetLocalPlayer();
	extern CNetVarManager* NetVars;
	extern void SpoofCvar( ConVar* pCvar );
	extern wchar_t* ConvertCharArrayToLPCWSTR( const char* charArray );
	extern void TraceLine( const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, CBaseEntity* ignore, trace_t* ptr );
	extern unsigned short GetAchievementEventMask();
	extern void ServerRankRevealAll();
	extern void InitKeyValues( KeyValues* pKeyValues, const char* name );
	extern void LoadFromBuffer( KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem = nullptr, const char* pPathID = NULL, void* pfnEvaluateSymbolProc = nullptr );
	extern void SendPacket( byte toggle );
	extern void SetupInterfaces();
	extern void SetupHooks();
	extern void SetupOffsets();
	extern void SetupTextures();
	extern void Setup();

	template< class T, class Y >
	T Clamp( T const& val, Y const& minVal, Y const& maxVal )
	{
		if( val < minVal )
			return minVal;
		else if( val > maxVal )
			return maxVal;
		else
			return val;
	}

	template< typename T >
	T GetVFunc( void* vTable, int iIndex )
	{
		return ( *( T** )vTable )[ iIndex ];
	}

	template< typename T >
	T* CaptureInterface( std::string strModule, std::string strInterface )
	{
		typedef T* ( *CreateInterfaceFn )( const char* szName, int iReturn );
		CreateInterfaceFn CreateInterface = ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( strModule.c_str() ), charenc( "CreateInterface" ) );

		return CreateInterface( strInterface.c_str(), 0 );
	}

	extern float RandomFloat( float min, float max );
}
