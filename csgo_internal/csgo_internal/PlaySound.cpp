#include "Cheat.h"

// c&p from Mark_HC's source because it worked

PlaySoundFn oPlaySound;

void __stdcall Hooks::PlaySound_CSGO( const char* fileName )
{
	static std::string Buffer = "";
	
	oPlaySound( fileName );

	if( offsets.IsReadyEx == 0 || !fileName || *fileName == '\0' || !I::Engine || I::Engine->IsConnected() || !Vars.Misc.AutoAccept )
		return;

	Buffer.assign( fileName );
	
	if( Buffer.find( charenc( "beep.wav" ) ) != -1 )
	{
		// Restores Window If Needed
		if (GetForegroundWindow() != G::Window)
		{
			ShowWindow(G::Window, SW_RESTORE);
			SetActiveWindow(G::Window);
		}

		// Accepts The Game
		U::IsReady = ( IsReadyFn )( offsets.IsReadyEx );
		U::IsReady();
	}
}
