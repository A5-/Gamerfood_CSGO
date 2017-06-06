#include "Cheat.h"

CreateMoveFn oCreateMove;

bool __stdcall Hooks::CreateMove( float flInputSampleTime, CUserCmd* cmd )
{
	G::LocalPlayer = U::GetLocalPlayer();
	G::UserCmd = cmd;

	if( Vars.Ragebot.Enabled )
		Vars.Legitbot.Aimbot.Enabled = false;
	else if( Vars.Legitbot.Aimbot.Enabled )
		Vars.Ragebot.Enabled = false;

	if( cmd->command_number == 0 )
		return false;

	DWORD* framePointer;
	__asm mov framePointer, ebp;
	*( bool* )( *framePointer - 0x1C ) = G::SendPacket;

	if( !G::Aimbotting || !G::InAntiAim )
		G::LastAngle = cmd->viewangles;

	E::LegitBot->Run();

	if( Vars.Misc.Bhop )
		E::Misc->Bunnyhop();

	if( Vars.Misc.AutoStrafe )
		E::Misc->AutoStrafe();

	if( Vars.Ragebot.Enabled )
		E::RageBot->Run();
	
	if( Vars.Misc.AirStuck )
		E::Misc->AirStuck();

	if( Vars.Ragebot.UntrustedCheck )
		cmd->viewangles.Clamp();

	if( Vars.Visuals.Removals.VisualRecoil && Vars.Legitbot.Aimbot.RCS && Vars.Legitbot.Aimbot.Enabled )
		return false;

	if (Vars.Ragebot.Enabled && Vars.Ragebot.Silent)
		return false;

	I::Engine->SetViewAngles( G::UserCmd->viewangles );
	
	return false;
}
