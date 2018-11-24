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

/*
struct WeaponInfo_t
{
	float m_flArmorRatio; 
	float m_flPenetration; 
	float m_iDamage; 
	float m_flRange; 
	float m_flRangeModifier; 
};
*/

class CHudTexture
{
public:
	char szShortName[64]; //0x0000
	char szTextureFile[64]; //0x0040
	bool bRenderUsingFont; //0x0080
	bool bPrecached; //0x0081
	char cCharacterInFont; //0x0082
	BYTE pad_0x0083; //0x0083
	int hFont; //0x0084
	int iTextureId; //0x0088
	float afTexCoords[4]; //0x008C
	int iPosX[4]; //0x009C
};//Size=0x00AC

class WeaponInfo_t {
public:
    char    _0x0000[20];
    __int32 iMaxClip1;			//0x0014 
    char    _0x0018[12];
    __int32 max_reserved_ammo;	//0x0024 
    char    _0x0028[96];
    char*   hud_name;			//0x0088 
    char*   weapon_name;		//0x008C 
    char    _0x0090[60];
    __int32 type;				//0x00CC 
    __int32 price;				//0x00D0 
    __int32 reward;				//0x00D4 
    char    _0x00D8[20];
    char    full_auto;			//0x00EC 
    char    _0x00ED[3];
    __int32 iDamage;			//0x00F0 
    float   flArmorRatio;		//0x00F4 
    __int32 iBullets;			//0x00F8 
    float   flPenetration;		//0x00FC 
    char    _0x0100[8];
    float   flRange;
    float   flRangeModifier;
    float   flThrowVelocity;
    char    _0x0110[12];
    char    silencer;			//0x0120 
    char    _0x0121[15];
    float   max_speed;			//0x0130 
    float   max_speed_alt;		//0x0134 
    char    _0x0138[76];
    __int32 recoil_seed;		//0x0184 
    char    _0x0188[32];
};// Size=0x01A8

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
