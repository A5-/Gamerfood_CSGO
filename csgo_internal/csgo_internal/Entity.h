#pragma once

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class CBaseCombatWeapon;

class CBaseEntity
{
public:
	char __pad[0x64];
	int index;
	int GetHealth();
	int GetTeam();
	int GetFlags();
	int GetTickBase();
	int GetShotsFired();
	int GetMoveType();
	int GetModelIndex();
	int GetHitboxSet();
	int GetUserID();
	int GetArmor();
	int GetCollisionGroup();
	int PhysicsSolidMaskForEntity();
	int GetOwner();
	int GetGlowIndex();
	bool GetAlive();
	bool GetDormant();
	bool GetImmune();
	bool IsEnemy();
	bool IsVisible( int bone );
	bool m_visible;
	bool IsBroken();
	bool HasHelmet();
	bool IsFlashed();
	bool IsTargetingLocal();
	float GetFlashDuration();
	float GetBombTimer();
	QAngle GetViewPunch();
	QAngle GetPunch();
	QAngle GetEyeAngles();
	Vector GetOrigin();
	Vector GetEyePosition();
	Vector GetBonePosition( int iBone );
	bool SetupBones( matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime );
	Vector GetVelocity();
	Vector GetPredicted( Vector p0 );
	ICollideable* GetCollideable();
	player_info_t GetPlayerInfo();
	model_t* GetModel();
	std::string GetName();
	std::string GetSteamID();
	std::string GetLastPlace();
	int& GetXUIDLow();
	int& GetXUIDHigh();
	CBaseCombatWeapon* GetWeapon();
	ClientClass* GetClientClass();
};

class WeaponInfo_t {
public:
    char pad_0x0000[0x4]; //0x0000
    char* m_WeaponName; //0x0004 
    char pad_0x0008[0xC]; //0x0008
    __int32 m_MaxClip; //0x0014 
    char pad_0x0018[0xC]; //0x0018
    __int32 m_MaxReservedAmmo; //0x0024 
    char pad_0x0028[0x4]; //0x0028
    char* m_WeaponMdlPath; //0x002C 
    char pad_0x0030[0x4]; //0x0030
    char* m_DropWeaponMdlPath; //0x0034 
    char pad_0x0038[0x48]; //0x0038
    char* m_BulletType; //0x0080 
    char pad_0x0084[0x4]; //0x0084
    char* m_WeaponHudName; //0x0088 
    char pad_0x008C[0x40]; //0x008C
    __int32 m_WeaponType; //0x00CC 
    __int32 m_WeaponPrice; //0x00D0 
    __int32 m_WeaponReward; //0x00D4 
    char* m_WeaponGroupName; //0x00D8 
    char pad_0x00DC[0x10]; //0x00DC
    unsigned char m_FullAuto; //0x00EC 
    char pad_0x00ED[0x3]; //0x00ED
    __int32 iDamage; //0x00F0 
    float flArmorRatio; //0x00F4 
    __int32 m_iBullets;
    float flPenetration; //0x00F8
    char pad_0x00F8[0x8]; //0x00FC
    float flRange; //0x0108 
    float flRangeModifier; //0x010C 
    char pad_0x0110[0x10]; //0x0110
    unsigned char m_HasSilencer; //0x0120 
    char pad_0x0121[0xF]; //0x0121
    float m_MaxSpeed; //0x0130 
    float m_MaxSpeed2; //0x0134 
    char pad_0x0138[0x4C]; //0x0138
    __int32 m_RecoilSeed; //0x0184 
    char pad_0x0188[0x68]; //0x0188
    char* m_WeaponTracesType; //0x01F0 
    char pad_0x01F4[0x638]; //0x01F4
};// Size=0x082C

class CBaseCombatWeapon
{
public:
	char __pad[0x64];
	int index;
	short& GetWeaponID();
	float& GetNextPrimaryAttack();
	float& GetAccuracyPenalty();
	int& GetXUIDLow();
	int& GetXUIDHigh();
	int& GetEntityQuality();
	int& GetAccountID();
	int& GetItemIDHigh();
	int& GetItemDefinitionIndex();
	int& GetFallbackPaintKit();
	int& GetFallbackStatTrak();
	float& GetFallbackWear();
	bool IsEmpty();
	bool IsReloading();
	void UpdateAccuracyPenalty();
	float GetWeaponSpread();
	float GetWeaponCone();
	WeaponInfo_t* GetCSWpnData();
	bool IsGun();
	std::string GetWeaponName();
};
