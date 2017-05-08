#include "Cheat.h"

// lazy
float m_bestfov = 360.0f;
float m_bestdist = 8192.f;
float m_bestthreat = 0.f;
Vector m_besthitbox = Vector( 0, 0, 0 );
CBaseEntity* m_bestent = nullptr;
bool m_target = false;

void CRageBot::Run()
{
	DropTarget();

	// AA fixmove
	CFixMove fixMove;
	fixMove.Start();

	if( Vars.Ragebot.Antiaim.Enabled )
	{
		G::InAntiAim = true;
		AntiAim->Run();
	}
	else
	{
		G::InAntiAim = false;
		G::SendPacket = true;
	}

	fixMove.End();

	G::Aimbotting = false;

	if( !G::LocalPlayer->GetWeapon()->IsGun() || G::LocalPlayer->GetWeapon()->IsEmpty() )
		return;

	if( G::BestTarget == -1 )
		FindTarget();

	if( Vars.Ragebot.Hold && !G::PressedKeys[ Vars.Ragebot.HoldKey ] )
		return;

	if( G::BestTarget != -1 && !G::LocalPlayer->GetWeapon()->IsReloading() && m_target )
		GoToTarget();
}

void CRageBot::FindTarget()
{
	m_bestfov = Vars.Ragebot.FOV;
	m_bestdist = 8192.f;
	m_bestthreat = 0.f;
	m_besthitbox = Vector( 0, 0, 0 );
	m_target = false;
	m_bestent = nullptr;

	for( int i = 0; i <= I::Globals->maxClients; i++ )
	{
		if( !EntityIsValid( i ) )
			continue;

		CBaseEntity* Entity = I::ClientEntList->GetClientEntity( i );
		Vector hitbox = Entity->GetBonePosition( Vars.Ragebot.Hitbox );

		if( Vars.Ragebot.TargetMethod == 0 )
		{
			float fov = M::GetFov( G::UserCmd->viewangles, M::CalcAngle( G::LocalPlayer->GetEyePosition(), hitbox ) );
			if( fov < m_bestfov )
			{
				m_bestfov = fov;
				G::BestTarget = i;
				m_bestent = Entity;
				if( Vars.Ragebot.HitScanAll )
				{
					if( BestAimPointAll( Entity, m_besthitbox ) )
						m_target = true;
				}
				else if( Vars.Ragebot.HitScanHitbox )
				{
					if( BestAimPointHitbox( Entity, m_besthitbox ) )
						m_target = true;
				}
				else
				{
					m_besthitbox = hitbox;
					m_target = true;
				}
			}
		}

		if( Vars.Ragebot.TargetMethod == 1 )
		{
			float fov = M::GetFov( G::UserCmd->viewangles, M::CalcAngle( G::LocalPlayer->GetEyePosition(), hitbox ) );
			float dist = M::VectorDistance( G::LocalPlayer->GetOrigin(), Entity->GetOrigin() );
			if( dist < m_bestdist && fov < m_bestfov )
			{
				m_bestdist = dist;
				G::BestTarget = i;
				m_bestent = Entity;
				if( Vars.Ragebot.HitScanAll )
				{
					if( BestAimPointAll( Entity, m_besthitbox ) )
						m_target = true;
				}
				else if( Vars.Ragebot.HitScanHitbox )
				{
					if( BestAimPointHitbox( Entity, m_besthitbox ) )
						m_target = true;
				}
				else
				{
					m_besthitbox = hitbox;
					m_target = true;
				}
			}
		}

		// threat target method spreads the damage evenally across all players that you're able to do damage to
		// if a target is aiming at you they're are prioritised
		if( Vars.Ragebot.TargetMethod == 2 )
		{
			float fov = M::GetFov( G::UserCmd->viewangles, M::CalcAngle( G::LocalPlayer->GetEyePosition(), hitbox ) );
			float dist = M::VectorDistance( G::LocalPlayer->GetOrigin(), Entity->GetOrigin() );
			float health = ( float )Entity->GetHealth();

			float threat = health / dist;

			if( Entity->IsTargetingLocal() )
				threat += 100;

			if( threat > m_bestthreat && fov < m_bestfov )
			{
				m_bestthreat = threat;
				G::BestTarget = i;
				m_bestent = Entity;
				if( Vars.Ragebot.HitScanAll )
				{
					if( BestAimPointAll( Entity, m_besthitbox ) )
						m_target = true;
				}
				else if( Vars.Ragebot.HitScanHitbox )
				{
					if( BestAimPointHitbox( Entity, m_besthitbox ) )
						m_target = true;
				}
				else
				{
					m_besthitbox = hitbox;
					m_target = true;
				}
			}
		}
	}
}

void CRageBot::GoToTarget()
{
	bool auto_shoot = false;
	bool can_shoot = true;
	bool reloading = false;

	auto weapon = G::LocalPlayer->GetWeapon();
	float server_time = G::LocalPlayer->GetTickBase() * I::Globals->interval_per_tick;

	float next_shot = weapon->GetNextPrimaryAttack() - server_time;
	if( next_shot > 0 )
		can_shoot = false;

	// just simple velocity prediction, not engine prediction
	m_besthitbox = m_bestent->GetPredicted( m_besthitbox );

	QAngle aim_angle = M::CalcAngle( G::LocalPlayer->GetEyePosition(), m_besthitbox );

	aim_angle -= G::LocalPlayer->GetPunch() * 2.f;

	// fixmove for silentaim
	CFixMove fixMove;
	if( Vars.Ragebot.Silent )
		fixMove.Start();

	if( Vars.Ragebot.Aimstep && can_shoot )
	{
		G::Aimbotting = true;

		QAngle angNextAngle;
		bool bFinished = Aimstep( G::LastAngle, aim_angle, angNextAngle, Vars.Ragebot.AimstepAmount );
		G::UserCmd->viewangles = angNextAngle;
		G::LastAngle = angNextAngle;
		if( bFinished )
			auto_shoot = true;
	}
	else if( can_shoot )
	{
		G::Aimbotting = true;

		G::UserCmd->viewangles = aim_angle;
		auto_shoot = true;
	}

	if( Vars.Ragebot.Silent )
		fixMove.End();

	if( Vars.Ragebot.AutoStop )
	{
		G::UserCmd->forwardmove = 0;
		G::UserCmd->sidemove = 0;
		G::UserCmd->upmove = 0;
		G::UserCmd->buttons = 0;
	}

	if( Vars.Ragebot.AutoCrouch )
		G::UserCmd->buttons |= IN_DUCK;

	float hitchance = 75.f + ( Vars.Ragebot.HitChanceAmt / 4 );

	if( auto_shoot && can_shoot && Vars.Ragebot.AutoFire && ( !Vars.Ragebot.HitChance || ( 1.0f - G::LocalPlayer->GetWeapon()->GetAccuracyPenalty() ) * 100.f >= hitchance ) )
		G::UserCmd->buttons |= IN_ATTACK;
}

void CRageBot::DropTarget()
{
	if( !EntityIsValid( G::BestTarget ) || Vars.Ragebot.HitScanAll || Vars.Ragebot.HitScanHitbox )
		G::BestTarget = -1;
}

bool CRageBot::Aimstep( QAngle src, QAngle dst, QAngle& out, int steps )
{
	QAngle delta_angle = ( dst - src ).Normalized();;
	bool x_finished = false;
	bool y_finished = false;

	if( delta_angle.x > steps )
		src.x += steps;

	else if( delta_angle.x < -steps )
		src.x -= steps;

	else
	{
		x_finished = true;
		src.x = dst.x;
	}

	if( delta_angle.y > steps )
		src.y += steps;

	else if( delta_angle.y < -steps )
		src.y -= steps;

	else
	{
		y_finished = true;
		src.y = dst.y;
	}

	src.Normalized();

	out = src;

	return x_finished && y_finished;
}

bool CRageBot::GetHitbox( CBaseEntity* target, Hitbox* hitbox )
{
	matrix3x4a_t matrix[ MAXSTUDIOBONES ];

	if( !target->SetupBones( matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0 ) )
		return false;

	studiohdr_t* hdr = I::ModelInfo->GetStudioModel( target->GetModel() );

	if( !hdr )
		return false;

	mstudiohitboxset_t* hitboxSet = hdr->pHitboxSet( target->GetHitboxSet() );
	mstudiobbox_t* untransformedBox = hitboxSet->pHitbox( hitbox->hitbox );

	Vector bbmin = untransformedBox->bbmin;
	Vector bbmax = untransformedBox->bbmax;

	if( untransformedBox->radius != -1.f )
	{
		bbmin -= Vector( untransformedBox->radius, untransformedBox->radius, untransformedBox->radius );
		bbmax += Vector( untransformedBox->radius, untransformedBox->radius, untransformedBox->radius );
	}

	Vector points[] = { ( ( bbmin + bbmax ) * .5f ),
		Vector( bbmin.x, bbmin.y, bbmin.z ),
		Vector( bbmin.x, bbmax.y, bbmin.z ),
		Vector( bbmax.x, bbmax.y, bbmin.z ),
		Vector( bbmax.x, bbmin.y, bbmin.z ),
		Vector( bbmax.x, bbmax.y, bbmax.z ),
		Vector( bbmin.x, bbmax.y, bbmax.z ),
		Vector( bbmin.x, bbmin.y, bbmax.z ),
		Vector( bbmax.x, bbmin.y, bbmax.z ) };

	for( int index = 0; index <= 8; ++index )
	{
		if( index != 0 )
			points[ index ] = ( ( ( ( points[ index ] + points[ 0 ] ) * .5f ) + points[ index ] ) * .5f );

		M::VectorTransform( points[ index ], matrix[ untransformedBox->bone ], hitbox->points[ index ] );
	}

	return true;
}

bool CRageBot::GetBestPoint( CBaseEntity* target, Hitbox* hitbox, BestPoint* point )
{
	Vector center = hitbox->points[ 0 ];

	if( hitbox->hitbox == HITBOX_HEAD )
	{
		Vector high = ( ( hitbox->points[ 3 ] + hitbox->points[ 5 ] ) * .5f );

		float pitch = target->GetEyeAngles().x;
		if( ( pitch > 0.f ) && ( pitch <= 89.f ) )
		{
			Vector height = ( ( ( high - hitbox->points[ 0 ] ) / 3 ) * 4 );
			Vector new_height = ( hitbox->points[ 0 ] + ( height * ( pitch / 89.f ) ) );

			hitbox->points[ 0 ] = new_height;
			point->flags |= FL_HIGH;
		}
		else if( ( pitch < 292.5f ) && ( pitch >= 271.f ) )
		{
			hitbox->points[ 0 ] -= Vector( 0.f, 0.f, 1.f );
			point->flags |= FL_LOW;
		}
	}

	for( int index = 0; index <= 8; ++index )
	{
		int temp_damage = AutoWall->GetDamage( hitbox->points[ index ] );

		if( ( point->dmg < temp_damage ) )
		{
			point->dmg = temp_damage;
			point->point = hitbox->points[ index ];
			point->index = index;
		}
	}

	return ( point->dmg > Vars.Ragebot.AutoWallDmg );
}

int hitboxes[7] =
{
	HITBOX_HEAD,
	HITBOX_CHEST,
	HITBOX_PELVIS,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF
};

// goes through the above hitboxes and selects the best point on the target that will deal the most damage
bool CRageBot::BestAimPointAll( CBaseEntity* target, Vector& hitbox )
{
	BestPoint aim_point;

	for( int i = 0; i < 7; i++ )
	{
		Hitbox hitbox( hitboxes[ i ] );
		GetHitbox( target, &hitbox );

		if( !GetBestPoint( target, &hitbox, &aim_point ) )
			continue;
	}

	if( aim_point.dmg > Vars.Ragebot.AutoWallDmg )
	{
		hitbox = aim_point.point;
		return true;
	}

	return false;
}

// checks multiple points inside the selected hitbox and picks the best point which will deal the most damage
bool CRageBot::BestAimPointHitbox( CBaseEntity* target, Vector& hitbox )
{
	BestPoint aim_point;

	int hitboxnum = 0;

	switch( Vars.Ragebot.Hitbox )
	{
		case 0:
			hitboxnum = 3;
			break;
		case 1:
			hitboxnum = 6;
			break;
		case 2:
		case 3:
			hitboxnum = 4;
			break;
		case 4:
			hitboxnum = 7;
			break;
		case 5:
			hitboxnum = 1;
			break;
		case 6:
			hitboxnum = 0;
			break;
		default:
			hitboxnum = 0;
			break;
	}

	Hitbox hitboxx( hitboxnum );
	GetHitbox( target, &hitboxx );

	if( !GetBestPoint( target, &hitboxx, &aim_point ) )
		return false;

	if( aim_point.dmg > Vars.Ragebot.AutoWallDmg )
	{
		hitbox = aim_point.point;
		return true;
	}

	return false;
}

bool CRageBot::EntityIsValid( int i )
{
	CBaseEntity* Entity = I::ClientEntList->GetClientEntity( i );

	if( !Entity )
		return false;
	if( Entity == G::LocalPlayer )
		return false;

	if( !Vars.Ragebot.FriendlyFire )
	{
		if( Entity->GetTeam() == G::LocalPlayer->GetTeam() )
			return false;
	}

	if( Entity->GetDormant() )
		return false;
	if( Entity->GetImmune() )
		return false;
	if( Entity->GetHealth() <= 0 )
		return false;

	if( !Vars.Ragebot.HitScanAll && !Vars.Ragebot.HitScanHitbox )
	{
		if( Vars.Ragebot.AutoWallDmg > AutoWall->GetDamage( Entity->GetBonePosition( Vars.Ragebot.Hitbox ) ) )
			return false;
	}

	return true;
}

void CRageBot::CFixMove::Start()
{
	m_oldangle = G::UserCmd->viewangles;
	m_oldforward = G::UserCmd->forwardmove;
	m_oldsidemove = G::UserCmd->sidemove;
}

void CRageBot::CFixMove::End()
{
	float yaw_delta = G::UserCmd->viewangles.y - m_oldangle.y;
	float f1;
	float f2;

	if( m_oldangle.y < 0.f )
		f1 = 360.0f + m_oldangle.y;
	else
		f1 = m_oldangle.y;

	if( G::UserCmd->viewangles.y < 0.0f )
		f2 = 360.0f + G::UserCmd->viewangles.y;
	else
		f2 = G::UserCmd->viewangles.y;

	if( f2 < f1 )
		yaw_delta = abs( f2 - f1 );
	else
		yaw_delta = 360.0f - abs( f1 - f2 );
	yaw_delta = 360.0f - yaw_delta;

	G::UserCmd->forwardmove = cos( DEG2RAD( yaw_delta ) ) * m_oldforward + cos( DEG2RAD( yaw_delta + 90.f ) ) * m_oldsidemove;
	G::UserCmd->sidemove = sin( DEG2RAD( yaw_delta ) ) * m_oldforward + sin( DEG2RAD( yaw_delta + 90.f ) ) * m_oldsidemove;
}
