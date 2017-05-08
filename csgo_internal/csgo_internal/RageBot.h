#pragma once

struct Hitbox
{
	Hitbox( void )
	{
		hitbox = -1;
	}

	Hitbox( int newHitBox )
	{
		hitbox = newHitBox;
	}

	int hitbox;
	Vector points[ 9 ];
};

enum
{
	FL_HIGH = ( 1 << 0 ),
	FL_LOW = ( 1 << 1 ),
	FL_SPECIAL = ( 1 << 2 )
};

struct BestPoint
{
	BestPoint( void )
	{
		hitbox = -1;
		point = Vector( 0, 0, 0 );
		index = -1;
		dmg = -1;
		flags = 0;
	}

	BestPoint( int newHitBox )
	{
		hitbox = newHitBox;
		point = Vector( 0, 0, 0 );
		index = -1;
		dmg = -1;
		flags = 0;
	}

	Vector point;
	int index;
	int dmg;
	int flags;
	int hitbox;
};

class CRageBot
{
public:
	void Run();
	bool Autowall( CBaseEntity* other, const Vector& startpos, const Vector& endpos, float* damage );
	bool Aimstep( QAngle src, QAngle dst, QAngle& out, int steps );

private:
	void FindTarget();
	void GoToTarget();
	void DropTarget();
	bool GhettoAutowall( CBaseEntity* );

	bool EntityIsValid( int i );
	bool IsEnemyUsingAntiAim( CBaseEntity* );
	bool GetHitbox( CBaseEntity* target, Hitbox* hitbox );
	bool GetBestPoint( CBaseEntity* target, Hitbox* hitbox, BestPoint* point );
	bool BestAimPointAll( CBaseEntity* target, Vector& hitbox );
	bool BestAimPointHitbox( CBaseEntity* target, Vector& hitbox );

	class CFixMove
	{
	public:
		void Start();
		void End();
	private:
		float m_oldforward, m_oldsidemove;
		QAngle m_oldangle;
	};

private:
	//float m_fbestfov;
	//float m_fbestdist;
	//int	m_ibesttarget;
};
