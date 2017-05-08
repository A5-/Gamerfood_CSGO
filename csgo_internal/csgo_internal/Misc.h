#pragma once

#include "SDK.h"

#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

// intp is an integer that can accomodate a pointer
// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
typedef intptr_t intp;
typedef uintptr_t uintp;

inline float BitsToFloat( uint32 i )
{
	union Convertor_t
	{
		float f;
		unsigned long ul;
	} tmp;
	tmp.ul = i;
	return tmp.f;
}

#define FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )

#define VEC_T_NAN FLOAT32_NAN

#define DECL_ALIGN(x)			__declspec( align( x ) )

#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)

//extern float( *pfSqrt )( float x );

//#define FastSqrt(x)			(*pfSqrt)(x)

#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

#define IN_ATTACK				(1 << 0)
#define IN_JUMP					(1 << 1)
#define IN_DUCK					(1 << 2)
#define IN_FORWARD				(1 << 3)
#define IN_BACK					(1 << 4)
#define IN_USE					(1 << 5)
#define IN_CANCEL				(1 << 6)
#define IN_LEFT					(1 << 7)
#define IN_RIGHT				(1 << 8)
#define IN_MOVELEFT				(1 << 9)
#define IN_MOVERIGHT			(1 << 10)
#define IN_ATTACK2				(1 << 11)
#define IN_RUN					(1 << 12)
#define IN_RELOAD				(1 << 13)
#define IN_ALT1					(1 << 14)
#define IN_ALT2					(1 << 15)
#define IN_SCORE				(1 << 16)
#define IN_SPEED				(1 << 17)
#define IN_WALK					(1 << 18)
#define IN_ZOOM					(1 << 19)
#define IN_WEAPON1				(1 << 20)
#define IN_WEAPON2				(1 << 21)
#define IN_BULLRUSH				(1 << 22)

#define	FL_ONGROUND				(1 << 0)
#define FL_DUCKING				(1 << 1)
#define	FL_WATERJUMP			(1 << 3)
#define FL_ONTRAIN				(1 << 4)
#define FL_INRAIN				(1 << 5)
#define FL_FROZEN				(1 << 6)
#define FL_ATCONTROLS			(1 << 7)
#define	FL_CLIENT				(1 << 8)
#define FL_FAKECLIENT			(1 << 9)
#define	FL_INWATER				(1 << 10)

#define HIDEHUD_SCOPE			(1 << 11)

#define MAX_AREA_STATE_BYTES		32
#define MAX_AREA_PORTAL_STATE_BYTES 24

#define  Assert( _exp )										((void)0)
#define  AssertAligned( ptr )								((void)0)
#define  AssertOnce( _exp )									((void)0)
#define  AssertMsg( _exp, _msg )							((void)0)
#define  AssertMsgOnce( _exp, _msg )						((void)0)
#define  AssertFunc( _exp, _f )								((void)0)
#define  AssertEquals( _exp, _expectedValue )				((void)0)
#define  AssertFloatEquals( _exp, _expectedValue, _tol )	((void)0)
#define  Verify( _exp )										(_exp)
#define  VerifyEquals( _exp, _expectedValue )           	(_exp)

#define TEXTURE_GROUP_LIGHTMAP						"Lightmaps"
#define TEXTURE_GROUP_WORLD							"World textures"
#define TEXTURE_GROUP_MODEL							"Model textures"
#define TEXTURE_GROUP_VGUI							"VGUI textures"
#define TEXTURE_GROUP_PARTICLE						"Particle textures"
#define TEXTURE_GROUP_DECAL							"Decal textures"
#define TEXTURE_GROUP_SKYBOX						"SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				"ClientEffect textures"
#define TEXTURE_GROUP_OTHER							"Other textures"
#define TEXTURE_GROUP_PRECACHED						"Precached"				// TODO: assign texture groups to the precached materials
#define TEXTURE_GROUP_CUBE_MAP						"CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					"RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					"Unaccounted textures"	// Textures that weren't assigned a texture group.
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		"Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			"Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			"Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			"Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					"DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					"ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					"Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				"Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			"RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					"Morph Targets"

#define	CONTENTS_EMPTY			0		// No contents

#define	CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight
#define CONTENTS_OPAQUE			0x80	// things that cannot be seen through (may be non-solid though)
#define	LAST_VISIBLE_CONTENTS	CONTENTS_OPAQUE

#define ALL_VISIBLE_CONTENTS (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200

// unused 
// NOTE: If it's visible, grab from the top + update LAST_VISIBLE_CONTENTS
// if not visible, then grab from the bottom.
// CONTENTS_OPAQUE + SURF_NODRAW count as CONTENTS_OPAQUE (shadow-casting toolsblocklight textures)
#define CONTENTS_BLOCKLIGHT		0x400

#define CONTENTS_TEAM1			0x800	// per team contents used to differentiate collisions 
#define CONTENTS_TEAM2			0x1000	// between players and objects on different teams

// ignore CONTENTS_OPAQUE on surfaces that have SURF_NODRAW
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000

// hits entities which are MOVETYPE_PUSH (doors, plats, etc.)
#define CONTENTS_MOVEABLE		0x4000

// remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000	// use accurate hitboxes on trace

#define	MASK_SHOT	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)

// NOTE: These are stored in a short in the engine now.  Don't use more than 16 bits
#define	SURF_LIGHT		0x0001		// value will hold the light strength
#define	SURF_SKY2D		0x0002		// don't draw, indicates we should skylight + draw 2d sky but not draw the 3D skybox
#define	SURF_SKY		0x0004		// don't draw, but add to skybox
#define	SURF_WARP		0x0008		// turbulent water warp
#define	SURF_TRANS		0x0010
#define SURF_NOPORTAL	0x0020	// the surface can not have a portal placed on it
#define	SURF_TRIGGER	0x0040	// FIXME: This is an xbox hack to work around elimination of trigger surfaces, which breaks occluders
#define	SURF_NODRAW		0x0080	// don't bother referencing the texture

#define	SURF_HINT		0x0100	// make a primary bsp splitter

#define	SURF_SKIP		0x0200	// completely ignore, allowing non-closed brushes
#define SURF_NOLIGHT	0x0400	// Don't calculate light
#define SURF_BUMPLIGHT	0x0800	// calculate three lightmaps for the surface for bumpmapping
#define SURF_NOSHADOWS	0x1000	// Don't receive shadows
#define SURF_NODECALS	0x2000	// Don't receive decals
#define SURF_NOPAINT	SURF_NODECALS	// the surface can not have paint placed on it
#define SURF_NOCHOP		0x4000	// Don't subdivide patches on this surface 
#define SURF_HITBOX		0x8000	// surface is part of a hitbox

#define	MASK_ALL					(0xFFFFFFFF)
// everything that is normally solid
#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// everything that blocks player movement
#define	MASK_PLAYERSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks npc movement
#define	MASK_NPCSOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
// blocks fluid movement
#define	MASK_NPCFLUID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
// water physics in these contents
#define	MASK_WATER					(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
// everything that blocks lighting
#define	MASK_OPAQUE					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
// everything that blocks lighting, but with monsters added.
#define MASK_OPAQUE_AND_NPCS		(MASK_OPAQUE|CONTENTS_MONSTER)
// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)
// everything that blocks line of sight for players
#define	MASK_VISIBLE					(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
// everything that blocks line of sight for players, but with monsters added.
#define MASK_VISIBLE_AND_NPCS		(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
// bullets see these as solid
#define	MASK_SHOT					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
// bullets see these as solid, except monsters (world+brush only)
#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
// non-raycasted weapons see this as solid (includes grates)
#define MASK_SHOT_HULL				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
// hits solids (not grates) and passes through everything else
#define MASK_SHOT_PORTAL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
// everything normally solid, except monsters (world+brush only)
#define MASK_SOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
// everything normally solid for player movement, except monsters (world+brush only)
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
// everything normally solid for npc movement, except monsters (world+brush only)
#define MASK_NPCSOLID_BRUSHONLY		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC			(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
// just the world, used for route rebuilding
#define MASK_NPCWORLDSTATIC_FLUID	(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
// These are things that can split areaportals
#define MASK_SPLITAREAPORTAL		(CONTENTS_WATER|CONTENTS_SLIME)

// UNDONE: This is untested, any moving water
#define MASK_CURRENT				(CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)

// everything that blocks corpse movement
// UNDONE: Not used yet / may be deleted
#define	MASK_DEADSOLID				(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)

struct colorVec
{
	unsigned r, g, b, a;
};

enum
{
	PITCH = 0, // up / down
	YAW, // left / right
	ROLL // fall over
};

enum FontRenderFlag_t
{
	FONT_LEFT = 0,
	FONT_RIGHT = 1,
	FONT_CENTER = 2
};

enum ItemDefinitionIndex : int
{
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

constexpr int AchievementId[ 167 ]
{
	1001, 1002, 1004, 1005, 1006, 1007, 1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015,
	2001, 2002, 2003, 2004, 2005, 3001, 3002, 3003, 3005, 3006, 3007, 3008, 3009, 3010,
	3011, 3012, 3013, 3014, 3015, 3016, 3017, 3018, 3019, 3020, 3021, 3022, 3023, 3024,
	3025, 3026, 3027, 3028, 3029, 3030, 3031, 3032, 3033, 3034, 3035, 3036, 3037, 3038,
	3039, 3040, 3044, 3046, 3047, 3048, 3049, 3050, 4001, 4003, 4005, 4006, 4007, 4008,
	4009, 4010, 4011, 4012, 4013, 4014, 4015, 4019, 4020, 4022, 4024, 4025, 4026, 4027,
	4030, 4031, 4032, 4033, 4035, 4036, 4037, 4038, 4039, 4040, 4041, 4042, 4043, 4044,
	4045, 4046, 4047, 4048, 5001, 5002, 5003, 5004, 5005, 5006, 5007, 5008, 5009, 5010,
	5011, 5012, 5013, 5014, 5015, 5016, 5017, 5018, 5019, 5020, 5021, 5022, 5023, 5024,
	5025, 5026, 5027, 5029, 5031, 5032, 5033, 5034, 5035, 5036, 5037, 5039, 5040, 5041,
	5042, 5044, 6004, 6006, 6007, 6010, 6011, 6012, 6013, 6018, 6019, 6020, 6021, 6022,
	6023, 6024, 6030, 6031, 6032, 6033, 6034, 6035, 6036, 6037, 6038, 6039, 6040
};

struct player_info_t
{
	char __pad0[0x8];

	int xuidlow;
	int xuidhigh;

	char name[128];
	int userid;
	char guid[33];

	char __pad1[0x17B];
};

class Vector
{
public:
	float x, y, z;
	Vector( void );
	Vector( float X, float Y, float Z );
	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );
	bool IsValid() const;
	float operator[]( int i ) const;
	float& operator[]( int i );
	inline void Zero();
	bool operator==( const Vector& v ) const;
	bool operator!=( const Vector& v ) const;
	__forceinline Vector& operator+=( const Vector& v );
	__forceinline Vector& operator-=( const Vector& v );
	__forceinline Vector& operator*=( const Vector& v );
	__forceinline Vector& operator*=( float s );
	__forceinline Vector& operator/=( const Vector& v );
	__forceinline Vector& operator/=( float s );
	__forceinline Vector& operator+=( float fl );
	__forceinline Vector& operator-=( float fl );
	inline float Length() const;

	__forceinline float LengthSqr( void ) const
	{
		CHECK_VALID( *this );
		return ( this->x * this->x + this->y * this->y + this->z * this->z );
	}

	bool IsZero( float tolerance = 0.01f ) const
	{
		return ( x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance );
	}

	float NormalizeInPlace();
	Vector Normalize();
	__forceinline float DistToSqr( const Vector& vOther ) const;
	__forceinline float Dist( const Vector& vOther ) const;
	float Dot( const Vector& vOther ) const;
	float Dot( const float* fOther ) const;
	float Length2D( void ) const;
	float Length2DSqr( void ) const;
	Vector& operator=( const Vector& vOther );
	Vector operator-( const Vector& v ) const;
	Vector operator+( const Vector& v ) const;
	Vector operator*( const Vector& v ) const;
	Vector operator/( const Vector& v ) const;
	Vector operator*( float fl ) const;
	Vector operator/( float fl ) const;
	// Base address...
	float* Base();
	float const* Base() const;
};

//===============================================
inline void Vector::Init( float ix, float iy, float iz )
{
	x = ix;
	y = iy;
	z = iz;
	CHECK_VALID( *this );
}

//===============================================
inline Vector::Vector( float X, float Y, float Z )
{
	x = X;
	y = Y;
	z = Z;
	CHECK_VALID( *this );
}

//===============================================
inline Vector::Vector( void )
{ }

//===============================================
inline void Vector::Zero()
{
	x = y = z = 0.0f;
}

//===============================================
inline void VectorClear( Vector& a )
{
	a.x = a.y = a.z = 0.0f;
}

//===============================================
inline Vector& Vector::operator=( const Vector& vOther )
{
	CHECK_VALID( vOther );
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	return *this;
}

//===============================================
inline float& Vector::operator[]( int i )
{
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( ( float* )this )[ i ];
}

//===============================================
inline float Vector::operator[]( int i ) const
{
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( ( float* )this )[ i ];
}

//===============================================
inline bool Vector::operator==( const Vector& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x == x ) && ( src.y == y ) && ( src.z == z );
}

//===============================================
inline bool Vector::operator!=( const Vector& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x != x ) || ( src.y != y ) || ( src.z != z );
}

//===============================================
__forceinline void VectorCopy( const Vector& src, Vector& dst )
{
	CHECK_VALID( src );
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

//===============================================
__forceinline Vector& Vector::operator+=( const Vector& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator-=( const Vector& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator*=( float fl )
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator*=( const Vector& v )
{
	CHECK_VALID( v );
	x *= v.x;
	y *= v.y;
	z *= v.z;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator+=( float fl )
{
	x += fl;
	y += fl;
	z += fl;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator-=( float fl )
{
	x -= fl;
	y -= fl;
	z -= fl;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator/=( float fl )
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
__forceinline Vector& Vector::operator/=( const Vector& v )
{
	CHECK_VALID( v );
	Assert( v.x != 0.0f && v.y != 0.0f && v.z != 0.0f );
	x /= v.x;
	y /= v.y;
	z /= v.z;
	CHECK_VALID( *this );
	return *this;
}

//===============================================
inline float Vector::Length( void ) const
{
	CHECK_VALID( *this );

	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	__asm sqrtss xmm0, sqsr
	__asm movss root, xmm0

	return root;
}

//===============================================
inline float Vector::Length2D( void ) const
{
	CHECK_VALID( *this );

	float root = 0.0f;

	float sqst = x * x + y * y;

	__asm
	{
		sqrtss xmm0, sqst
		movss root, xmm0
	}

	return root;
}

//===============================================
inline float Vector::Length2DSqr( void ) const
{
	return ( x * x + y * y );
}

//===============================================
inline Vector CrossProduct( const Vector& a, const Vector& b )
{
	return Vector( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}

//===============================================
float Vector::DistToSqr( const Vector& vOther ) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}

float Vector::Dist( const Vector& vOther ) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length();
}

inline Vector Vector::Normalize()
{
	Vector vector;
	float length = this->Length();

	if( length != 0 )
	{
		vector.x = x / length;
		vector.y = y / length;
		vector.z = z / length;
	}
	else
		vector.x = vector.y = 0.0f;
	vector.z = 1.0f;

	return vector;
}

//===============================================
inline float Vector::NormalizeInPlace()
{
	Vector& v = *this;

	float iradius = 1.f / ( this->Length() + 1.192092896e-07F ); //FLT_EPSILON

	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
}

//===============================================
inline float VectorNormalize( Vector& v )
{
	Assert( v.IsValid() );
	float l = v.Length();
	if( l != 0.0f )
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
		v.z = 1.0f;
	}
	return l;
}

//===============================================
FORCEINLINE float VectorNormalize( float* v )
{
	return VectorNormalize( *( reinterpret_cast< Vector * >( v ) ) );
}

//===============================================
inline Vector Vector::operator+( const Vector& v ) const
{
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline Vector Vector::operator-( const Vector& v ) const
{
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

//===============================================
inline Vector Vector::operator*( float fl ) const
{
	Vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

//===============================================
inline Vector Vector::operator*( const Vector& v ) const
{
	Vector res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}

//===============================================
inline Vector Vector::operator/( float fl ) const
{
	Vector res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

//===============================================
inline Vector Vector::operator/( const Vector& v ) const
{
	Vector res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}

inline float Vector::Dot( const Vector& vOther ) const
{
	const Vector& a = *this;

	return ( a.x * vOther.x + a.y * vOther.y + a.z * vOther.z );
}

inline float Vector::Dot( const float* fOther ) const
{
	const Vector& a = *this;

	return ( a.x * fOther[ 0 ] + a.y * fOther[ 1 ] + a.z * fOther[ 2 ] );
}

inline float* Vector::Base()
{
	return ( float* )this;
}

inline float const* Vector::Base() const
{
	return ( float const* )this;
}

class QAngleByValue;

class QAngle
{
public:
	float x, y, z;

	QAngle( void );
	QAngle( float X, float Y, float Z );

	operator QAngleByValue &()
	{
		return *( ( QAngleByValue * )( this ) );
	}

	operator const QAngleByValue &() const
	{
		return *( ( const QAngleByValue * )( this ) );
	}

	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );
	void Random( float minVal, float maxVal );

	bool IsValid() const;
	void Invalidate();

	bool IsZero()
	{
		CHECK_VALID( *this );
		if( this->x == 0.f && this->y == 0.f && this->z == 0.f )
			return true;

		return false;
	}

	float operator[]( int i ) const;
	float& operator[]( int i );

	float* Base();
	float const* Base() const;

	bool operator==( const QAngle& v ) const;
	bool operator!=( const QAngle& v ) const;

	QAngle& operator+=( const QAngle& v );
	QAngle& operator-=( const QAngle& v );
	QAngle& operator*=( float s );
	QAngle& operator/=( float s );

	float Length() const;
	float LengthSqr() const;

	QAngle& operator=( const QAngle& src );

	QAngle operator-( void ) const;

	QAngle operator+( const QAngle& v ) const;
	QAngle operator-( const QAngle& v ) const;
	QAngle operator*( float fl ) const;
	QAngle operator/( float fl ) const;

	QAngle Clamp();
	QAngle Mod( float N );

	inline QAngle QAngle::Normalize()
	{
		QAngle vector;
		float length = this->Length();

		if( length != 0 )
		{
			vector.x = x / length;
			vector.y = y / length;
			vector.z = z / length;
		}
		else
			vector.x = vector.y = 0.0f;
		vector.z = 1.0f;

		return vector;
	}

	QAngle Normalized()
	{
		if( this->x != this->x )
			this->x = 0;
		if( this->y != this->y )
			this->y = 0;
		if( this->z != this->z )
			this->z = 0;

		if( this->x > 89.f )
			this->x = 89.f;
		if( this->x < -89.f )
			this->x = -89.f;

		while( this->y > 180 )
			this->y -= 360;
		while( this->y <= -180 )
			this->y += 360;

		if( this->y > 180.f )
			this->y = 180.f;
		if( this->y < -180.f )
			this->y = -180.f;

		this->z = 0;

		return *this;
	}
};

//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle( void )
{
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = float_NAN;
#endif
#endif
}

inline QAngle::QAngle( float X, float Y, float Z )
{
	x = X;
	y = Y;
	z = Z;
	CHECK_VALID( *this );
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init( float ix, float iy, float iz )
{
	x = ix;
	y = iy;
	z = iz;
	CHECK_VALID( *this );
}

inline void QAngle::Random( float minVal, float maxVal )
{
	x = minVal + ( ( float )rand() / RAND_MAX ) * ( maxVal - minVal );
	y = minVal + ( ( float )rand() / RAND_MAX ) * ( maxVal - minVal );
	z = minVal + ( ( float )rand() / RAND_MAX ) * ( maxVal - minVal );
	CHECK_VALID( *this );
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=( const QAngle& vOther )
{
	CHECK_VALID( vOther );
	x = vOther.x;
	y = vOther.y;
	z = vOther.z;
	return *this;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==( const QAngle& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x == x ) && ( src.y == y ) && ( src.z == z );
}

inline bool QAngle::operator!=( const QAngle& src ) const
{
	CHECK_VALID( src );
	CHECK_VALID( *this );
	return ( src.x != x ) || ( src.y != y ) || ( src.z != z );
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=( const QAngle& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

inline QAngle& QAngle::operator-=( const QAngle& v )
{
	CHECK_VALID( *this );
	CHECK_VALID( v );
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

inline QAngle& QAngle::operator*=( float fl )
{
	x *= fl;
	y *= fl;
	z *= fl;
	CHECK_VALID( *this );
	return *this;
}

inline QAngle& QAngle::operator/=( float fl )
{
	Assert( fl != 0.0f );
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	CHECK_VALID( *this );
	return *this;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* QAngle::Base()
{
	return ( float* )this;
}

inline float const* QAngle::Base() const
{
	return ( float const* )this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float& QAngle::operator[]( int i )
{
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( ( float* )this )[ i ];
}

inline float QAngle::operator[]( int i ) const
{
	Assert( ( i >= 0 ) && ( i < 3 ) );
	return ( ( float* )this )[ i ];
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float QAngle::Length() const
{
	CHECK_VALID( *this );
	return ( float )sqrt( LengthSqr() ); //todo replace with fastsqrt
}

inline float QAngle::LengthSqr() const
{
	CHECK_VALID( *this );
	return x * x + y * y;
}

inline QAngle QAngle::operator-( void ) const
{
	return QAngle( -x, -y, -z );
}

inline QAngle QAngle::operator+( const QAngle& v ) const
{
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

inline QAngle QAngle::operator-( const QAngle& v ) const
{
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline QAngle QAngle::operator*( float fl ) const
{
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

inline QAngle QAngle::operator/( float fl ) const
{
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

inline QAngle QAngle::Clamp()
{
	CHECK_VALID( *this );

	if( this->x < -89.0f )
		this->x = -89.0f;

	if( this->x > 89.0f )
		this->x = 89.0f;

	while( this->y < -180.0f )
		this->y += 360.0f;

	while( this->y > 180.0f )
		this->y -= 360.0f;

	this->z = 0.0f;

	return *this;
}

inline QAngle QAngle::Mod( float N )
{
	CHECK_VALID( *this );
	this->x = fmod( x, N );
	this->y = fmod( y, N );
	this->z = fmod( z, N );

	return *this;
}

class CUserCmd
{
public:
	virtual ~CUserCmd()
	{ };
	int command_number;
	int tick_count;
	QAngle viewangles;
	Vector aimdirection;
	float forwardmove;
	float sidemove;
	float upmove;
	int buttons;
	byte impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
	char pad_0x4C[0x18];
};

struct VMatrix
{
	float m[ 4 ][ 4 ];

	inline float* operator[]( int i )
	{
		return m[ i ];
	}

	inline const float* operator[]( int i ) const
	{
		return m[ i ];
	}
};

struct matrix3x3_t
{
	float m_flMatVal[3][3];
};

struct matrix3x4_t
{
	matrix3x4_t()
	{}

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		m_flMatVal[ 0 ][ 0 ] = m00;
		m_flMatVal[ 0 ][ 1 ] = m01;
		m_flMatVal[ 0 ][ 2 ] = m02;
		m_flMatVal[ 0 ][ 3 ] = m03;
		m_flMatVal[ 1 ][ 0 ] = m10;
		m_flMatVal[ 1 ][ 1 ] = m11;
		m_flMatVal[ 1 ][ 2 ] = m12;
		m_flMatVal[ 1 ][ 3 ] = m13;
		m_flMatVal[ 2 ][ 0 ] = m20;
		m_flMatVal[ 2 ][ 1 ] = m21;
		m_flMatVal[ 2 ][ 2 ] = m22;
		m_flMatVal[ 2 ][ 3 ] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		m_flMatVal[ 0 ][ 0 ] = xAxis.x;
		m_flMatVal[ 0 ][ 1 ] = yAxis.x;
		m_flMatVal[ 0 ][ 2 ] = zAxis.x;
		m_flMatVal[ 0 ][ 3 ] = vecOrigin.x;
		m_flMatVal[ 1 ][ 0 ] = xAxis.y;
		m_flMatVal[ 1 ][ 1 ] = yAxis.y;
		m_flMatVal[ 1 ][ 2 ] = zAxis.y;
		m_flMatVal[ 1 ][ 3 ] = vecOrigin.y;
		m_flMatVal[ 2 ][ 0 ] = xAxis.z;
		m_flMatVal[ 2 ][ 1 ] = yAxis.z;
		m_flMatVal[ 2 ][ 2 ] = zAxis.z;
		m_flMatVal[ 2 ][ 3 ] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void SetOrigin( Vector const& p )
	{
		m_flMatVal[ 0 ][ 3 ] = p.x;
		m_flMatVal[ 1 ][ 3 ] = p.y;
		m_flMatVal[ 2 ][ 3 ] = p.z;
	}

	inline void Invalidate( void )
	{
		for( int i = 0; i < 3; i++ )
		{
			for( int j = 0; j < 4; j++ )
			{
				m_flMatVal[ i ][ j ] = VEC_T_NAN;
			}
		}
	}

	float* operator[]( int i )
	{
		Assert( ( i >= 0 ) && ( i < 3 ) );
		return m_flMatVal[ i ];
	}

	const float* operator[]( int i ) const
	{
		Assert( ( i >= 0 ) && ( i < 3 ) );
		return m_flMatVal[ i ];
	}

	float* Base()
	{
		return &m_flMatVal[ 0 ][ 0 ];
	}

	const float* Base() const
	{
		return &m_flMatVal[ 0 ][ 0 ];
	}

	float m_flMatVal[ 3 ][ 4 ];
};

class ALIGN16 matrix3x4a_t : public matrix3x4_t
{
public:
	/*
	matrix3x4a_t() { if (((size_t)Base()) % 16 != 0) { Error( "matrix3x4a_t missaligned" ); } }
	*/
	matrix3x4a_t& operator=( const matrix3x4_t& src )
	{
		memcpy( Base(), src.Base(), sizeof( float) * 3 * 4 );
		return *this;
	};
};

class ICollideable
{
public:
	virtual void pad0();
	virtual const Vector& OBBMins() const;
	virtual const Vector& OBBMaxs() const;
};

struct mstudiobbox_t
{
	int bone;
	int group; // intersection group
	Vector bbmin; // bounding box 
	Vector bbmax;
	int hitboxnameindex; // offset to the name of the hitbox.
	int pad[ 3 ];
	float radius;
	int pad2[ 4 ];

	char* getHitboxName()
	{
		if( hitboxnameindex == 0 )
			return "";

		return ( ( char* )this ) + hitboxnameindex;
	}
};

class Color
{
public:
	Color()
	{
		*( ( int * )this ) = 0;
	}

	Color( int color32 )
	{
		*( ( int * )this ) = color32;
	}

	Color( int _r, int _g, int _b )
	{
		SetColor( _r, _g, _b, 255 );
	}

	Color( int _r, int _g, int _b, int _a )
	{
		SetColor( _r, _g, _b, _a );
	}

	void SetColor( int _r, int _g, int _b, int _a = 255 )
	{
		_color[ 0 ] = ( unsigned char ) _r;
		_color[ 1 ] = ( unsigned char ) _g;
		_color[ 2 ] = ( unsigned char ) _b;
		_color[ 3 ] = ( unsigned char ) _a;
	}

	void GetColor( int& _r, int& _g, int& _b, int& _a ) const
	{
		_r = _color[ 0 ];
		_g = _color[ 1 ];
		_b = _color[ 2 ];
		_a = _color[ 3 ];
	}

	void SetRawColor( int color32 )
	{
		*( ( int* )this ) = color32;
	}

	int GetRawColor() const
	{
		return *( ( int* )this );
	}

	int GetD3DColor() const
	{
		return ( ( int ) ( ( ( ( _color[ 3 ] ) & 0xff ) << 24 ) | ( ( ( _color[ 0 ] ) & 0xff ) << 16 ) | ( ( ( _color[ 1 ] ) & 0xff ) << 8 ) | ( ( _color[ 2 ] ) & 0xff ) ) );
	}

	inline int r() const
	{
		return _color[ 0 ];
	}

	inline int g() const
	{
		return _color[ 1 ];
	}

	inline int b() const
	{
		return _color[ 2 ];
	}

	inline int a() const
	{
		return _color[ 3 ];
	}

	inline float rBase() const
	{
		return _color[ 0 ] / 255.0f;
	}

	inline float gBase() const
	{
		return _color[ 1 ] / 255.0f;
	}

	inline float bBase() const
	{
		return _color[ 2 ] / 255.0f;
	}

	inline float aBase() const
	{
		return _color[ 3 ] / 255.0f;
	}

	unsigned char& operator[]( int index )
	{
		return _color[ index ];
	}

	const unsigned char& operator[]( int index ) const
	{
		return _color[ index ];
	}

	bool operator ==( const Color& rhs ) const
	{
		return ( *( ( int * )this ) == *( ( int * ) &rhs ) );
	}

	bool operator !=( const Color& rhs ) const
	{
		return !( operator==( rhs ) );
	}

	Color& operator=( const Color& rhs )
	{
		SetRawColor( rhs.GetRawColor() );
		return *this;
	}

	float* Base()
	{
		float clr[3];

		clr[ 0 ] = _color[ 0 ] / 255.0f;
		clr[ 1 ] = _color[ 1 ] / 255.0f;
		clr[ 2 ] = _color[ 2 ] / 255.0f;

		return &clr[ 0 ];
	}

	float* BaseAlpha()
	{
		float clr[4];

		clr[ 0 ] = _color[ 0 ] / 255.0f;
		clr[ 1 ] = _color[ 1 ] / 255.0f;
		clr[ 2 ] = _color[ 2 ] / 255.0f;
		clr[ 3 ] = _color[ 3 ] / 255.0f;

		return &clr[ 0 ];
	}

	float Hue() const
	{
		if( _color[ 0 ] == _color[ 1 ] && _color[ 1 ] == _color[ 2 ] )
		{
			return 0.0f;
		}

		float r = _color[ 0 ] / 255.0f;
		float g = _color[ 1 ] / 255.0f;
		float b = _color[ 2 ] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
				min = r < g ? r : g < b ? g : b;
		float delta = max - min;
		float hue = 0.0f;

		if( r == max )
		{
			hue = ( g - b ) / delta;
		}
		else if( g == max )
		{
			hue = 2 + ( b - r ) / delta;
		}
		else if( b == max )
		{
			hue = 4 + ( r - g ) / delta;
		}
		hue *= 60;

		if( hue < 0.0f )
		{
			hue += 360.0f;
		}
		return hue;
	}

	float Saturation() const
	{
		float r = _color[ 0 ] / 255.0f;
		float g = _color[ 1 ] / 255.0f;
		float b = _color[ 2 ] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
				min = r < g ? r : g < b ? g : b;
		float l, s = 0;

		if( max != min )
		{
			l = ( max + min ) / 2;
			if( l <= 0.5f )
				s = ( max - min ) / ( max + min );
			else
				s = ( max - min ) / ( 2 - max - min );
		}
		return s;
	}

	float Brightness() const
	{
		float r = _color[ 0 ] / 255.0f;
		float g = _color[ 1 ] / 255.0f;
		float b = _color[ 2 ] / 255.0f;

		float max = r > g ? r : g > b ? g : b,
				min = r < g ? r : g < b ? g : b;
		return ( max + min ) / 2;
	}

	static Color FromHSB( float hue, float saturation, float brightness )
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - ( int ) h;
		float p = brightness * ( 1.0f - saturation );
		float q = brightness * ( 1.0f - saturation * f );
		float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );

		if( h < 1 )
		{
			return Color(
			             ( unsigned char ) ( brightness * 255 ),
			             ( unsigned char ) ( t * 255 ),
			             ( unsigned char ) ( p * 255 )
			            );
		}
		else if( h < 2 )
		{
			return Color(
			             ( unsigned char ) ( q * 255 ),
			             ( unsigned char ) ( brightness * 255 ),
			             ( unsigned char ) ( p * 255 )
			            );
		}
		else if( h < 3 )
		{
			return Color(
			             ( unsigned char ) ( p * 255 ),
			             ( unsigned char ) ( brightness * 255 ),
			             ( unsigned char ) ( t * 255 )
			            );
		}
		else if( h < 4 )
		{
			return Color(
			             ( unsigned char ) ( p * 255 ),
			             ( unsigned char ) ( q * 255 ),
			             ( unsigned char ) ( brightness * 255 )
			            );
		}
		else if( h < 5 )
		{
			return Color(
			             ( unsigned char ) ( t * 255 ),
			             ( unsigned char ) ( p * 255 ),
			             ( unsigned char ) ( brightness * 255 )
			            );
		}
		else
		{
			return Color(
			             ( unsigned char ) ( brightness * 255 ),
			             ( unsigned char ) ( p * 255 ),
			             ( unsigned char ) ( q * 255 )
			            );
		}
	}

	static Color Red()
	{
		return Color( 255, 0, 0 );
	}

	static Color Green()
	{
		return Color( 0, 255, 0 );
	}

	static Color Blue()
	{
		return Color( 0, 0, 255 );
	}

	static Color LightBlue()
	{
		return Color( 100, 100, 255 );
	}

	static Color Grey()
	{
		return Color( 128, 128, 128 );
	}

	static Color DarkGrey()
	{
		return Color( 45, 45, 45 );
	}

	static Color Black()
	{
		return Color( 0, 0, 0 );
	}

	static Color White()
	{
		return Color( 255, 255, 255 );
	}

	static Color Purple()
	{
		return Color( 220, 0, 220 );
	}

	//Menu
	static Color Background()
	{
		return Color( 55, 55, 55 );
	}

	static Color FrameBorder()
	{
		return Color( 80, 80, 80 );
	}

	static Color MainText()
	{
		return Color( 230, 230, 230 );
	}

	static Color HeaderText()
	{
		return Color( 49, 124, 230 );
	}

	static Color CurrentTab()
	{
		return Color( 55, 55, 55 );
	}

	static Color Tabs()
	{
		return Color( 23, 23, 23 );
	}

	static Color Highlight()
	{
		return Color( 49, 124, 230 );
	}

	static Color ElementBorder()
	{
		return Color( 0, 0, 0 );
	}

	static Color SliderScroll()
	{
		return Color( 78, 143, 230 );
	}

private:
	unsigned char _color[4];
};

class CViewSetup
{
public:
	int x, x_old;
	int y, y_old;
	int width, width_old;
	int height, height_old;
	bool m_bOrtho;
	float m_OrthoLeft;
	float m_OrthoTop;
	float m_OrthoRight;
	float m_OrthoBottom;
	bool m_bCustomViewMatrix;
	matrix3x4_t m_matCustomViewMatrix;
	char pad_0x68[0x48];
	float fov;
	float fovViewmodel;
	Vector origin;
	QAngle angles;
	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;
	float m_flAspectRatio;
	float m_flNearBlurDepth;
	float m_flNearFocusDepth;
	float m_flFarFocusDepth;
	float m_flFarBlurDepth;
	float m_flNearBlurRadius;
	float m_flFarBlurRadius;
	int m_nDoFQuality;
	int m_nMotionBlurMode;
	float m_flShutterTime;
	Vector m_vShutterOpenPosition;
	QAngle m_shutterOpenAngles;
	Vector m_vShutterClosePosition;
	QAngle m_shutterCloseAngles;
	float m_flOffCenterTop;
	float m_flOffCenterBottom;
	float m_flOffCenterLeft;
	float m_flOffCenterRight;
	int m_EdgeBlur;
};

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

typedef unsigned long HFont;

struct Vector2D
{
public:
	float x, y;

	Vector2D()
	{}

	Vector2D( float x_, float y_ )
	{
		x = x_;
		y = y_;
	}
};

struct FontVertex_t
{
	Vector2D m_Position;
	Vector2D m_TexCoord;

	FontVertex_t()
	{}

	FontVertex_t( const Vector2D& pos, const Vector2D& coord = Vector2D( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	void Init( const Vector2D& pos, const Vector2D& coord = Vector2D( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
};

typedef FontVertex_t Vertex_t;

struct surfacephysicsparams_t
{
	float friction;
	float elasticity;
	float density;
	float thickness;
	float dampening;
};

struct surfaceaudioparams_t
{
	float reflectivity;
	float hardnessFactor;
	float roughnessFactor;
	float roughThreshold;
	float hardThreshold;
	float hardVelocityThreshold;
};

struct surfacesoundnames_t
{
	unsigned short walkStepLeft;
	unsigned short walkStepRight;
	unsigned short runStepLeft;
	unsigned short runStepRight;
	unsigned short impactSoft;
	unsigned short impactHard;
	unsigned short scrapeSmooth;
	unsigned short scrapeRough;
	unsigned short bulletImpact;
	unsigned short rolling;
	unsigned short breakSound;
	unsigned short strainSound;
};

struct surfacesoundhandles_t
{
	short walkStepLeft;
	short walkStepRight;
	short runStepLeft;
	short runStepRight;
	short impactSoft;
	short impactHard;
	short scrapeSmooth;
	short scrapeRough;
	short bulletImpact;
	short rolling;
	short breakSound;
	short strainSound;
};

struct surfacegameprops_t
{
public:
	float maxSpeedFactor;
	float jumpFactor;
	char pad00[ 0x4 ];
	float flPenetrationModifier;
	float flDamageModifier;
	unsigned short material;
	char pad01[ 0x3 ];
};

struct surfacedata_t
{
	surfacephysicsparams_t physics;
	surfaceaudioparams_t audio;
	surfacesoundnames_t sounds;
	surfacegameprops_t game;
	surfacesoundhandles_t soundhandles;
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

#define MAX_SPLITSCREEN_CLIENT_BITS 2
// this should == MAX_JOYSTICKS in InputEnums.h
#define MAX_SPLITSCREEN_CLIENTS	( 1 << MAX_SPLITSCREEN_CLIENT_BITS ) // 4

enum
{
	MAX_JOYSTICKS = MAX_SPLITSCREEN_CLIENTS,
	MOUSE_BUTTON_COUNT = 5,
};

enum JoystickAxis_t
{
	JOY_AXIS_X = 0,
	JOY_AXIS_Y,
	JOY_AXIS_Z,
	JOY_AXIS_R,
	JOY_AXIS_U,
	JOY_AXIS_V,
	MAX_JOYSTICK_AXES,
};

enum
{
	JOYSTICK_MAX_BUTTON_COUNT = 32,
	JOYSTICK_POV_BUTTON_COUNT = 4,
	JOYSTICK_AXIS_BUTTON_COUNT = MAX_JOYSTICK_AXES * 2,
};

#define JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_BUTTON + ((_joystick) * JOYSTICK_MAX_BUTTON_COUNT) + (_button) )
#define JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_POV_BUTTON + ((_joystick) * JOYSTICK_POV_BUTTON_COUNT) + (_button) )
#define JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) ( JOYSTICK_FIRST_AXIS_BUTTON + ((_joystick) * JOYSTICK_AXIS_BUTTON_COUNT) + (_button) )

#define JOYSTICK_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_POV_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_POV_BUTTON_INTERNAL( _joystick, _button ) )
#define JOYSTICK_AXIS_BUTTON( _joystick, _button ) ( (ButtonCode_t)JOYSTICK_AXIS_BUTTON_INTERNAL( _joystick, _button ) )

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP, // A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN, // A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,

	// Joystick
	JOYSTICK_FIRST = MOUSE_LAST + 1,

	JOYSTICK_FIRST_BUTTON = JOYSTICK_FIRST,
	JOYSTICK_LAST_BUTTON = JOYSTICK_BUTTON_INTERNAL( MAX_JOYSTICKS - 1, JOYSTICK_MAX_BUTTON_COUNT - 1 ),
	JOYSTICK_FIRST_POV_BUTTON,
	JOYSTICK_LAST_POV_BUTTON = JOYSTICK_POV_BUTTON_INTERNAL( MAX_JOYSTICKS - 1, JOYSTICK_POV_BUTTON_COUNT - 1 ),
	JOYSTICK_FIRST_AXIS_BUTTON,
	JOYSTICK_LAST_AXIS_BUTTON = JOYSTICK_AXIS_BUTTON_INTERNAL( MAX_JOYSTICKS - 1, JOYSTICK_AXIS_BUTTON_COUNT - 1 ),

	JOYSTICK_LAST = JOYSTICK_LAST_AXIS_BUTTON,

	BUTTON_CODE_LAST,
	BUTTON_CODE_COUNT = BUTTON_CODE_LAST - KEY_FIRST + 1,

	// Helpers for XBox 360
	KEY_XBUTTON_UP = JOYSTICK_FIRST_POV_BUTTON, // POV buttons
	KEY_XBUTTON_RIGHT,
	KEY_XBUTTON_DOWN,
	KEY_XBUTTON_LEFT,

	KEY_XBUTTON_A = JOYSTICK_FIRST_BUTTON, // Buttons
	KEY_XBUTTON_B,
	KEY_XBUTTON_X,
	KEY_XBUTTON_Y,
	KEY_XBUTTON_LEFT_SHOULDER,
	KEY_XBUTTON_RIGHT_SHOULDER,
	KEY_XBUTTON_BACK,
	KEY_XBUTTON_START,
	KEY_XBUTTON_STICK1,
	KEY_XBUTTON_STICK2,
	KEY_XBUTTON_INACTIVE_START,

	KEY_XSTICK1_RIGHT = JOYSTICK_FIRST_AXIS_BUTTON, // XAXIS POSITIVE
	KEY_XSTICK1_LEFT, // XAXIS NEGATIVE
	KEY_XSTICK1_DOWN, // YAXIS POSITIVE
	KEY_XSTICK1_UP, // YAXIS NEGATIVE
	KEY_XBUTTON_LTRIGGER, // ZAXIS POSITIVE
	KEY_XBUTTON_RTRIGGER, // ZAXIS NEGATIVE
	KEY_XSTICK2_RIGHT, // UAXIS POSITIVE
	KEY_XSTICK2_LEFT, // UAXIS NEGATIVE
	KEY_XSTICK2_DOWN, // VAXIS POSITIVE
	KEY_XSTICK2_UP, // VAXIS NEGATIVE
};

#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

#define BONE_CALCULATE_MASK			0x1F
#define BONE_PHYSICALLY_SIMULATED	0x01	// bone is physically simulated when physics are active
#define BONE_PHYSICS_PROCEDURAL		0x02	// procedural when physics is active
#define BONE_ALWAYS_PROCEDURAL		0x04	// bone is always procedurally animated
#define BONE_SCREEN_ALIGN_SPHERE	0x08	// bone aligns to the screen, not constrained in motion.
#define BONE_SCREEN_ALIGN_CYLINDER	0x10	// bone aligns to the screen, constrained by it's own axis.

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800
#define BONE_USED_BY_VERTEX_LOD2	0x00001000
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it

#define BONE_USED_BY_VERTEX_AT_LOD(lod) ( BONE_USED_BY_VERTEX_LOD0 << (lod) )
#define BONE_USED_BY_ANYTHING_AT_LOD(lod) ( ( BONE_USED_BY_ANYTHING & ~BONE_USED_BY_VERTEX_MASK ) | BONE_USED_BY_VERTEX_AT_LOD(lod) )

#define MAX_NUM_LODS 8

#define BONE_TYPE_MASK				0x00F00000
#define BONE_FIXED_ALIGNMENT		0x00100000	// bone can't spin 360 degrees, all interpolation is normalized around a fixed orientation

#define BONE_HAS_SAVEFRAME_POS		0x00200000	// Vector48
#define BONE_HAS_SAVEFRAME_ROT64	0x00400000	// Quaternion64
#define BONE_HAS_SAVEFRAME_ROT32	0x00800000	// Quaternion32

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10			// alerts NPC, but doesn't do damage or bleed (1/100th damage)

struct model_t
{
	char name[ 255 ];
};

typedef unsigned short ModelInstanceHandle_t;

struct ModelRenderInfo_t
{
	Vector origin;
	QAngle angles;
	void* pRenderable;
	const model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector* pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = NULL;
		pLightingOffset = NULL;
		pLightingOrigin = NULL;
	}
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0, // bool
	MATERIAL_PROPERTY_OPACITY, // int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY, // vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS // bool
};

enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F, // Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F, // NOTE: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL, // Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N, // One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N, // Two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102, // 10 bit-per component render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F, // 16 bit FP format

	// Depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW, // Specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW, // Specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	NUM_IMAGE_FORMATS
};

class IHandleEntity
{
public:
	virtual ~IHandleEntity()
	{}

	virtual void SetRefEHandle() = 0;
	virtual void GetRefEHandle() const = 0;
};

enum Collision_Group_t
{
	COLLISION_GROUP_NONE = 0,
	COLLISION_GROUP_DEBRIS, // Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS, // Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE, // Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT, // For HL2, same as Collision_Group_Player, for
	// TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC, // Generic NPC group
	COLLISION_GROUP_IN_VEHICLE, // for any entity inside a vehicle
	COLLISION_GROUP_WEAPON, // for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP, // vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE, // Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER, // Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR, // Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING, // Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY, // Nonsolid on client and server, pushaway in player code

	COLLISION_GROUP_NPC_ACTOR, // Used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED, // USed for NPCs in scripts that should not collide with each other
	COLLISION_GROUP_PZ_CLIP,

	COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE, // Only collides with bullets

	LAST_SHARED_COLLISION_GROUP
};

struct string_t
{
public:
	bool operator!() const
	{
		return ( pszValue == NULL );
	}

	bool operator==( const string_t& rhs ) const
	{
		return ( pszValue == rhs.pszValue );
	}

	bool operator!=( const string_t& rhs ) const
	{
		return ( pszValue != rhs.pszValue );
	}

	bool operator<( const string_t& rhs ) const
	{
		return ( ( void * )pszValue < ( void * )rhs.pszValue );
	}

	const char* ToCStr() const
	{
		return ( pszValue ) ? pszValue : "";
	}

protected:
	const char* pszValue;
};

#define PHYSICS_MULTIPLAYER_AUTODETECT	0	// use multiplayer physics mode as defined in model prop data
#define PHYSICS_MULTIPLAYER_SOLID		1	// soild, pushes player away 
#define PHYSICS_MULTIPLAYER_NON_SOLID	2	// nonsolid, but pushed by player
#define PHYSICS_MULTIPLAYER_CLIENTSIDE	3	// Clientside only, nonsolid 	

class IMultiplayerPhysics
{
public:
	virtual int GetMultiplayerPhysicsMode() = 0;
	virtual float GetMass() = 0;
	virtual bool IsAsleep() = 0;
};

enum propdata_interactions_t
{
	PROPINTER_PHYSGUN_WORLD_STICK, // "onworldimpact"	"stick"
	PROPINTER_PHYSGUN_FIRST_BREAK, // "onfirstimpact"	"break"
	PROPINTER_PHYSGUN_FIRST_PAINT, // "onfirstimpact"	"paintsplat"
	PROPINTER_PHYSGUN_FIRST_IMPALE, // "onfirstimpact"	"impale"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_NONE, // "onlaunch"		"spin_none"
	PROPINTER_PHYSGUN_LAUNCH_SPIN_Z, // "onlaunch"		"spin_zaxis"
	PROPINTER_PHYSGUN_BREAK_EXPLODE, // "onbreak"		"explode_fire"
	PROPINTER_PHYSGUN_BREAK_EXPLODE_ICE, // "onbreak"	"explode_ice"
	PROPINTER_PHYSGUN_DAMAGE_NONE, // "damage"			"none"

	PROPINTER_FIRE_FLAMMABLE, // "flammable"			"yes"
	PROPINTER_FIRE_EXPLOSIVE_RESIST, // "explosive_resist"	"yes"
	PROPINTER_FIRE_IGNITE_HALFHEALTH, // "ignite"				"halfhealth"

	PROPINTER_PHYSGUN_CREATE_FLARE, // "onpickup"		"create_flare"

	PROPINTER_PHYSGUN_ALLOW_OVERHEAD, // "allow_overhead"	"yes"

	PROPINTER_WORLD_BLOODSPLAT, // "onworldimpact", "bloodsplat"

	PROPINTER_PHYSGUN_NOTIFY_CHILDREN, // "onfirstimpact" cause attached flechettes to explode
	PROPINTER_MELEE_IMMUNE, // "melee_immune"	"yes"

	// If we get more than 32 of these, we'll need a different system

	PROPINTER_NUM_INTERACTIONS,
};

enum mp_break_t
{
	MULTIPLAYER_BREAK_DEFAULT,
	MULTIPLAYER_BREAK_SERVERSIDE,
	MULTIPLAYER_BREAK_CLIENTSIDE,
	MULTIPLAYER_BREAK_BOTH
};

class IBreakableWithPropData
{
public:
	// Damage modifiers
	virtual void SetDmgModBullet( float flDmgMod ) = 0;
	virtual void SetDmgModClub( float flDmgMod ) = 0;
	virtual void SetDmgModExplosive( float flDmgMod ) = 0;
	virtual float GetDmgModBullet( void ) = 0;
	virtual float GetDmgModClub( void ) = 0;
	virtual float GetDmgModExplosive( void ) = 0;
	virtual float GetDmgModFire( void ) = 0;

	// Explosive
	virtual void SetExplosiveRadius( float flRadius ) = 0;
	virtual void SetExplosiveDamage( float flDamage ) = 0;
	virtual float GetExplosiveRadius( void ) = 0;
	virtual float GetExplosiveDamage( void ) = 0;

	// Physics damage tables
	virtual void SetPhysicsDamageTable( string_t iszTableName ) = 0;
	virtual string_t GetPhysicsDamageTable( void ) = 0;

	// Breakable chunks
	virtual void SetBreakableModel( string_t iszModel ) = 0;
	virtual string_t GetBreakableModel( void ) = 0;
	virtual void SetBreakableSkin( int iSkin ) = 0;
	virtual int GetBreakableSkin( void ) = 0;
	virtual void SetBreakableCount( int iCount ) = 0;
	virtual int GetBreakableCount( void ) = 0;
	virtual void SetMaxBreakableSize( int iSize ) = 0;
	virtual int GetMaxBreakableSize( void ) = 0;

	// LOS blocking
	virtual void SetPropDataBlocksLOS( bool bBlocksLOS ) = 0;
	virtual void SetPropDataIsAIWalkable( bool bBlocksLOS ) = 0;

	// Interactions
	virtual void SetInteraction( propdata_interactions_t Interaction ) = 0;
	virtual bool HasInteraction( propdata_interactions_t Interaction ) = 0;

	// Multiplayer physics mode
	virtual void SetPhysicsMode( int iMode ) = 0;
	virtual int GetPhysicsMode() = 0;

	// Multiplayer breakable spawn behavior
	virtual void SetMultiplayerBreakMode( mp_break_t mode ) = 0;
	virtual mp_break_t GetMultiplayerBreakMode( void ) const = 0;

	// Used for debugging
	virtual void SetBasePropData( string_t iszBase ) = 0;
	virtual string_t GetBasePropData( void ) = 0;
};

#define FCVAR_NONE				0

// Command to ConVars and ConCommands
// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 
// Note: IVEngineClient::ClientCmd_Unrestricted can run any client command.

// #define FCVAR_AVAILABLE			(1<<15)
// #define FCVAR_AVAILABLE			(1<<18)
// #define FCVAR_AVAILABLE			(1<<19)
// #define FCVAR_AVAILABLE			(1<<20)
// #define FCVAR_AVAILABLE			(1<<21)
// #define FCVAR_AVAILABLE			(1<<23)
// #define FCVAR_AVAILABLE			(1<<26)
// #define FCVAR_AVAILABLE			(1<<27)
// #define FCVAR_AVAILABLE			(1<<31)

#define FCVAR_MATERIAL_THREAD_MASK ( FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD )

struct FileHandle_t;

class KeyValues
{
public:
	//	By default, the KeyValues class uses a string table for the key names that is
	//	limited to 4MB. The game will exit in error if this space is exhausted. In
	//	general this is preferable for game code for performance and memory fragmentation
	//	reasons.
	//
	//	If this is not acceptable, you can use this call to switch to a table that can grow
	//	arbitrarily. This call must be made before any KeyValues objects are allocated or it
	//	will result in undefined behavior. If you use the growable string table, you cannot
	//	share KeyValues pointers directly with any other module. You can serialize them across
	//	module boundaries. These limitations are acceptable in the Steam backend code 
	//	this option was written for, but may not be in other situations. Make sure to
	//	understand the implications before using this.
	static void SetUseGrowableStringTable( bool bUseGrowableTable );

	KeyValues( const char* setName )
	{}

	//
	// AutoDelete class to automatically free the keyvalues.
	// Simply construct it with the keyvalues you allocated and it will free them when falls out of scope.
	// When you decide that keyvalues shouldn't be deleted call Assign(NULL) on it.
	// If you constructed AutoDelete(NULL) you can later assign the keyvalues to be deleted with Assign(pKeyValues).
	// You can also pass temporary KeyValues object as an argument to a function by wrapping it into KeyValues::AutoDelete
	// instance:   call_my_function( KeyValues::AutoDelete( new KeyValues( "test" ) ) )
	//
	class AutoDelete
	{
	public:
		explicit inline AutoDelete( KeyValues* pKeyValues )
			: m_pKeyValues( pKeyValues )
		{}

		explicit inline AutoDelete( const char* pchKVName )
			: m_pKeyValues( new KeyValues( pchKVName ) )
		{}

		inline ~AutoDelete( void )
		{
			if( m_pKeyValues )
				m_pKeyValues->deleteThis();
		}

		inline void Assign( KeyValues* pKeyValues )
		{
			m_pKeyValues = pKeyValues;
		}

		KeyValues* operator->()
		{
			return m_pKeyValues;
		}

		operator KeyValues *()
		{
			return m_pKeyValues;
		}

	private:
		AutoDelete( AutoDelete const& x ); // forbid
		AutoDelete& operator=( AutoDelete const& x ); // forbid
		KeyValues* m_pKeyValues;
	};

	// Quick setup constructors
	KeyValues( const char* setName, const char* firstKey, const char* firstValue );
	KeyValues( const char* setName, const char* firstKey, const wchar_t* firstValue );
	KeyValues( const char* setName, const char* firstKey, int firstValue );
	KeyValues( const char* setName, const char* firstKey, const char* firstValue, const char* secondKey, const char* secondValue );
	KeyValues( const char* setName, const char* firstKey, int firstValue, const char* secondKey, int secondValue );

	// Section name
	const char* GetName() const;
	void SetName( const char* setName );

	// gets the name as a unique int
	int GetNameSymbol() const
	{
		return m_iKeyName;
	}

	// File access. Set UsesEscapeSequences true, if resource file/buffer uses Escape Sequences (eg \n, \t)
	void UsesEscapeSequences( bool state ); // default false
	void UsesConditionals( bool state ); // default true
	bool LoadFromFile( void* filesystem, const char* resourceName, const char* pathID = NULL );
	bool SaveToFile( void* filesystem, const char* resourceName, const char* pathID = NULL, bool sortKeys = false, bool bAllowEmptyString = false );

	// Read from a buffer...  Note that the buffer must be null terminated
	bool LoadFromBuffer( char const* resourceName, const char* pBuffer, void* pFileSystem = NULL, const char* pPathID = NULL );

	// Read from a utlbuffer...
	bool LoadFromBuffer( char const* resourceName, void* buf, void* pFileSystem = NULL, const char* pPathID = NULL );

	// Find a keyValue, create it if it is not found.
	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
	KeyValues* FindKey( const char* keyName, bool bCreate = false );
	KeyValues* FindKey( int keySymbol ) const;
	KeyValues* CreateNewKey(); // creates a new key, with an autogenerated name.  name is guaranteed to be an integer, of value 1 higher than the highest other integer key name
	void AddSubKey( KeyValues* pSubkey ); // Adds a subkey. Make sure the subkey isn't a child of some other keyvalues
	void RemoveSubKey( KeyValues* subKey ); // removes a subkey from the list, DOES NOT DELETE IT

	// Key iteration.
	//
	// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
	// below if you want to iterate over just the keys or just the values.
	//
	KeyValues* GetFirstSubKey()
	{
		return m_pSub;
	} // returns the first subkey in the list
	KeyValues* GetNextKey()
	{
		return m_pPeer;
	} // returns the next subkey
	void SetNextKey( KeyValues* pDat );
	KeyValues* FindLastSubKey(); // returns the LAST subkey in the list.  This requires a linked list iteration to find the key.  Returns NULL if we don't have any children

	//
	// These functions can be used to treat it like a true key/values tree instead of 
	// confusing values with keys.
	//
	// So if you wanted to iterate all subkeys, then all values, it would look like this:
	//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
	//     {
	//		   Msg( "Key name: %s\n", pKey->GetName() );
	//     }
	//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
	//     {
	//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
	//     }
	KeyValues* GetFirstTrueSubKey();
	KeyValues* GetNextTrueSubKey();

	KeyValues* GetFirstValue(); // When you get a value back, you can use GetX and pass in NULL to get the value.
	KeyValues* GetNextValue();

	// Data access
	int GetInt( const char* keyName = NULL, int defaultValue = 0 );
	uint64 GetUint64( const char* keyName = NULL, uint64 defaultValue = 0 );
	float GetFloat( const char* keyName = NULL, float defaultValue = 0.0f );
	const char* GetString( const char* keyName = NULL, const char* defaultValue = "" );
	const wchar_t* GetWString( const char* keyName = NULL, const wchar_t* defaultValue = L"" );
	void* GetPtr( const char* keyName = NULL, void* defaultValue = ( void* )0 );
	bool GetBool( const char* keyName = NULL, bool defaultValue = false );
	Color GetColor( const char* keyName = NULL /* default value is all black */ );
	bool IsEmpty( const char* keyName = NULL );

	// Data access
	int GetInt( int keySymbol, int defaultValue = 0 );
	float GetFloat( int keySymbol, float defaultValue = 0.0f );
	const char* GetString( int keySymbol, const char* defaultValue = "" );
	const wchar_t* GetWString( int keySymbol, const wchar_t* defaultValue = L"" );
	void* GetPtr( int keySymbol, void* defaultValue = ( void* )0 );
	Color GetColor( int keySymbol /* default value is all black */ );
	bool IsEmpty( int keySymbol );

	// Key writing
	void SetWString( const char* keyName, const wchar_t* value );
	void SetString( const char* keyName, const char* value );
	void SetInt( const char* keyName, int value );
	void SetUint64( const char* keyName, uint64 value );
	void SetFloat( const char* keyName, float value );
	void SetPtr( const char* keyName, void* value );
	void SetColor( const char* keyName, Color value );

	void SetBool( const char* keyName, bool value )
	{
		SetInt( keyName, value ? 1 : 0 );
	}

	// Adds a chain... if we don't find stuff in this keyvalue, we'll look
	// in the one we're chained to.
	void ChainKeyValue( KeyValues* pChain );

	void RecursiveSaveToFile( void* buf, int indentLevel, bool sortKeys = false, bool bAllowEmptyString = false );

	bool WriteAsBinary( void* buffer );
	bool ReadAsBinary( void* buffer, int nStackDepth = 0 );

	// Allocate & create a new copy of the keys
	KeyValues* MakeCopy( void ) const;

	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
	void CopySubkeys( KeyValues* pParent ) const;

	// Clear out all subkeys, and the current value
	void Clear( void );

	// Data type
	enum types_t
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_NUMTYPES,
	};

	types_t GetDataType( const char* keyName = NULL );

	// Virtual deletion function - ensures that KeyValues object is deleted from correct heap
	void deleteThis();

	void SetStringValue( char const* strValue );

	// unpack a key values list into a structure
	void UnpackIntoStructure( struct KeyValuesUnpackStructure const* pUnpackTable, void* pDest, size_t DestSizeInBytes );

	// Process conditional keys for widescreen support.
	bool ProcessResolutionKeys( const char* pResString );

	// Dump keyvalues recursively into a dump context
	bool Dump( class IKeyValuesDumpContext* pDump, int nIndentLevel = 0 );

	// Merge in another KeyValues, keeping "our" settings
	void RecursiveMergeKeyValues( KeyValues* baseKV );

private:
	KeyValues( KeyValues& ); // prevent copy constructor being used

	// prevent delete being called except through deleteThis()
	~KeyValues();

	KeyValues* CreateKey( const char* keyName );

	/// Create a child key, given that we know which child is currently the last child.
	/// This avoids the O(N^2) behaviour when adding children in sequence to KV,
	/// when CreateKey() wil have to re-locate the end of the list each time.  This happens,
	/// for example, every time we load any KV file whatsoever.
	KeyValues* CreateKeyUsingKnownLastChild( const char* keyName, KeyValues* pLastChild );
	void AddSubkeyUsingKnownLastChild( KeyValues* pSubKey, KeyValues* pLastChild );

	void RecursiveCopyKeyValues( KeyValues& src );
	void RemoveEverything();
	//	void RecursiveSaveToFile( IBaseFileSystem *filesystem, void*buffer, int indentLevel );
	//	void WriteConvertedString( void*buffer, const char *pszString );

	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
	// If filesystem is null, it'll ignore f.
	void RecursiveSaveToFile( void* filesystem, FileHandle_t f, void* pBuf, int indentLevel, bool sortKeys, bool bAllowEmptyString );
	void SaveKeyToFile( KeyValues* dat, void* filesystem, FileHandle_t f, void* pBuf, int indentLevel, bool sortKeys, bool bAllowEmptyString );
	void WriteConvertedString( void* filesystem, FileHandle_t f, void* pBuf, const char* pszString );

	void RecursiveLoadFromBuffer( char const* resourceName, void* buf );

	// For handling #include "filename"
	void AppendIncludedKeys( void* includedKeys );
	void ParseIncludedKeys( char const* resourceName, const char* filetoinclude,
	                        void* pFileSystem, const char* pPathID, void* includedKeys );

	// For handling #base "filename"
	void MergeBaseKeys( void* baseKeys );

	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
	// If filesystem is null, it'll ignore f.
	void InternalWrite( void* filesystem, FileHandle_t f, void* pBuf, const void* pData, int len );

	void Init();
	const char* ReadToken( void* buf, bool& wasQuoted, bool& wasConditional );
	void WriteIndents( void* filesystem, FileHandle_t f, void* pBuf, int indentLevel );

	void FreeAllocatedValue();
	void AllocateValueBlock( int size );

	int m_iKeyName; // keyname is a symbol defined in KeyValuesSystem

	// These are needed out of the union because the API returns string pointers
	char* m_sValue;
	wchar_t* m_wsValue;

	// we don't delete these
	union
	{
		int m_iValue;
		float m_flValue;
		void* m_pValue;
		unsigned char m_Color[ 4 ];
	};

	char m_iDataType;
	char m_bHasEscapeSequences; // true, if while parsing this KeyValue, Escape Sequences are used (default false)
	char m_bEvaluateConditionals; // true, if while parsing this KeyValue, conditionals blocks are evaluated (default true)
	char unused[ 1 ];

	KeyValues* m_pPeer; // pointer to next key in list
	KeyValues* m_pSub; // pointer to Start of a new sub key list
	KeyValues* m_pChain;// Search here if it's not in our list

private:
	// Statics to implement the optional growable string table
	// Function pointers that will determine which mode we are in
	static int ( *s_pfGetSymbolForString )( const char* name, bool bCreate );
	static const char*( *s_pfGetStringForSymbol )( int symbol );
	static void* s_pGrowableStringTable;

public:
	// Functions that invoke the default behavior
	static int GetSymbolForStringClassic( const char* name, bool bCreate = true );
	static const char* GetStringForSymbolClassic( int symbol );

	// Functions that use the growable string table
	static int GetSymbolForStringGrowable( const char* name, bool bCreate = true );
	static const char* GetStringForSymbolGrowable( int symbol );

	// Functions to get external access to whichever of the above functions we're going to call.
	static int CallGetSymbolForString( const char* name, bool bCreate = true )
	{
		return s_pfGetSymbolForString( name, bCreate );
	}

	static const char* CallGetStringForSymbol( int symbol )
	{
		return s_pfGetStringForSymbol( symbol );
	}
};

struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};
