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

class CBaseCombatWeapon;
class CBaseEntity
{
public:
	char				__pad[0x64];
	int					index;
	int					GetHealth();
	int					GetTeam();
	int					GetFlags();
	int					GetTickBase();
	int					GetShotsFired();
	int					GetMoveType();
	int					GetModelIndex();
	int					GetHitboxSet();
	int					GetUserID();
	int					GetArmor();
	int					GetCollisionGroup();
	int					PhysicsSolidMaskForEntity();
	int					GetOwner();
	int					GetGlowIndex();
	bool				GetAlive();
	bool				GetDormant();
	bool				GetImmune();
	bool				IsEnemy();
	bool				IsVisible( int bone );
	bool				m_visible;
	bool				IsBroken();
	bool				HasHelmet();
	bool				IsFlashed();
	bool				IsTargetingLocal();
	float				GetFlashDuration();
	float				GetBombTimer();
	QAngle				GetViewPunch();
	QAngle				GetPunch();
	QAngle				GetEyeAngles();
	Vector				GetOrigin();
	Vector				GetEyePosition();
	Vector				GetBonePosition( int iBone );
	bool				SetupBones( matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime );
	Vector				GetVelocity();
	Vector				GetPredicted( Vector p0 );
	ICollideable*		GetCollideable();
	player_info_t		GetPlayerInfo();
	model_t*			GetModel();
	std::string			GetName();
	std::string			GetSteamID();
	std::string			GetLastPlace();
	int&				GetXUIDLow();
	int&				GetXUIDHigh();
	CBaseCombatWeapon*	GetWeapon();
	ClientClass*		GetClientClass();

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

class WeaponInfo_t
{
public:
	char pad_0x0000[ 99 ];	//0x0000
	char m_name[ 80 ];			//0x00
	char pad_0x07B0[ 0x6FD ];	//0x0000
	float m_flArmorRatio;		//0x07B0 
	float unkwn1;				//0x07B4 
	float unkwn2;				//0x07B8 
	__int32 unkwn3;				//0x07BC 
	__int32 unkwn4;				//0x07C0 
	float m_flPenetration;		//0x07C4 
	__int32 m_iDamage;			//0x07C8 
	float m_flRange;			//0x07CC 
	float m_flRangeModifier;	//0x07D0 
};

class CHudTexture
{
	public:
	char type[ 64 ]; //0x0000
	char subtype[ 64 ]; //0x0040
	char unknowndata00[ 2 ]; //0x0080
	char charinFont; //0x0082
	char unknowndata01[ 1 ]; //0x0083
};//Size=0x00AC

class CCSWeaponInfo_t
{
	public:
	void* vTable; //0x0000
	BYTE N0309F389; //0x0004
	BYTE N030AC4C1; //0x0005
	char name_class[ 93 ]; //0x0006
	char printname[ 80 ]; //0x00
	char viewmodel[ 80 ]; //0x00A6
	char playermodel[ 80 ]; //0x00F6
	char bullettype[ 32 ]; //0x0146
	char anim_prefix[ 58 ]; //0x0166
	char N03122066[ 32 ]; //0x01A0
	char contentpack[ 32 ]; //0x01C0
	char unknowndata00[ 128 ]; //0x01E0
	char name_real[ 32 ]; //0x0260
	char unknowndata01[ 1184 ]; //0x0280
	CHudTexture* hudTexture_silenced; //0x0720
	CHudTexture* hudTexture; //0x0724
	CHudTexture* hudTexture_ammo; //0x0728
	char unknowndata02[ 4 ]; //0x072C
	CHudTexture* hudTexture_crosshair_01; //0x0730
	CHudTexture* hudTexture_crosshair_02; //0x0734
	CHudTexture* hudTexture_crosshair_03; //0x0738
	CHudTexture* hudTexture_crosshair_04; //0x073C
	char unknowndata03[ 8 ]; //0x0740
	__int32 WeaponType; //0x0748
	__int32 Team; //0x074C
	__int32 WeaponID; //0x0750
	BYTE FullAuto; //0x0754
	char unknowndata04[ 7 ]; //0x0755
	__int32 WeaponPrice; //0x075C
	float WeaponArmorRatio; //0x0760
	float MaxPlayerSpeed; //0x0764
	float MaxPlayerSpeedAlt; //0x0768
	__int32 CrosshairMinDistance; //0x076C
	__int32 CrosshairDeltaDistance; //0x0770
	float flPenetrationPower; //0x0774
	__int32 iDamage; //0x0778
	float flRange; //0x077C
	float flRangeModifier; //0x0780
	__int32 iBullets; //0x0784
	float CycleTime; //0x0788

};//Size=0x078C

class CBaseCombatWeapon
{
public:
	char			__pad[0x64];
	int				index;
	int&			GetWeaponID();
	float&			GetNextPrimaryAttack();
	float&			GetAccuracyPenalty();
	int&			GetXUIDLow();
	int&			GetXUIDHigh();
	int&			GetEntityQuality();
	int&			GetAccountID();
	int&			GetItemIDHigh();
	int&			GetItemDefinitionIndex();
	int&			GetFallbackPaintKit();
	int&			GetFallbackStatTrak();
	float&			GetFallbackWear();
	bool			IsEmpty();
	bool			IsReloading();
	void			UpdateAccuracyPenalty();
	float			GetWeaponSpread();
	float			GetWeaponCone();
	WeaponInfo_t*	GetCSWpnData();
	bool			IsGun();
	std::string		GetWeaponName();
};