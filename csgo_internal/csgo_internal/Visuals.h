#pragma once

struct GlowObject_t
{
	CBaseEntity* m_pEntity; // 0
	Vector m_vGlowColor; // 4
	float m_flGlowAlpha; // 16
	unsigned char junk[ 16 ]; // 20
	bool m_bRenderWhenOccluded; // 36
	bool m_bRenderWhenUnoccluded; // 37
	bool m_bFullBloomRender; // 38
	unsigned char junk2[ 13 ]; // 39
};

class CVisuals
{
public:
	void Run();
	void Watermark();
	void BulletTraceOverlay( CBaseEntity* entity, Color color );
	bool EntityIsInvalid( CBaseEntity* Entity );
	void DrawGlow();
private:
	bool InGame();

	void PlayerESP( int index );
	void WorldESP( int index );
	void BulletTrace( CBaseEntity* entity, Color color );
	void PlayerBox( float x, float y, float w, float h, Color clr );
	void HealthBar( Vector bot, Vector top, float health );
	void Skeleton( CBaseEntity* Entity, Color color );
	void DrawHitbox( matrix3x4a_t* matrix, Vector bbmin, Vector bbmax, int bone, Color color );
	void Hitboxes( CBaseEntity* Entity, Color color );
	void DrawCrosshair( Color color );
};
