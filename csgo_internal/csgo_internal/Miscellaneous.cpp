#include "Cheat.h"
#include <sstream>
#include <fstream>

void CMiscellaneous::Initialize()
{
	if( m_bInitialized )
		return;
}

void CMiscellaneous::Run()
{
	Bunnyhop();
}

void CMiscellaneous::Think()
{}

void CMiscellaneous::Bunnyhop()
{
	static bool bLastJumped = false;
	static bool bShouldFake = false;

	if( !bLastJumped && bShouldFake )
	{
		bShouldFake = false;
		G::UserCmd->buttons |= IN_JUMP;
	}
	else if( G::UserCmd->buttons & IN_JUMP )
	{
		if( G::LocalPlayer->GetFlags() & FL_ONGROUND )
		{
			bLastJumped = true;
			bShouldFake = true;
		}
		else
		{
			G::UserCmd->buttons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldFake = false;
	}
}

void CMiscellaneous::AutoStrafe()
{
	if( G::LocalPlayer->GetFlags() & FL_ONGROUND )
		return;

	if( G::UserCmd->mousedx > 1 || G::UserCmd->mousedx < -1 )
	{
		G::UserCmd->sidemove = G::UserCmd->mousedx < 0.f ? -400.f : 400.f;
	}
}

size_t linenum = 0;

void CMiscellaneous::Chatspam()
{
	static float nextTime = 0.f;
	float flServerTime = G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;

	if( nextTime > flServerTime )
		return;

	nextTime = flServerTime + 0.5f;

	if( m_spamlines.empty() )
		return;

	if( Vars.Misc.ChatSpamMode == 0 )
		linenum = rand() % m_spamlines.size() - 1;

	char str[512];
	strcpy( str, charenc( "say " ) );
	strcat( str, m_spamlines[ linenum ].c_str() );

	I::Engine->ClientCmd_Unrestricted( str, nullptr );

	if( Vars.Misc.ChatSpamMode == 1 )
	{
		linenum++;

		if( linenum >= m_spamlines.size() )
			linenum = 0;
	}
}

void CMiscellaneous::ReadChatspam( const char* fileName )
{
	m_spamlines.clear();

	std::ifstream spamfile( fileName );
	if( spamfile.good() )
	{
		std::string line;

		while( std::getline( spamfile, line ) )
			m_spamlines.push_back( line );

		linenum = 0;

		spamfile.close();

		U::PrintMessage( "%s%s%s\n", charenc( "Spam file " ), fileName, charenc( " Loaded!" ) );
	}
}

void CMiscellaneous::ChangeName( const char* name )
{
	ConVar* nameConvar = I::Cvar->FindVar( charenc( "name" ) );
	*( int* )( ( DWORD )&nameConvar->fnChangeCallback + 0xC ) = NULL;
	nameConvar->SetValue( name );
}

void CMiscellaneous::AirStuck()
{
	if( Vars.Ragebot.Enabled && G::UserCmd->buttons & IN_ATTACK )
		return;

	if( G::PressedKeys[ Vars.Misc.AirStuckKey ] )
		G::UserCmd->tick_count = 16777216;
}

void CMiscellaneous::Panic()
{
	if( !G::PressedKeys[ VK_END ] )
		return;

	I::Engine->ClientCmd_Unrestricted( charenc( "cl_mouseenable 1" ), nullptr );
	I::Engine->ClientCmd_Unrestricted( charenc( "crosshair 1" ), nullptr );

	H::ModelRender->UnHook();
	H::VPanel->UnHook();
	H::ClientMode->UnHook();
	H::Client->UnHook();
	H::Surface->UnHook();
	//H::StudioRender->UnHook();
	H::D3D9->UnHook();
	SetWindowLongPtr( G::Window, GWL_WNDPROC, ( LONG_PTR )Hooks::oldWindowProc );

    LOG_FREE;
}
