#pragma once

#include "Common.h"

struct COffsets
{
	DWORD m_iHealth;
	DWORD m_iTeamNum;
	DWORD m_bDormant;
	DWORD m_bGunGameImmunity;
	DWORD m_lifeState;
	DWORD m_fFlags;
	DWORD m_Local;
	DWORD m_nTickBase;
	DWORD m_nForceBone;
	DWORD m_mBoneMatrix;
	DWORD m_nModelIndex;
	DWORD m_viewPunchAngle;
	DWORD m_aimPunchAngle;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_vecVelocity;
	DWORD m_szLastPlaceName;
	DWORD m_flNextPrimaryAttack;
	DWORD m_hActiveWeapon;
	DWORD m_fAccuracyPenalty;
	DWORD m_Collision;
	DWORD m_iShotsFired;
	DWORD m_iWeaponID;
	DWORD m_nMoveType;
	DWORD m_nHitboxSet;
	DWORD m_bHasHelmet;
	DWORD m_ArmorValue;
	DWORD m_CollisionGroup;
	DWORD m_iClass;
	DWORD m_bIsBroken;
	DWORD m_angEyeAngles;
	DWORD m_hOwnerEntity;
	DWORD m_flC4Blow;
	DWORD m_flFlashDuration;
	DWORD m_iGlowIndex;

	DWORD m_nFallbackPaintKit;
	DWORD m_nFallbackSeed;
	DWORD m_flFallbackWear;
	DWORD m_nFallbackStatTrak;
	DWORD m_AttributeManager;
	DWORD m_Item;
	DWORD m_iEntityLevel;
	DWORD m_iItemIDHigh;
	DWORD m_iItemIDLow;
	DWORD m_iAccountID;
	DWORD m_iEntityQuality;
	DWORD m_OriginalOwnerXuidLow;
	DWORD m_OriginalOwnerXuidHigh;
	DWORD m_iItemDefinitionIndex;
	DWORD m_iClip1;
	DWORD m_bReloadVisuallyComplete;

	//sigs
	DWORD CalcPlayerView;
	DWORD d3d9Device;
	DWORD SendPacket;
	DWORD GlowManager;
	DWORD LoadFromBufferEx;
	DWORD InitKeyValuesEx;
	DWORD ServerRankRevealAllEx;
	DWORD IsReadyEx;
};

extern COffsets offsets;

namespace Offsets
{
	extern void GrabOffsets();
}
