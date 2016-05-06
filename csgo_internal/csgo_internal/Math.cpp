#include "SDK.h"

void inline M::SinCos( float radians, float *sine, float *cosine )
{
	*sine = sin( radians );
	*cosine = cos( radians );
}

void M::VectorAngles( const Vector& forward, QAngle &angles )
{
	if( forward[ 1 ] == 0.0f && forward[ 0 ] == 0.0f )
	{
		angles[ 0 ] = ( forward[ 2 ] > 0.0f ) ? 270.0f : 90.0f;
		angles[ 1 ] = 0.0f;
	}
	else
	{
		float len2d = sseSqrt( square( forward[ 0 ] ) + square( forward[ 1 ] ) );

		angles[ 0 ] = RAD2DEG( atan2f( -forward[ 2 ], len2d ) );
		angles[ 1 ] = RAD2DEG( atan2f( forward[ 1 ], forward[ 0 ] ) );

		if( angles[ 0 ] < 0.0f ) angles[ 0 ] += 360.0f;
		if( angles[ 1 ] < 0.0f ) angles[ 1 ] += 360.0f;
	}

	angles[ 2 ] = 0.0f;
}

void M::AngleVectors( const QAngle &angles, Vector *forward )
{
	float sp, sy, cp, cy;

	SinCos( DEG2RAD( angles[ YAW ] ), &sy, &cy );
	SinCos( DEG2RAD( angles[ PITCH ] ), &sp, &cp );

	forward->x = cp*cy;
	forward->y = cp*sy;
	forward->z = -sp;
}

void M::AngleVectors( const QAngle &angles, Vector *forward, Vector *right, Vector *up )
{
	float sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( angles[ YAW ] ), &sy, &cy );
	SinCos( DEG2RAD( angles[ PITCH ] ), &sp, &cp );
	SinCos( DEG2RAD( angles[ ROLL ] ), &sr, &cr );

	if( forward )
	{
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	if( right )
	{
		right->x = ( -1 * sr*sp*cy + -1 * cr*-sy );
		right->y = ( -1 * sr*sp*sy + -1 * cr*cy );
		right->z = -1 * sr*cp;
	}

	if( up )
	{
		up->x = ( cr*sp*cy + -sr*-sy );
		up->y = ( cr*sp*sy + -sr*cy );
		up->z = cr*cp;
	}
}

float M::GetFov( const QAngle& viewAngle, const QAngle& aimAngle )
{
	Vector ang, aim;

	AngleVectors( viewAngle, &aim  );
	AngleVectors( aimAngle, &ang );

	return RAD2DEG( acos( aim.Dot( ang ) / aim.LengthSqr() ) );
}
// fix aimbot issues using wrong math funcs in calcangle
QAngle M::CalcAngle( Vector src, Vector dst )
{
	QAngle angles;
	Vector delta = src - dst;
	angles.x = ( asinf( delta.z / delta.Length() ) * M_RADPI );
	angles.y = ( atanf( delta.y / delta.x ) * M_RADPI );
	angles.z = 0.0f;
	if( delta.x >= 0.0 ) { angles.y += 180.0f; }

	return angles;
}

float M::VectorDistance( Vector v1, Vector v2 )
{
	return FASTSQRT( pow( v1.x - v2.x, 2 ) + pow( v1.y - v2.y, 2 ) + pow( v1.z - v2.z, 2 ) );
}

void M::VectorTransform( Vector& in1, matrix3x4a_t& in2, Vector &out )
{
	out.x = in1.Dot( in2.m_flMatVal[ 0 ] ) + in2.m_flMatVal[ 0 ][ 3 ];
	out.y = in1.Dot( in2.m_flMatVal[ 1 ] ) + in2.m_flMatVal[ 1 ][ 3 ];
	out.z = in1.Dot( in2.m_flMatVal[ 2 ] ) + in2.m_flMatVal[ 2 ][ 3 ];
}

float M::GetDelta( float hspeed, float maxspeed, float airaccelerate )
{
	auto term = ( 30.0 - ( airaccelerate * maxspeed / 66.0 ) ) / hspeed;

	if( term < 1.0f && term > -1.0f ) {
		return acos( term );
	}

	return 0.f;
}

inline float M::RandFloat( float M, float N )
{
	return ( float )( M + ( rand() / ( RAND_MAX / ( N - M ) ) ) );
}

inline Vector M::ExtrapolateTick( Vector p0, Vector v0 )
{
	return p0 + ( v0 * I::Globals->interval_per_tick );
}