#include "Cheat.h"

void CheatThread()
{
    LOG_INIT;

	U::Setup();

	// loop for functions/features that are not needed in a hooked function
	while( !G::PressedKeys[ VK_END ] )
	{
		if( I::Engine->IsInGame() )
		{
			if( Vars.Misc.ChatSpam )
				E::Misc->Chatspam();

			if( Vars.Misc.Ranks && G::PressedKeys[ VK_TAB ] )
				U::ServerRankRevealAll();
		}

		Sleep( 1 );
	}

	E::Misc->Panic();
}

DWORD WINAPI DllMain( HMODULE hDll, DWORD dwReason, LPVOID lpReserved )
{
	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			while( !( G::Window = FindWindowA( charenc( "Valve001" ), NULL ) ) )
				Sleep( 200 );

			Hooks::oldWindowProc = ( WNDPROC )SetWindowLongPtr( G::Window, GWL_WNDPROC, ( LONG_PTR )Hooks::WndProc );

			CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )CheatThread, NULL, NULL, NULL );

			G::Dll = hDll;

			return TRUE;
		}
		case DLL_PROCESS_DETACH:
		{
			return TRUE;
		}

			return FALSE;
	}
}
