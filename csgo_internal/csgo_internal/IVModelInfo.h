#pragma once

enum Hitboxes
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IMaterial;
struct vcollide_t;
struct model_t;
class Vector;
class QAngle;
class CGameTrace;
struct cplane_t;
typedef CGameTrace trace_t;
struct studiohdr_t;
struct virtualmodel_t;
typedef unsigned char byte;
struct virtualterrainparams_t;
class CPhysCollide;
typedef unsigned short MDLHandle_t;
class CUtlBuffer;
class IClientRenderable;
class Quaternion;
struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct mstudiobodyparts_t;
struct mstudiotexture_t;

class RadianEuler
{
public:
	inline RadianEuler( void )
	{ }

	inline RadianEuler( float X, float Y, float Z )
	{
		x = X;
		y = Y;
		z = Z;
	}

	inline RadianEuler( Quaternion const& q ); // evil auto type promotion!!!
	inline RadianEuler( QAngle const& angles ); // evil auto type promotion!!!

	// Initialization
	inline void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f )
	{
		x = ix;
		y = iy;
		z = iz;
	}

	//	conversion to qangle
	QAngle ToQAngle( void ) const;
	bool IsValid() const;
	void Invalidate();

	inline float* Base()
	{
		return &x;
	}

	inline const float* Base() const
	{
		return &x;
	}

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	float x, y, z;
};

class Quaternion // same data-layout as engine's vec4_t,
{ //		which is a float[4]
public:
	inline Quaternion( void )
	{}

	inline Quaternion( float ix, float iy, float iz, float iw )
		: x( ix ),
		  y( iy ),
		  z( iz ),
		  w( iw )
	{ }

	inline Quaternion( RadianEuler const& angle ); // evil auto type promotion!!!

	inline void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f )
	{
		x = ix;
		y = iy;
		z = iz;
		w = iw;
	}

	bool IsValid() const;
	void Invalidate();

	bool operator==( const Quaternion& src ) const;
	bool operator!=( const Quaternion& src ) const;

	float* Base()
	{
		return ( float* )this;
	}

	const float* Base() const
	{
		return ( float* )this;
	}

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	float x, y, z, w;
};

struct mstudiohitboxset_t
{
	int sznameindex;

	inline char* pszName( void ) const
	{
		return ( ( char* )this ) + sznameindex;
	}

	int numhitboxes;
	int hitboxindex;

	inline mstudiobbox_t* pHitbox( int i ) const
	{
		return ( mstudiobbox_t* )( ( ( byte* )this ) + hitboxindex ) + i;
	}
};

struct mstudiobone_t
{
	int sznameindex;

	inline char* pszName( void ) const
	{
		return ( ( char * )this ) + sznameindex;
	}

	int parent; // parent bone
	int bonecontroller[ 6 ]; // bone controller index, -1 == none

	// default values
	Vector pos;
	Quaternion quat;
	RadianEuler rot;
	// compression scale
	Vector posscale;
	Vector rotscale;

	matrix3x4_t poseToBone;
	Quaternion qAlignment;
	int flags;
	int proctype;
	int procindex; // procedural rule
	mutable int physicsbone; // index into physically simulated bone
	inline void* pProcedure() const
	{
		if( procindex == 0 )
			return NULL;
		else
			return ( void * )( ( ( byte * )this ) + procindex );
	};
	int surfacepropidx; // index into string tablefor property name
	inline char* pszSurfaceProp( void ) const
	{
		return ( ( char * )this ) + surfacepropidx;
	}

	inline int GetSurfaceProp( void ) const
	{
		return surfacepropLookup;
	}

	int contents; // See BSPFlags.h for the contents flags
	int surfacepropLookup; // this index must be cached by the loader, not saved in the file
	int unused[ 7 ]; // remove as appropriate
};

struct mstudiobonecontroller_t
{
	int bone; // -1 == 0
	int type; // X, Y, Z, XR, YR, ZR, M
	float start;
	float end;
	int rest; // byte index value at rest
	int inputfield; // 0-3 user set controller, 4 mouth
	int unused[ 8 ];
};

struct studiohdr_t
{
	int id;
	int version;

	long checksum; // this has to be the same in the phy and vtx files to load!

	inline const char* pszName( void ) const
	{
		return name;
	}

	char name[ 64 ];

	int length;

	Vector eyeposition; // ideal eye position

	Vector illumposition; // illumination center

	Vector hull_min; // ideal movement hull size
	Vector hull_max;

	Vector view_bbmin; // clipping bounding box
	Vector view_bbmax;

	int flags;

	int numbones; // bones
	int boneindex;

	inline mstudiobone_t* pBone( int i ) const
	{
		Assert( i >= 0 && i < numbones );
		return ( mstudiobone_t * )( ( ( byte * )this ) + boneindex ) + i;
	};
	int RemapSeqBone( int iSequence, int iLocalBone ) const; // maps local sequence bone to global bone
	int RemapAnimBone( int iAnim, int iLocalBone ) const; // maps local animations bone to global bone

	int numbonecontrollers; // bone controllers
	int bonecontrollerindex;

	inline mstudiobonecontroller_t* pBonecontroller( int i ) const
	{
		Assert( i >= 0 && i < numbonecontrollers );
		return ( mstudiobonecontroller_t * )( ( ( byte * )this ) + bonecontrollerindex ) + i;
	};

	int numhitboxsets;
	int hitboxsetindex;

	// Look up hitbox set by index
	mstudiohitboxset_t* pHitboxSet( int i ) const
	{
		Assert( i >= 0 && i < numhitboxsets );
		return ( mstudiohitboxset_t * )( ( ( byte * )this ) + hitboxsetindex ) + i;
	};

	// Calls through to hitbox to determine size of specified set
	inline mstudiobbox_t* pHitbox( int i, int set ) const
	{
		mstudiohitboxset_t const* s = pHitboxSet( set );
		if( !s )
			return NULL;

		return s->pHitbox( i );
	};

	// Calls through to set to get hitbox count for set
	inline int iHitboxCount( int set ) const
	{
		mstudiohitboxset_t const* s = pHitboxSet( set );
		if( !s )
			return 0;

		return s->numhitboxes;
	};

	// file local animations? and sequences
	//private:
	int numlocalanim; // animations/poses
	int localanimindex; // animation descriptions
	inline mstudioanimdesc_t* pLocalAnimdesc( int i ) const
	{
		return NULL;
	};

	int numlocalseq; // sequences
	int localseqindex;

	inline mstudioseqdesc_t* pLocalSeqdesc( int i ) const
	{
		return NULL;
	};

	//public:
	bool SequencesAvailable() const
	{
		return true;
	}

	int GetNumSeq() const;
	mstudioanimdesc_t& pAnimdesc( int i ) const;
	mstudioseqdesc_t& pSeqdesc( int i ) const;
	int iRelativeAnim( int baseseq, int relanim ) const; // maps seq local anim reference to global anim index
	int iRelativeSeq( int baseseq, int relseq ) const; // maps seq local seq reference to global seq index

	//private:
	mutable int activitylistversion; // initialization flag - have the sequences been indexed?
	mutable int eventsindexed;
	//public:
	int GetSequenceActivity( int iSequence );
	void SetSequenceActivity( int iSequence, int iActivity );
	int GetActivityListVersion( void );
	void SetActivityListVersion( int version ) const;
	int GetEventListVersion( void );
	void SetEventListVersion( int version );

	// raw textures
	int numtextures;
	int textureindex;

	inline mstudiotexture_t* pTexture( int i ) const
	{
		return NULL;
	};

	// raw textures search paths
	int numcdtextures;
	int cdtextureindex;

	inline char* pCdtexture( int i ) const
	{
		return ( ( ( char * )this ) + *( ( int * )( ( ( byte * )this ) + cdtextureindex ) + i ) );
	};

	// replaceable textures tables
	int numskinref;
	int numskinfamilies;
	int skinindex;

	inline short* pSkinref( int i ) const
	{
		return ( short * )( ( ( byte * )this ) + skinindex ) + i;
	};

	int numbodyparts;
	int bodypartindex;

	inline mstudiobodyparts_t* pBodypart( int i ) const
	{
		return NULL;
	};
};

//-----------------------------------------------------------------------------
// Indicates the type of translucency of an unmodulated renderable
//-----------------------------------------------------------------------------
enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS, // has both translucent and opaque sub-partsa
};

//-----------------------------------------------------------------------------
// Model info interface
//-----------------------------------------------------------------------------

class IVModelInfo
{
public:
	virtual ~IVModelInfo( void )
	{ }

	virtual const model_t* GetModel( int modelindex ) const = 0;
	// Returns index of model by name
	virtual int GetModelIndex( const char* name ) const = 0;
	// Returns name of model
	virtual const char* GetModelName( const model_t* model ) const = 0;
	virtual vcollide_t* GetVCollide( const model_t* model ) const = 0;
	virtual vcollide_t* GetVCollide( int modelindex ) const = 0;
	virtual void GetModelBounds( const model_t* model, Vector& mins, Vector& maxs ) const = 0;
	virtual void GetModelRenderBounds( const model_t* model, Vector& mins, Vector& maxs ) const = 0;
	virtual int GetModelFrameCount( const model_t* model ) const = 0;
	virtual int GetModelType( const model_t* model ) const = 0;
	virtual void* GetModelExtraData( const model_t* model ) = 0;
	virtual bool ModelHasMaterialProxy( const model_t* model ) const = 0;
	virtual bool IsTranslucent( model_t const* model ) const = 0;
	virtual bool IsTranslucentTwoPass( const model_t* model ) const = 0;

	virtual void Unused0()
	{};
	virtual RenderableTranslucencyType_t ComputeTranslucencyType( const model_t* model, int nSkin, int nBody ) = 0;
	virtual int GetModelMaterialCount( const model_t* model ) const = 0;
	virtual void GetModelMaterials( const model_t* model, int count, IMaterial** ppMaterials ) = 0;
	virtual bool IsModelVertexLit( const model_t* model ) const = 0;
	virtual const char* GetModelKeyValueText( const model_t* model ) = 0;
	virtual bool GetModelKeyValue( const model_t* model, CUtlBuffer& buf ) = 0; // supports keyvalue blocks in submodels
	virtual float GetModelRadius( const model_t* model ) = 0;

	virtual const studiohdr_t* FindModel( const studiohdr_t* pStudioHdr, void** cache, const char* modelname ) const = 0;
	virtual const studiohdr_t* FindModel( void* cache ) const = 0;
	virtual virtualmodel_t* GetVirtualModel( const studiohdr_t* pStudioHdr ) const = 0;
	virtual byte* GetAnimBlock( const studiohdr_t* pStudioHdr, int iBlock ) const = 0;
	virtual bool HasAnimBlockBeenPreloaded( studiohdr_t const*, int ) const = 0;

	// Available on client only!!!
	virtual void GetModelMaterialColorAndLighting( const model_t* model, Vector const& origin,
	                                               QAngle const& angles, trace_t* pTrace,
	                                               Vector& lighting, Vector& matColor ) = 0;
	virtual void GetIlluminationPoint( const model_t* model, IClientRenderable* pRenderable, Vector const& origin,
	                                   QAngle const& angles, Vector* pLightingCenter ) = 0;

	virtual int GetModelContents( int modelIndex ) const = 0;
	virtual studiohdr_t* GetStudioModel( const model_t* mod ) = 0;
	virtual int GetModelSpriteWidth( const model_t* model ) const = 0;
	virtual int GetModelSpriteHeight( const model_t* model ) const = 0;

	// Sets/gets a map-specified fade range (client only)
	virtual void SetLevelScreenFadeRange( float flMinSize, float flMaxSize ) = 0;
	virtual void GetLevelScreenFadeRange( float* pMinArea, float* pMaxArea ) const = 0;

	// Sets/gets a map-specified per-view fade range (client only)
	virtual void SetViewScreenFadeRange( float flMinSize, float flMaxSize ) = 0;

	// Computes fade alpha based on distance fade + screen fade (client only)
	virtual unsigned char ComputeLevelScreenFade( const Vector& vecAbsOrigin, float flRadius, float flFadeScale ) const = 0;
	virtual unsigned char ComputeViewScreenFade( const Vector& vecAbsOrigin, float flRadius, float flFadeScale ) const = 0;

	// both client and server
	virtual int GetAutoplayList( const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList ) const = 0;

	// Gets a virtual terrain collision model (creates if necessary)
	// NOTE: This may return NULL if the terrain model cannot be virtualized
	virtual CPhysCollide* GetCollideForVirtualTerrain( int index ) = 0;
	virtual bool IsUsingFBTexture( const model_t* model, int nSkin, int nBody, void /*IClientRenderable*/ * pClientRenderable ) const = 0;
	virtual const model_t* FindOrLoadModel( const char* name ) const = 0;
	virtual MDLHandle_t GetCacheHandle( const model_t* model ) const = 0;
	// Returns planes of non-nodraw brush model surfaces
	virtual int GetBrushModelPlaneCount( const model_t* model ) const = 0;
	virtual void GetBrushModelPlane( const model_t* model, int nIndex, cplane_t& plane, Vector* pOrigin ) const = 0;
	virtual int GetSurfacepropsForVirtualTerrain( int index ) = 0;
	virtual bool UsesEnvCubemap( const model_t* model ) const = 0;
	virtual bool UsesStaticLighting( const model_t* model ) const = 0;
};

class IVModelInfoClient : public IVModelInfo
{
public:
};

struct virtualterrainparams_t
{
	// UNDONE: Add grouping here, specified in BSP file? (test grouping to see if this is necessary)
	int index;
};
