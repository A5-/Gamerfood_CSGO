#include "Cheat.h"

// c&p from Mark_HC's source because it worked

PlaySoundFn oPlaySound;
void __stdcall Hooks::PlaySound_CSGO( const char* fileName )
{
	oPlaySound( fileName );
	
	if( I::Engine->IsInGame() || !Vars.Misc.AutoAccept ) 
		return;
	
	if( !strcmp( fileName, "weapons/hegrenade/beep.wav" ) ) {

		//Accept the game
		U::IsReady = ( IsReadyFn )( offsets.IsReadyEx );
		U::IsReady();

		//This will flash the CSGO window on the taskbar
		//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
		FLASHWINFO fi;
		fi.cbSize = sizeof( FLASHWINFO );
		fi.hwnd = G::Window;
		fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		fi.uCount = 0;
		fi.dwTimeout = 0;
		FlashWindowEx( &fi );
	}
}