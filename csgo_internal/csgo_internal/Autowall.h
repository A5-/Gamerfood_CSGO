#pragma once

struct FireBulletData
{
	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

class CAutowall
{
public:
	float GetDamage( const Vector& vecPoint );

	bool SimulateFireBullet( CBaseCombatWeapon* pWeapon, FireBulletData& data );
	bool HandleBulletPenetration( WeaponInfo_t* wpn_data, FireBulletData& data );
	bool TraceToExit( Vector& end, trace_t& tr, float x, float y, float z, float x2, float y2, float z2, trace_t* trace );
	bool DidHitNonWorldEntity( CBaseEntity* m_pEnt );
	void ScaleDamage( int hitgroup, CBaseEntity* enemy, float weapon_armor_ratio, float& current_damage );
	float GetHitgroupDamageMultiplier( int iHitGroup );
};

extern CAutowall* AutoWall;
