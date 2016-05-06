#include "Cheat.h"
/*
AntiAimPitch:
 0 == SMAC/KAC Safe
 1 == Normal
 2 == Static

AntiAimYaw:
 0 == Normal
 1 == Jitter
 2 == Spinhack
 3 == Fake Sideways
 4 == Fake Static
*/

CAntiAim* AntiAim = new CAntiAim;

QAngle temp;

void CAntiAim::Run()
{
	if( !G::LocalPlayer || !G::LocalPlayer->GetWeapon() )
		return;

	if( G::LocalPlayer->GetMoveType() == MOVETYPE_LADDER || G::LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP )
		return;

	static bool flip = false;
	flip = !flip;

	temp = G::UserCmd->viewangles;

	if( Vars.Ragebot.Antiaim.Pitch != AA_PITCH_STATIC )
	{
		if( ( !G::SendPacket && Vars.Ragebot.Antiaim.PSilent ) || !Vars.Ragebot.Antiaim.PSilent )
		{
			if( G::UserCmd->buttons & IN_ATTACK )
				return;
		}

		antiAimYaw();

		if( Vars.Ragebot.Antiaim.Pitch == AA_PITCH_SMAC )
		{
			G::UserCmd->viewangles.x = 89.00f;
		}

		if( Vars.Ragebot.Antiaim.Pitch == AA_PITCH_NORMAL )
		{
			G::UserCmd->viewangles.x = 180.00f;
		}

		if( Vars.Ragebot.Antiaim.Pitch == AA_PITCH_UP )
		{
			G::UserCmd->viewangles.x = -271.f;
		}

		if( Vars.Ragebot.Antiaim.Pitch == AA_PITCH_FAKE_UP )
		{
			if( flip )
			{
				G::UserCmd->viewangles.x = -271.f;
				G::SendPacket = false;
			}
			else
			{
				G::UserCmd->viewangles.x = 271.f;
				G::SendPacket = true;
			}
		}
	}

	if( Vars.Ragebot.Antiaim.Pitch == AA_PITCH_STATIC )
	{
		if( G::UserCmd->buttons & IN_ATTACK )
		{
			if( Vars.Ragebot.Antiaim.PSilent && Vars.Ragebot.Enabled )
			{
				if( G::SendPacket )
				{
					antiAimYaw();
					G::UserCmd->viewangles.x = 180.0f;
				}
			}
			else
			{
				QAngle punch_angle = G::LocalPlayer->GetPunch();

				QAngle recoil = punch_angle * 2;

				G::UserCmd->viewangles.x = ( 180.0f - temp.x ) - recoil.x;
				G::UserCmd->viewangles.y = -( ( 180.0f - temp.y ) - ( -recoil.y ) );
			}
		}
		else
		{
			antiAimYaw();
			G::UserCmd->viewangles.x = 180.0f;
		}
	}

	if( Vars.Ragebot.Aimstep && !G::Aimbotting )
	{
		QAngle angNextAngle;
		bool bFinished = E::RageBot->Aimstep( G::LastAngle, temp, angNextAngle, Vars.Ragebot.AimstepAmount );
		G::UserCmd->viewangles.y = angNextAngle.y;
		G::LastAngle = angNextAngle;
	}
	else if( !G::Aimbotting )
	{
		G::UserCmd->viewangles.y = temp.y;
	}
}

void CAntiAim::antiAimYaw()
{
	float server_time = G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;

	static bool flip;
	static bool flip2;
	flip = !flip;
	flip2 = !flip2;

	
	if( Vars.Ragebot.Antiaim.AimAtPlayer )
		aimAtPlayer();
	
	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_NORMAL )
	{
		G::SendPacket = true;
		temp.y += 180.0f;
	}
	
	

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_JITTER )
	{
		G::SendPacket = true;

		if( !Vars.Ragebot.Antiaim.AimAtPlayer )
		{
			if( flip )
				temp.y = 270.0f;
			
			else
				temp.y = 90.0f;
		}
		else
		{
			if( flip )
				temp.y -= 270.0f;

			else
				temp.y -= 90.0f;
		}
	}

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_SPINHACK )
	{
		G::SendPacket = true;
		temp.y = ( float )( fmod( server_time / 1.5f * 360.0f, 360.0f ) );
	}

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_SIDEWAYS )
	{
		if( flip )
			temp.y += 90.0f;
		else
			temp.y -= 90.0f;
	}

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_FAKE_SIDEWAYS )
	{
		if( flip )
		{
			temp.y += 90.0f;
			G::SendPacket = false;
		}
		else
		{
			temp.y -= 90.0f;
			G::SendPacket = true;
		}
	}

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_STATIC )
	{
		if( flip )
		{
			if( flip2 )
				temp.y += 90.0f;

			else
				temp.y -= 90.0f;
		}
		else
		{
			temp.y += 180.0f;
		}
	}

	if( Vars.Ragebot.Antiaim.Yaw == AA_YAW_FAKE_STATIC )
	{
		if( flip )
		{
			G::SendPacket = false;

			if( flip2 )
				temp.y += 90.0f;

			else
				temp.y -= 90.0f;
		}
		else
		{
			G::SendPacket = true;
			temp.y += 180.0f;
		}
	}
}

void CAntiAim::aimAtPlayer()
{
	if( !G::LocalPlayer || !G::LocalPlayer->GetWeapon() || !G::LocalPlayer->GetWeapon()->GetCSWpnData() )
		return;

	Vector eye_position = G::LocalPlayer->GetEyePosition();

	float best_dist = G::LocalPlayer->GetWeapon()->GetCSWpnData()->m_flRange;

	CBaseEntity* target = I::ClientEntList->GetClientEntity( G::BestTarget );

	if( target == NULL )
		return;

	if( target == G::LocalPlayer )
		return;

	if( ( target->GetTeam() == G::LocalPlayer->GetTeam() ) || target->GetDormant() || !target->GetAlive() || target->GetHealth() <= 0 )
		return;

	Vector target_position = target->GetEyePosition();

	float temp_dist = eye_position.Dist( target_position );

	if( best_dist > temp_dist )
	{
		best_dist = temp_dist;
		temp = M::CalcAngle( eye_position, target_position );
	}
}