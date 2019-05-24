#include "Cheat.h"

CLegitBot* LegitBot;

float m_fbestfov = 20.0f;
int m_ibesttargetlegit = -1;

float deltaTime;
float curAimTime;

void CLegitBot::Run()
{
	DropTarget();

	static float oldServerTime = G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;
	float serverTime = G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;
	deltaTime = serverTime - oldServerTime;
	oldServerTime = serverTime;

	if( !G::LocalPlayer->GetWeapon()->IsGun() || G::LocalPlayer->GetWeapon()->IsEmpty() )
		return;

	if( ( G::PressedKeys[ Vars.Legitbot.Triggerbot.Key ] || Vars.Legitbot.Triggerbot.AutoFire ) && Vars.Legitbot.Triggerbot.Enabled )
		Triggerbot();

	if( ( !( G::UserCmd->buttons & IN_ATTACK ) || m_ibesttargetlegit == -1 ) && Vars.Legitbot.Aimbot.Enabled )
		FindTarget();

	if( m_ibesttargetlegit != -1 && ( Vars.Legitbot.Aimbot.AlwaysOn ) || ( Vars.Legitbot.Aimbot.OnKey && G::PressedKeys[ Vars.Legitbot.Aimbot.Key ] ) )
		GoToTarget();

	if( !Vars.Visuals.Removals.VisualRecoil && Vars.Legitbot.Aimbot.RCS && Vars.Legitbot.Aimbot.Enabled )
		RCS();

	if( Vars.Visuals.Removals.VisualRecoil && Vars.Legitbot.Aimbot.RCS && Vars.Legitbot.Aimbot.Enabled )
	{
		G::UserCmd->viewangles -= G::LocalPlayer->GetPunch() * 2.f;
	}
}

void CLegitBot::FindTarget()
{
	m_fbestfov = Vars.Legitbot.Aimbot.FOV;

	for( int i = 1; i <= I::Globals->maxClients; i++ )
	{
		if( !EntityIsValid( i ) )
			continue;

		CBaseEntity* Entity = I::ClientEntList->GetClientEntity( i );

		if( !Entity )
			continue;

		QAngle viewangles = G::UserCmd->viewangles + G::LocalPlayer->GetPunch() * 2.f;

		float fov = M::GetFov( G::UserCmd->viewangles, M::CalcAngle( G::LocalPlayer->GetEyePosition(), Entity->GetBonePosition( Vars.Legitbot.Aimbot.Hitbox ) ) );
		if( fov < m_fbestfov )
		{
			m_fbestfov = fov;
			m_ibesttargetlegit = i;
		}
	}
}

void CLegitBot::GoToTarget()
{
	CBaseEntity* Entity = I::ClientEntList->GetClientEntity( m_ibesttargetlegit );

	if( !Entity )
		return;

	Vector predicted = Entity->GetPredicted( Entity->GetBonePosition( Vars.Legitbot.Aimbot.Hitbox ) );

	QAngle dst = M::CalcAngle( G::LocalPlayer->GetEyePosition(), predicted );
	QAngle src = G::UserCmd->viewangles;

	dst -= G::LocalPlayer->GetPunch() * ( Vars.Legitbot.Aimbot.RCSAmount / 50.f );

	QAngle delta = dst - src;

	delta.Clamp();

	if( !delta.IsZero() )
	{
		float finalTime = delta.Length() / ( Vars.Legitbot.Aimbot.Speed / 10 );

		curAimTime += deltaTime;

		if( curAimTime > finalTime )
			curAimTime = finalTime;

		float percent = curAimTime / finalTime;

		delta *= percent;
		dst = src + delta;
	}

	G::UserCmd->viewangles = dst.Clamp();
}

void CLegitBot::DropTarget()
{
	if( !EntityIsValid( m_ibesttargetlegit ) )
	{
		m_ibesttargetlegit = -1;
		curAimTime = 0.f;
	}
}

bool CLegitBot::EntityIsValid( int i )
{
	CBaseEntity* pEntity = I::ClientEntList->GetClientEntity( i );

	if( !pEntity )
		return false;
	if( pEntity == G::LocalPlayer )
		return false;
	if( pEntity->GetHealth() <= 0 )
		return false;
	if( pEntity->GetImmune() )
		return false;
	if( pEntity->GetDormant() )
		return false;
	if( pEntity->GetTeam() == G::LocalPlayer->GetTeam() && !Vars.Legitbot.Aimbot.FriendlyFire )
		return false;
	if( !pEntity->IsVisible( Vars.Legitbot.Aimbot.Hitbox ) )
		return false;
	if( M::GetFov( G::UserCmd->viewangles, M::CalcAngle( G::LocalPlayer->GetEyePosition(), pEntity->GetBonePosition( Vars.Legitbot.Aimbot.Hitbox ) ) ) > Vars.Legitbot.Aimbot.FOV )
		return false;

	return true;
}

void CLegitBot::Triggerbot()
{
	Vector src, dst, forward;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	QAngle viewangle = G::UserCmd->viewangles;

	viewangle += G::LocalPlayer->GetPunch() * 2.f;

	M::AngleVectors( viewangle, &forward );
	forward *= G::LocalPlayer->GetWeapon()->GetCSWpnData()->flRange;
	filter.pSkip = G::LocalPlayer;
	src = G::LocalPlayer->GetEyePosition();
	dst = src + forward;

	ray.Init( src, dst );

	I::EngineTrace->TraceRay( ray, 0x46004003, &filter, &tr );

	if( !tr.m_pEnt )
		return;

	int hitgroup = tr.hitgroup;
	bool didHit = false;

	if( Vars.Legitbot.Triggerbot.Filter.Head && hitgroup == HITGROUP_HEAD )
		didHit = true;
	if( Vars.Legitbot.Triggerbot.Filter.Chest && hitgroup == HITGROUP_CHEST )
		didHit = true;
	if( Vars.Legitbot.Triggerbot.Filter.Stomach && hitgroup == HITGROUP_STOMACH )
		didHit = true;
	if( Vars.Legitbot.Triggerbot.Filter.Arms && ( hitgroup == HITGROUP_LEFTARM || hitgroup == HITGROUP_RIGHTARM ) )
		didHit = true;
	if( Vars.Legitbot.Triggerbot.Filter.Legs && ( hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG ) )
		didHit = true;

	if( ( Vars.Legitbot.Triggerbot.Filter.Friendly || tr.m_pEnt->GetTeam() != G::LocalPlayer->GetTeam() ) && tr.m_pEnt->GetHealth() > 0 && !tr.m_pEnt->GetImmune() )
	{
		float hitchance = 75.f + ( Vars.Legitbot.Triggerbot.HitChanceAmt / 4 );
		if( didHit && ( !Vars.Legitbot.Triggerbot.HitChance || ( 1.0f - G::LocalPlayer->GetWeapon()->GetAccuracyPenalty() ) * 100.f >= hitchance ) )
			G::UserCmd->buttons |= IN_ATTACK;
	}
}

QAngle m_oldangle;

void CLegitBot::RCS()
{
	if( G::UserCmd->buttons & IN_ATTACK )
	{
		QAngle aimpunch = G::LocalPlayer->GetPunch() * 2.f;

		G::UserCmd->viewangles += ( m_oldangle - aimpunch );

		m_oldangle = aimpunch;
	}
	else
	{
		m_oldangle.x = m_oldangle.y = m_oldangle.z = 0;
	}
}
