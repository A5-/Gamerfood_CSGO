#include "Cheat.h"

// c&p from Mark_HC's source because it worked

PlaySoundFn oPlaySound;

void __stdcall Hooks::PlaySound_CSGO( const char* fileName )
{
	oPlaySound( fileName );

    std::string file_name(fileName);

	if( offsets.IsReadyEx == 0 
        || file_name.empty() 
        || I::Engine->IsConnected() 
        || !Vars.Misc.AutoAccept )
		return;
	
	if(file_name.find( charenc( "accept_beep.wav" ) ) != -1 )
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
