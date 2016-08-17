#include "Cheat.h"

CAutowall* AutoWall = new CAutowall;

float CAutowall::GetHitgroupDamageMultiplier( int iHitGroup )
{
	switch( iHitGroup )
	{
	case HITGROUP_HEAD:
	{
		return 4.0f;
	}
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
	{
		return 1.0f;
	}
	case HITGROUP_STOMACH:
	{
		return 1.25f;
	}
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
	{
		return 0.75f;
	}
	}
	return 1.0f;
}

void CAutowall::ScaleDamage( int hitgroup, CBaseEntity *enemy, float weapon_armor_ratio, float &current_damage )
{
	current_damage *= GetHitgroupDamageMultiplier( hitgroup );

	int armor = enemy->GetArmor();
	int helmet = enemy->HasHelmet();

	if( armor > 0 )
	{
		if( hitgroup == HITGROUP_HEAD )
		{
			if( helmet )
				current_damage *= ( weapon_armor_ratio * .5f );
		}
		else
		{
			current_damage *= ( weapon_armor_ratio * .5f );
		}
	}
}

bool CAutowall::DidHitNonWorldEntity( CBaseEntity* m_pEnt )
{
	return m_pEnt != NULL && m_pEnt == I::ClientEntList->GetClientEntity( 0 );
}

bool CAutowall::TraceToExit(Vector& end, trace_t& tr, float x, float y, float z, float x2, float y2, float z2, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static TraceToExitFn TraceToExit = (TraceToExitFn)U::FindPattern("client.dll", "55 8B EC 83 EC 2C F3 0F 10 75 ? 33 C0");

	if (!TraceToExit)
	{
		return false;
		U::PrintMessage("TraceToExit not found\n");
	}
	_asm
	{
		push trace
		push z2
		push y2
		push x2
		push z
		push y
		push x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

bool CAutowall::HandleBulletPenetration( WeaponInfo_t *wpn_data, FireBulletData &data )
{
	surfacedata_t *enter_surface_data = I::Physprops->GetSurfaceData( data.enter_trace.surface.surfaceProps );
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = *( float* )( ( DWORD )enter_surface_data + 76 );

	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow( ( wpn_data->m_flRangeModifier ), ( data.trace_length * 0.002 ) );

	if( ( data.trace_length > 3000.f ) || ( enter_surf_penetration_mod < 0.1f ) )
		data.penetrate_count = 0;

	if( data.penetrate_count <= 0 )
		return false;

	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos.x, data.enter_trace.endpos.y, data.enter_trace.endpos.z, data.direction.x, data.direction.y, data.direction.z, &trace_exit))
		return false;




	surfacedata_t *exit_surface_data = I::Physprops->GetSurfaceData( trace_exit.surface.surfaceProps );
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = *( float* )( ( DWORD )exit_surface_data + 76 );
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if( ( ( data.enter_trace.contents & CONTENTS_GRATE ) != 0 ) || ( enter_material == 89 ) || ( enter_material == 71 ) )
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = ( enter_surf_penetration_mod + exit_surf_penetration_mod ) * 0.5f;
	}

	if( enter_material == exit_material )
	{
		if( exit_material == 87 || exit_material == 85 )
			combined_penetration_modifier = 3.0f;
		else if( exit_material == 76 )
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf( 0.f, 1.0f / combined_penetration_modifier );
	float v35 = ( data.current_damage * final_damage_modifier ) + v34 * 3.0f * fmaxf( 0.0f, ( 3.0f / wpn_data->m_flPenetration ) * 1.25f );
	float thickness = ( trace_exit.endpos - data.enter_trace.endpos ).Length();

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf( 0.0f, v35 + thickness );

	if( lost_damage > data.current_damage )
		return false;

	if( lost_damage >= 0.0f )
		data.current_damage -= lost_damage;

	if( data.current_damage < 1.0f )
		return false;

	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool CAutowall::SimulateFireBullet( CBaseCombatWeapon* pWeapon, FireBulletData &data )
{
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	WeaponInfo_t* weaponData = G::LocalPlayer->GetWeapon()->GetCSWpnData();

	if( weaponData == NULL )
		return false;

	data.current_damage = ( float )weaponData->m_iDamage;

	while( ( data.penetrate_count > 0 ) && ( data.current_damage >= 1.0f ) )
	{
		data.trace_length_remaining = weaponData->m_flRange - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		//data.enter_trace
		U::TraceLine( data.src, end, MASK_SHOT, G::LocalPlayer, &data.enter_trace );

		Ray_t ray;
		ray.Init( data.src, end + data.direction*40.f );

		I::EngineTrace->TraceRay( ray, MASK_SHOT, &data.filter, &data.enter_trace );

		U::TraceLine( data.src, end + data.direction*40.f, MASK_SHOT, G::LocalPlayer, &data.enter_trace );

		if( data.enter_trace.fraction == 1.0f )
			break;

		if( ( data.enter_trace.hitgroup <= 7 ) && ( data.enter_trace.hitgroup > 0 ) )
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow( weaponData->m_flRangeModifier, data.trace_length * 0.002 );
			ScaleDamage( data.enter_trace.hitgroup, data.enter_trace.m_pEnt, weaponData->m_flArmorRatio, data.current_damage );

			return true;
		}

		if( !HandleBulletPenetration( weaponData, data ) )
			break;
	}

	return false;
}

float CAutowall::GetDamage( const Vector& point )
{
	float damage = 0.f;
	Vector dst = point;
	FireBulletData data;
	data.src = G::LocalPlayer->GetEyePosition();
	data.filter.pSkip = G::LocalPlayer;

	QAngle angles = M::CalcAngle( data.src, dst );
	M::AngleVectors( angles, &data.direction );
	Vector dataNormalized;

	data.direction.Normalize();

	CBaseCombatWeapon* pWeap = G::LocalPlayer->GetWeapon();
	if( SimulateFireBullet( pWeap, data ) )
	{
		damage = data.current_damage;
	}

	return damage;
}