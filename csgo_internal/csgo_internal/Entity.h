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

class CHudTexture
{
public:
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
};//Size=0x00AC

class WeaponInfo_t
{
public:
	BYTE		pad_0x0000[4];			//0x0000
	bool		bParsedScript;			//0x0004
	bool		bLoadedHudElements;		//0x0005
	char		szClassName[80];		//0x0006
	char		szPrintName[80];		//0x0056
	char		szViewModel[80];		//0x00A6
	char		szWorldModel[80];		//0x00F6
	char		szBulletType[32];		//0x0146
	char		szWorldDroppedModel[80];//0x0166
	char		szAnimationPrefix[16];	//0x01B6
	BYTE		pad_0x01C6[2];			//0x01C6
	int			iSlot;					//0x01C8
	int			iPosition;				//0x01CC
	int			iMaxClip1;				//0x01D0
	int			iMaxClip2;				//0x01D4
	int			iDefaultClip1;			//0x01D8
	int			iDefaultClip2;			//0x01DC
	int			iWeight;				//0x01E0
	int			iRumbleEffect;			//0x01E4
	bool		bAutoSwitchTo;			//0x01E8
	bool		bAutoSwitchFrom;		//0x01E9
	BYTE		pad_0x01EA[2];			//0x01EA
	int			iFlags;					//0x01EC
	char		szAmmo1[16];			//0x01F0
	char		szAmmo2[16];			//0x0200
	char		szAIAddOn[80];			//0x0210
	char		aShootSounds[17][80];	//0x0260
	char		PAD_NEW[80];
	int			iAmmoType;
	int			iAmmo2Type;
	bool		bMeleeWeapon;
	bool		bBuiltRightHanded;
	bool		bAllowFlipping;
	BYTE		pad_0x07BB;
	int			iSpriteCount;
	CHudTexture* iconWeaponS;
	CHudTexture* iconWeapon;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconWeaponSmall;
	bool		bShowUsageHint;
	BYTE		pad_0x07E5[3];
	CSWeaponType iWeaponType;
	int			iTeam;
	int			iWeaponID;
	bool		bFullAuto;
	BYTE		pad_0x07F5[3];
	float		flHeatPerShot;
	int			iWeaponPrice;
	float		flArmorRatio;
	float		flMaxPlayerSpeed;
	float		flMaxPlayerSpeedAlt;
	int			iCrosshairMinDistance;
	int			iCrosshairDeltaDistance;
	float		flPenetration;
	int			iDamage;
	float		flRange;
	float		flRangeModifier;
	int			iBullets;
	float		flCycleTime;
	float		flCycleTimeAlt;
	char		szHeatEffect[80];
	float		aflSmokeColor[3];
	char		szMuzzleFlashEffect_1stPerson[80];
	char		szMuzzleFlashEffect_3rdPerson[80];
	char		szEjectBrassEffect[80];
	char		szTracerEffect[80];
	int			iTracerFrequency;
	float		flSpread;
	float		flSpreadAlt;
	float		flInaccuracyCrouch;
	float		flInaccuracyCrouchAlt;
	float		flInaccuracyStand;
	float		flInaccuracyStandAlt;
	float		flInaccuracyJump;
	float		flInaccuracyJumpAlt;
	float		flInaccuracyLand;
	float		flInaccuracyLandAlt;
	float		flInaccuracyLadder;
	float		flInaccuracyLadderAlt;
	float		flInaccuracyFire;
	float		flInaccuracyFireAlt;
	float		flInaccuracyMove;
	float		flInaccuracyMoveAlt;
	float		flRecoveryTimeStand;
	float		flRecoveryTimeCrouch;
	float		flInaccuracyReload;
	float		flInaccuracyAltSwitch;
	float		flRecoilAngle;
	float		flRecoilAngleAlt;
	float		flRecoilAngleVariance;
	float		flRecoilAngleVarianceAlt;
	float		flRecoilMagnitude;
	float		flRecoilMagnitudeAlt;
	float		flRecoilMagnitudeVariance;
	float		flRecoilMagnitudeVarianceAlt;
	int			iRecoilSeed;
	float		flFlinchVelocityModifierLarge;
	float		flFlinchVelocityModifierSmall;
	float		flTimeToIdle;
	float		flIdleInterval;
	float		recoilTable[2][128];
	int			iZoomLevels;
	int			iZoomFOV[3];
	float		fZoomTime[3];
	bool		bHideViewModelZoomed;
	char		szZoomInSound[80];
	char		szZoomOutSound[80];
	BYTE		pad_0x0F11[3];
	float		flBotAudibleRange;
	bool		bCanUseWithShield;
	char		szWrongTeamMsg[32];
	char		szAnimPrefix[16];
	char		szShieldViewModel[64];
	char		szAddonModel[80];
	char		szAddonLocation[80];
	char		szSilencerModel[80];
	BYTE		pad_0x1079[3];
	float		flAddonScale;
	float		flThrowVelocity;
	int			iKillAward;
	BYTE		pad_0x1088[8];
};

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
