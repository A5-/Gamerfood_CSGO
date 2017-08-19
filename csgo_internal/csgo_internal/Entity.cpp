#include "SDK.h"

int CBaseEntity::GetHealth()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iHealth );
}

int CBaseEntity::GetTeam()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iTeamNum );
}

int CBaseEntity::GetFlags()
{
	return *( int* ) ( ( DWORD )this + offsets.m_fFlags );
}

int CBaseEntity::GetTickBase()
{
	return *( int* ) ( ( DWORD )this + offsets.m_nTickBase );
}

int CBaseEntity::GetShotsFired()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iShotsFired );
}

int CBaseEntity::GetMoveType()
{
	return *( int* ) ( ( DWORD )this + offsets.m_nMoveType );
}

int CBaseEntity::GetModelIndex()
{
	return *( int* )( ( DWORD )this + offsets.m_nModelIndex );
}

int CBaseEntity::GetHitboxSet()
{
	return *( int* )( ( DWORD )this + offsets.m_nHitboxSet );
}

int CBaseEntity::GetUserID()
{
	return this->GetPlayerInfo().userid;
}

int CBaseEntity::GetArmor()
{
	return *( int* )( ( DWORD )this + offsets.m_ArmorValue );
}

int CBaseEntity::PhysicsSolidMaskForEntity()
{
	typedef unsigned int ( __thiscall* OriginalFn )( void* );
	return U::GetVFunc< OriginalFn >( this, 154 )( this );
}

int CBaseEntity::GetOwner()
{
	return *( int* )( ( DWORD )this + offsets.m_hOwnerEntity );
}

int CBaseEntity::GetGlowIndex()
{
	return *( int* )( ( DWORD )this + offsets.m_iGlowIndex );
}

float CBaseEntity::GetBombTimer()
{
	float bombTime = *( float* )( ( DWORD )this + offsets.m_flC4Blow );
	float returnValue = bombTime - I::Globals->curtime;
	return ( returnValue < 0 ) ? 0.f : returnValue;
}

float CBaseEntity::GetFlashDuration()
{
	return *( float* )( ( DWORD )this + offsets.m_flFlashDuration );
}

bool CBaseEntity::IsFlashed()
{
	return GetFlashDuration() > 0 ? true : false;
}

bool CBaseEntity::GetAlive()
{
	return ( bool ) ( *( int* ) ( ( DWORD )this + offsets.m_lifeState ) == 0 );
}

bool CBaseEntity::GetDormant()
{
	return *( bool* ) ( ( DWORD )this + offsets.m_bDormant );
}

bool CBaseEntity::GetImmune()
{
	return *( bool* ) ( ( DWORD )this + offsets.m_bGunGameImmunity );
}

bool CBaseEntity::HasHelmet()
{
	return *( bool* )( ( DWORD )this + offsets.m_bHasHelmet );
}

bool CBaseEntity::IsTargetingLocal()
{
	Vector src, dst, forward;
	trace_t tr;

	if( !this )
		return false;

	QAngle viewangle = this->GetEyeAngles();

	M::AngleVectors( viewangle, &forward );
	forward *= 8142.f;
	src = this->GetEyePosition();
	dst = src + forward;

	U::TraceLine( src, dst, MASK_SHOT, this, &tr );

	if( tr.m_pEnt == G::LocalPlayer )
		return true;

	return false;
}

model_t* CBaseEntity::GetModel()
{
	return *( model_t** )( ( DWORD )this + 0x6C );
}

bool CBaseEntity::IsEnemy()
{
	return ( this->GetTeam() == G::LocalPlayer->GetTeam() || this->GetTeam() == 0 ) ? false : true;
}

bool CBaseEntity::IsVisible( int bone )
{
	Ray_t ray;
	trace_t tr;
	m_visible = false;
	ray.Init( G::LocalPlayer->GetEyePosition(), this->GetBonePosition( bone ) );

	CTraceFilter filter;
	filter.pSkip = G::LocalPlayer;

	I::EngineTrace->TraceRay( ray, MASK_NPCWORLDSTATIC | CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_MONSTER | CONTENTS_WINDOW | CONTENTS_DEBRIS | CONTENTS_HITBOX, &filter, &tr );

	if( tr.m_pEnt == this || tr.fraction > 0.99f )
	{
		m_visible = true;
		return true;
	}

	return false;
}

bool CBaseEntity::IsBroken()
{
	return *( bool* ) ( ( DWORD )this + offsets.m_bIsBroken );
}

QAngle CBaseEntity::GetViewPunch()
{
	return *( QAngle* ) ( ( DWORD )this + offsets.m_viewPunchAngle );
}

QAngle CBaseEntity::GetPunch()
{
	return *( QAngle* ) ( ( DWORD )this + offsets.m_aimPunchAngle );
}

QAngle CBaseEntity::GetEyeAngles()
{
	return *( QAngle* )( ( DWORD )this + offsets.m_angEyeAngles );
}

Vector CBaseEntity::GetOrigin()
{
	return *( Vector* ) ( ( DWORD )this + offsets.m_vecOrigin );
}

Vector CBaseEntity::GetEyePosition()
{
	Vector origin = this->GetOrigin();
	Vector offset = *( Vector* )( ( DWORD )this + offsets.m_vecViewOffset );

	return ( origin + offset );
}

Vector CBaseEntity::GetBonePosition( int iBone )
{
	matrix3x4_t boneMatrixes[128];
	if( this->SetupBones( boneMatrixes, 128, 0x100, 0 ) )
	{
		matrix3x4_t boneMatrix = boneMatrixes[ iBone ];
		return Vector( boneMatrix.m_flMatVal[ 0 ][ 3 ], boneMatrix.m_flMatVal[ 1 ][ 3 ], boneMatrix.m_flMatVal[ 2 ][ 3 ] );
	}
	else
		return Vector( 0, 0, 0 );
}

bool CBaseEntity::SetupBones( matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )
{
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
}

Vector CBaseEntity::GetVelocity()
{
	return *( Vector* ) ( ( DWORD )this + offsets.m_vecVelocity );
}

Vector CBaseEntity::GetPredicted( Vector p0 )
{
	return M::ExtrapolateTick( p0, this->GetVelocity() );
}

ICollideable* CBaseEntity::GetCollideable()
{
	return ( ICollideable* ) ( ( DWORD )this + offsets.m_Collision );
}

player_info_t CBaseEntity::GetPlayerInfo()
{
	player_info_t pinfo;
	I::Engine->GetPlayerInfo( this->index, &pinfo );
	return pinfo;
}

std::string CBaseEntity::GetName()
{
	return this->GetPlayerInfo().name;
}

std::string CBaseEntity::GetSteamID()
{
	return this->GetPlayerInfo().name;
}

std::string CBaseEntity::GetLastPlace()
{
	return std::string( ( const char* )this + offsets.m_szLastPlaceName );
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	DWORD weaponData = *( DWORD* )( ( DWORD )this + offsets.m_hActiveWeapon );
	return ( CBaseCombatWeapon* ) I::ClientEntList->GetClientEntityFromHandle( weaponData );
}

ClientClass* CBaseEntity::GetClientClass()
{
	PVOID pNetworkable = ( PVOID )( ( DWORD )( this ) + 0x8 );
	typedef ClientClass*( __thiscall* OriginalFn )( PVOID );
	return U::GetVFunc< OriginalFn >( pNetworkable, 2 )( pNetworkable );
}

int CBaseEntity::GetCollisionGroup()
{
	return *( int* ) ( ( DWORD )this + offsets.m_CollisionGroup );
}

int& CBaseCombatWeapon::GetWeaponID()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iWeaponID );
}

float& CBaseCombatWeapon::GetNextPrimaryAttack()
{
	return *( float* ) ( ( DWORD )this + offsets.m_flNextPrimaryAttack );
}

float& CBaseCombatWeapon::GetAccuracyPenalty()
{
	return *( float* ) ( ( DWORD )this + offsets.m_fAccuracyPenalty );
}

int& CBaseCombatWeapon::GetXUIDLow()
{
	return *( int* ) ( ( DWORD )this + offsets.m_OriginalOwnerXuidLow );
}

int& CBaseCombatWeapon::GetXUIDHigh()
{
	return *( int* ) ( ( DWORD )this + offsets.m_OriginalOwnerXuidHigh );
}

int& CBaseCombatWeapon::GetEntityQuality()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iEntityQuality );
}

int& CBaseCombatWeapon::GetAccountID()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iAccountID );
}

int& CBaseCombatWeapon::GetItemIDHigh()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iItemIDHigh );
}

int& CBaseCombatWeapon::GetItemDefinitionIndex()
{
	return *( int* ) ( ( DWORD )this + offsets.m_iItemDefinitionIndex );
}

int& CBaseCombatWeapon::GetFallbackPaintKit()
{
	return *( int* ) ( ( DWORD )this + offsets.m_nFallbackPaintKit );
}

int& CBaseCombatWeapon::GetFallbackStatTrak()
{
	return *( int* ) ( ( DWORD )this + offsets.m_nFallbackStatTrak );
}

float& CBaseCombatWeapon::GetFallbackWear()
{
	return *( float* ) ( ( DWORD )this + offsets.m_flFallbackWear );
}

WeaponInfo_t* CBaseCombatWeapon::GetCSWpnData()
{
	if( !this ) return nullptr;
	typedef WeaponInfo_t*( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>(this, 446)(this);
	//return U::GetVFunc<OriginalFn>( this, 456 )( this ); //Wrong since update 18.08.17 
}

bool CBaseCombatWeapon::IsEmpty()
{
	int clip = *( int* )( ( DWORD )this + offsets.m_iClip1 );
	return clip == 0;
}

bool CBaseCombatWeapon::IsReloading()
{
	return *( bool* )( ( DWORD )this + 0x3235 );
}

void CBaseCombatWeapon::UpdateAccuracyPenalty()
{
	typedef void ( __thiscall *FunctionFn )( void* );
	reinterpret_cast< FunctionFn >( *reinterpret_cast< PDWORD >( *reinterpret_cast< PDWORD >( this ) + 0x748 ) )( ( void* )this );
}

float CBaseCombatWeapon::GetWeaponCone()
{
	if( !this ) return 0.f;
	typedef float ( __thiscall* OriginalFn )( void* );
	return U::GetVFunc< OriginalFn >( this, 478 )( this ); //What do we miss here?
}

float CBaseCombatWeapon::GetWeaponSpread()
{
	if( !this ) return 0.f;
	typedef float( __thiscall* OriginalFn )( void* );
	return U::GetVFunc<OriginalFn>( this, 439 )( this );
	//return U::GetVFunc<OriginalFn>( this, 479 )( this ); //Wrong since update 18.08.17 
}

bool CBaseCombatWeapon::IsGun()
{
	if( !this )
		return false;

	int id = this->GetWeaponID();
	//If your aimbot is broken, this is the reason. Just an FYI.

	switch( id )
	{
		case WEAPON_DEAGLE:
		case WEAPON_ELITE:
		case WEAPON_FIVESEVEN:
		case WEAPON_GLOCK:
		case WEAPON_AK47:
		case WEAPON_AUG:
		case WEAPON_AWP:
		case WEAPON_FAMAS:
		case WEAPON_G3SG1:
		case WEAPON_GALILAR:
		case WEAPON_M249:
		case WEAPON_M4A1:
		case WEAPON_MAC10:
		case WEAPON_P90:
		case WEAPON_UMP45:
		case WEAPON_XM1014:
		case WEAPON_BIZON:
		case WEAPON_MAG7:
		case WEAPON_NEGEV:
		case WEAPON_SAWEDOFF:
		case WEAPON_TEC9:
			return true;
		case WEAPON_TASER:
			return false;
		case WEAPON_HKP2000:
		case WEAPON_MP7:
		case WEAPON_MP9:
		case WEAPON_NOVA:
		case WEAPON_P250:
		case WEAPON_SCAR20:
		case WEAPON_SG556:
		case WEAPON_SSG08:
			return true;
		case WEAPON_KNIFE:
		case WEAPON_FLASHBANG:
		case WEAPON_HEGRENADE:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_MOLOTOV:
		case WEAPON_DECOY:
		case WEAPON_INCGRENADE:
		case WEAPON_C4:
		case WEAPON_KNIFE_T:
			return false;
		case WEAPON_M4A1_SILENCER:
		case WEAPON_USP_SILENCER:
		case WEAPON_CZ75A:
		case WEAPON_REVOLVER:
			return true;
		default:
			return false;
	}
}

std::string CBaseCombatWeapon::GetWeaponName()
{
	if( !this )
		return "";

	int id = this->GetWeaponID();

	switch( id )
	{
		case WEAPON_DEAGLE:
			return strenc( "Desert Eagle" );
		case WEAPON_ELITE:
			return strenc( "Dual Berettas" );
		case WEAPON_FIVESEVEN:
			return strenc( "Five-SeveN" );
		case WEAPON_GLOCK:
			return strenc( "Glock-18" );
		case WEAPON_AK47:
			return strenc( "AK-47" );
		case WEAPON_AUG:
			return strenc( "AUG" );
		case WEAPON_AWP:
			return strenc( "AWP" );
		case WEAPON_FAMAS:
			return strenc( "FAMAS" );
		case WEAPON_G3SG1:
			return strenc( "G3SG1" );
		case WEAPON_GALILAR:
			return strenc( "Galil" );
		case WEAPON_M249:
			return strenc( "M249" );
		case WEAPON_M4A1:
			return strenc( "M4A1" );
		case WEAPON_MAC10:
			return strenc( "MAC-10" );
		case WEAPON_P90:
			return strenc( "P90" );
		case WEAPON_UMP45:
			return strenc( "UMP-45" );
		case WEAPON_XM1014:
			return strenc( "XM1014" );
		case WEAPON_BIZON:
			return strenc( "PP-Bizon" );
		case WEAPON_MAG7:
			return strenc( "MAG-7" );
		case WEAPON_NEGEV:
			return strenc( "Negev" );
		case WEAPON_SAWEDOFF:
			return strenc( "Sawed-Off" );
		case WEAPON_TEC9:
			return strenc( "Tec-9" );
		case WEAPON_TASER:
			return strenc( "Taser" );
		case WEAPON_HKP2000:
			return strenc( "P2000" );
		case WEAPON_MP7:
			return strenc( "MP7" );
		case WEAPON_MP9:
			return strenc( "MP9" );
		case WEAPON_NOVA:
			return strenc( "Nova" );
		case WEAPON_P250:
			return strenc( "P250" );
		case WEAPON_SCAR20:
			return strenc( "SCAR-20" );
		case WEAPON_SG556:
			return strenc( "SG 553" );
		case WEAPON_SSG08:
			return strenc( "SSG 08" );
		case WEAPON_KNIFE:
			return strenc( "Knife" );
		case WEAPON_FLASHBANG:
			return strenc( "Flashbang" );
		case WEAPON_HEGRENADE:
			return strenc( "HE Grenade" );
		case WEAPON_SMOKEGRENADE:
			return strenc( "Smoke Grenade" );
		case WEAPON_MOLOTOV:
			return strenc( "Molotov" );
		case WEAPON_DECOY:
			return strenc( "Decoy" );
		case WEAPON_INCGRENADE:
			return strenc( "Incendiary Grenade" );
		case WEAPON_C4:
			return strenc( "C4" );
		case WEAPON_KNIFE_T:
			return strenc( "Knife" );
		case WEAPON_M4A1_SILENCER:
			return strenc( "M4A1-S" );
		case WEAPON_USP_SILENCER:
			return strenc( "USP-S" );
		case WEAPON_CZ75A:
			return strenc( "CZ75-Auto" );
		case WEAPON_REVOLVER:
			return strenc( "R8 Revolver" );
		default:
			return strenc( "Knife" );
	}

	return "";
}
