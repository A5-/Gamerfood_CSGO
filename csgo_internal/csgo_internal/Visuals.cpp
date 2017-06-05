#include "Cheat.h"

void CVisuals::Run()
{
	if( Vars.Misc.Watermark )
		E::Visuals->Watermark();

	if( !InGame() )
		return;

	if( !Vars.Visuals.Enabled )
		return;

	static bool crosshair = false;

	if( Vars.Visuals.CrosshairOn )
	{
		DrawCrosshair( Color::Red() );
		if( !crosshair )
		{
			I::Engine->ClientCmd_Unrestricted( charenc( "crosshair 0" ), nullptr );
			crosshair = true;
		}
	}
	else
	{
		if( crosshair )
		{
			I::Engine->ClientCmd_Unrestricted( charenc( "crosshair 1" ), nullptr );
			crosshair = false;
		}
	}

	DrawGlow();

	for( int i = 0; i < I::ClientEntList->GetHighestEntityIndex(); i++ )
	{
		PlayerESP( i );
		WorldESP( i );
	}
}

void CVisuals::PlayerESP( int index )
{
	CBaseEntity* Entity = I::ClientEntList->GetClientEntity( index );

	if (Entity && Vars.Visuals.Info.Name)
		if (Entity->GetClientClass()->m_ClassID == 85)
		{
			if (Entity->GetDormant())
				return;

			Vector Origin, Screen;
			Origin = Entity->GetOrigin() + Vector(0, 0, Entity->GetCollideable()->OBBMaxs().z);
			if (D::WorldToScreen(Origin, Screen))
				D::DrawString(F::ESP, Screen.x, Screen.y, Color::Green(), FONT_CENTER, charenc("Hostage"));
			
			return;
		}
	
	if( EntityIsInvalid( Entity ) )
		return;

	if( !Vars.Visuals.Filter.Friendlies && !Entity->IsEnemy() )
		return;

	if( !Vars.Visuals.Filter.Enemies && Entity->IsEnemy() )
		return;

	if( Vars.Visuals.Dlights )
	{
		dlight_t* dLight = I::vrtEffects->CL_AllocDlight( Entity->index );
		dLight->die = I::Globals->curtime + 0.05f;
		dLight->radius = 200.f;
		if( Entity->IsEnemy() )
		{
			dLight->color.r = 200;
			dLight->color.g = 200;
			dLight->color.b = 60;
		}
		else
		{
			dLight->color.r = 2;
			dLight->color.g = 48;
			dLight->color.b = 22;
		}
		dLight->color.exponent = 5;
		dLight->key = Entity->index;
		dLight->decay = dLight->radius / 5.0f;
		dLight->origin = Entity->GetOrigin() + Vector( 0, 0, 2 );
	}

	Vector max = Entity->GetCollideable()->OBBMaxs();
	Vector pos, pos3D;
	Vector top, top3D;
	pos3D = Entity->GetOrigin();
	top3D = pos3D + Vector( 0, 0, max.z );
	if( !D::WorldToScreen( pos3D, pos ) || !D::WorldToScreen( top3D, top ) )
		return;

	float height = ( pos.y - top.y );
	float width = height / 4.f;

	if( Vars.Visuals.Skeleton )
		Skeleton( Entity, Color::White() );

	if( Entity->IsVisible( 6 ) && Vars.Visuals.Box )
		PlayerBox( top.x, top.y, width, height, Entity->IsEnemy() ? Color( 200, 60, 60 ) : Color( 84, 167, 255 ) );
	else if( !Entity->IsVisible( 6 ) && Vars.Visuals.Box )
		PlayerBox( top.x, top.y, width, height, Entity->IsEnemy() ? Color( 232, 209, 32 ) : Color( 72, 219, 75 ) );

	if( Vars.Visuals.Info.Name )
		D::DrawString( F::ESP, top.x, top.y - 6, Color::White(), FONT_CENTER, "%s", Entity->GetName().c_str() );

	if( Vars.Visuals.Info.Health )
		HealthBar( pos, top, Entity->GetHealth() );

	if( Vars.Visuals.BulletTrace )
		BulletTrace( Entity, Color( 200, 50, 50, 150 ) );

	int bottom = 0;

	if( Vars.Visuals.Info.Weapon )
		D::DrawString( F::ESP, top.x, top.y + height + 8 + ( 10 * bottom++ ), Color::White(), FONT_CENTER, "%s", Entity->GetWeapon()->GetWeaponName().c_str() );

	if( Vars.Visuals.Info.Flashed && Entity->IsFlashed() )
		D::DrawString( F::ESP, top.x, top.y + height + 8 + ( 10 * bottom++ ), Color::Red(), FONT_CENTER, charenc( "FLASHED" ) );
}

void CVisuals::WorldESP( int index )
{
	CBaseEntity* Entity = I::ClientEntList->GetClientEntity( index );

	if( !Entity )
		return;

	Vector pos, pos3D;
	pos3D = Entity->GetOrigin();

	if( pos3D.IsZero() )
		return;

	if( !D::WorldToScreen( pos3D, pos ) )
		return;

	int owner = Entity->GetOwner();

	if( owner == -1 )
	{
		if( Vars.Visuals.Filter.Weapons )
		{
			if( strstr( Entity->GetClientClass()->m_pNetworkName, charenc( "CWeapon" ) ) )
			{
				D::DrawString( F::ESP, pos.x, pos.y, Color::White(), FONT_RIGHT, "%s", std::string( Entity->GetClientClass()->m_pNetworkName ).substr( 7 ).c_str() );
			}

			if( Entity->GetClientClass()->m_ClassID == 1 )
			{
				D::DrawString( F::ESP, pos.x, pos.y, Color::White(), FONT_RIGHT, "%s", charenc( "AK-47" ) );
			}

			if( Entity->GetClientClass()->m_ClassID == 39 )
			{
				D::DrawString( F::ESP, pos.x, pos.y, Color::White(), FONT_RIGHT, "%s", charenc( "Deagle") );
			}
		}

		if( Entity->GetClientClass()->m_ClassID == 29 && Vars.Visuals.Filter.C4 )
			D::DrawString( F::ESP, pos.x, pos.y, Color::LightBlue(), FONT_RIGHT, "%s", charenc( "C4" ) );
	}

	if( Entity->GetClientClass()->m_ClassID == 41 && Vars.Visuals.Filter.Decoy )
	{
		CBaseEntity* OwnerEntity = I::ClientEntList->GetClientEntity( owner & 0xFFF );
		D::DrawString( F::ESP, pos.x, pos.y, Color( 230, 78, 255 ), FONT_RIGHT, charenc( "Decoy - %s" ), OwnerEntity ? OwnerEntity->GetName().c_str() : charenc( "Disconnected" ) );
	}

	if( strstr( Entity->GetClientClass()->m_pNetworkName, charenc( "CPlantedC4" ) ) && Vars.Visuals.Filter.C4 )
		D::DrawString( F::ESP, pos.x, pos.y, Color::Red(), FONT_CENTER, "%.1f", Entity->GetBombTimer() );
}

void CVisuals::DrawGlow()
{
	CGlowObjectManager* GlowObjectManager = ( CGlowObjectManager* )offsets.GlowManager;

	for( int i = 0; i < GlowObjectManager->size; ++i )
	{
		CGlowObjectManager::GlowObjectDefinition_t* glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[ i ];
		CBaseEntity* Entity = glowEntity->getEntity();

		if( glowEntity->IsEmpty() || !Entity )
			continue;

		switch( Entity->GetClientClass()->m_ClassID )
		{
			case 1:
				if( Vars.Visuals.Filter.Weapons )
					glowEntity->set( Color( 255, 138, 46, 250 ) );

				break;

			case 29:
				if( Vars.Visuals.Filter.C4 )
					glowEntity->set( Color( 84, 147, 230, 250 ) );

				break;

			case 35:
				if( Vars.Visuals.Glow )
				{
					if( !Vars.Visuals.Filter.Friendlies && !Entity->IsEnemy() )
						break;

					if( !Vars.Visuals.Filter.Enemies && Entity->IsEnemy() )
						break;

					if( Entity->m_visible )
						glowEntity->set( Entity->IsEnemy() ? Color( 232, 209, 32, 250 ) : Color( 72, 219, 75, 250 ) );

					else
						glowEntity->set( Entity->IsEnemy() ? Color( 200, 60, 60, 250 ) : Color( 84, 167, 255, 250 ) );
				}
				break;

			case 39:
				if( Vars.Visuals.Filter.Weapons )
					glowEntity->set( Color( 255, 138, 46, 250 ) );

				break;

			case 41:
				if( Vars.Visuals.Filter.Decoy )
					glowEntity->set( Color( 230, 78, 255, 250 ) );

				break;

			case 105:
				if( Vars.Visuals.Filter.C4 )
					glowEntity->set( Color( 255, 39, 33, 250 ) );
				break;

			default:
				if( Vars.Visuals.Filter.Weapons )
				{
					if( strstr( Entity->GetClientClass()->m_pNetworkName, charenc( "CWeapon" ) ) )
						glowEntity->set( Color( 255, 138, 46, 250 ) );
				}
				break;
		}
	}
}

bool CVisuals::InGame()
{
	if( !I::Engine->IsInGame() )
		return false;

	if( !G::LocalPlayer )
		return false;

	return true;
}

bool CVisuals::EntityIsInvalid( CBaseEntity* Entity )
{
	if( !Entity )
		return true;
	if( Entity == G::LocalPlayer )
		return true;
	if( Entity->GetHealth() <= 0 )
		return true;
	if( Entity->GetDormant() )
		return true;

	return false;
}

void CVisuals::BulletTraceOverlay( CBaseEntity* entity, Color color )
{
	Vector src, dst, forward;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	M::AngleVectors( entity->GetEyeAngles(), &forward );
	filter.pSkip = entity;
	src = entity->GetBonePosition( 6 );
	dst = src + ( forward * Vars.Visuals.BulletTraceLength );

	ray.Init( src, dst );

	I::EngineTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );

	I::DebugOverlay->AddLineOverlay( src, tr.endpos, color.r(), color.g(), color.b(), true, -1 );
}

void CVisuals::BulletTrace( CBaseEntity* entity, Color color )
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	M::AngleVectors( entity->GetEyeAngles(), &forward );
	filter.pSkip = entity;
	src3D = entity->GetBonePosition( 6 ) - Vector( 0, 0, 0 );
	dst3D = src3D + ( forward * Vars.Visuals.BulletTraceLength );

	ray.Init( src3D, dst3D );

	I::EngineTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );

	if( !D::WorldToScreen( src3D, src ) || !D::WorldToScreen( tr.endpos, dst ) )
		return;

	D::DrawLine( src.x, src.y, dst.x, dst.y, color );
	D::DrawRect( dst.x - 3, dst.y - 3, 6, 6, color );
}

void CVisuals::Watermark()
{
	static float rainbow;
	rainbow += 0.005f;
	if( rainbow > 1.f )
		rainbow = 0.f;
	D::DrawString( F::Watermark, 5, 11, Color::FromHSB( rainbow, 1.f, 1.f ), FONT_LEFT, "%s", charenc( "TEAMGAMERFOOD.COM" ) );
}

void CVisuals::PlayerBox( float x, float y, float w, float h, Color clr )
{
	I::Surface->DrawSetColor( clr );
	I::Surface->DrawOutlinedRect( x - w, y, x + w, y + h );

	I::Surface->DrawSetColor( Color::Black() );
	I::Surface->DrawOutlinedRect( x - w - 1, y - 1, x + w + 1, y + h + 1 );
	I::Surface->DrawOutlinedRect( x - w + 1, y + 1, x + w - 1, y + h - 1 );
}

void CVisuals::HealthBar( Vector bot, Vector top, float health )
{
	float h = ( bot.y - top.y );
	float offset = ( h / 4.f ) + 5;
	float w = h / 64.f;

	UINT hp = h - ( UINT )( ( h * health ) / 100 ); // Percentage

	int Red = 255 - ( health * 2.55 );
	int Green = health * 2.55;

	D::DrawOutlinedRect( ( top.x - offset ) - 1, top.y - 1, 3, h + 2, Color::Black() );

	D::DrawLine( ( top.x - offset ), top.y + hp, ( top.x - offset ), top.y + h, Color( Red, Green, 0, 255 ) );
}

void CVisuals::Skeleton( CBaseEntity* Entity, Color color )
{
	studiohdr_t* pStudioModel = I::ModelInfo->GetStudioModel( Entity->GetModel() );
	if( pStudioModel )
	{
		static matrix3x4_t pBoneToWorldOut[ 128 ];
		if( Entity->SetupBones( pBoneToWorldOut, 128, 256, I::Globals->curtime ) )
		{
			for( int i = 0; i < pStudioModel->numbones; i++ )
			{
				mstudiobone_t* pBone = pStudioModel->pBone( i );
				if( !pBone || !( pBone->flags & 256 ) || pBone->parent == -1 )
					continue;

				Vector vBonePos1;
				if( !D::WorldToScreen( Vector( pBoneToWorldOut[ i ][ 0 ][ 3 ], pBoneToWorldOut[ i ][ 1 ][ 3 ], pBoneToWorldOut[ i ][ 2 ][ 3 ] ), vBonePos1 ) )
					continue;

				Vector vBonePos2;
				if( !D::WorldToScreen( Vector( pBoneToWorldOut[ pBone->parent ][ 0 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 1 ][ 3 ], pBoneToWorldOut[ pBone->parent ][ 2 ][ 3 ] ), vBonePos2 ) )
					continue;

				D::DrawLine( ( int )vBonePos1.x, ( int )vBonePos1.y, ( int )vBonePos2.x, ( int )vBonePos2.y, color );
			}
		}
	}
}

void CVisuals::DrawHitbox( matrix3x4a_t* matrix, Vector bbmin, Vector bbmax, int bone, Color color )
{
	Vector points[] = {
		Vector( bbmin.x, bbmin.y, bbmin.z ),
		Vector( bbmin.x, bbmax.y, bbmin.z ),
		Vector( bbmax.x, bbmax.y, bbmin.z ),
		Vector( bbmax.x, bbmin.y, bbmin.z ),
		Vector( bbmax.x, bbmax.y, bbmax.z ),
		Vector( bbmin.x, bbmax.y, bbmax.z ),
		Vector( bbmin.x, bbmin.y, bbmax.z ),
		Vector( bbmax.x, bbmin.y, bbmax.z )
	};

	Vector pointsTransformed[ 8 ];

	for( int index = 0; index <= 8; ++index )
	{
		if( index != 0 )
			points[ index ] = ( ( ( ( points[ index ] + points[ 0 ] ) * .5f ) + points[ index ] ) * .5f );

		M::VectorTransform( points[ index ], matrix[ bone ], pointsTransformed[ index ] );
	}

	D::Draw3DBox( pointsTransformed, color );
}

void CVisuals::Hitboxes( CBaseEntity* Entity, Color color )
{
	matrix3x4a_t matrix[ MAXSTUDIOBONES ];

	if( !Entity->SetupBones( matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::Engine->GetLastTimeStamp() ) )
		return;

	studiohdr_t* pStudioModel = I::ModelInfo->GetStudioModel( Entity->GetModel() );
	if( !pStudioModel )
		return;

	mstudiohitboxset_t* set = pStudioModel->pHitboxSet( Entity->GetHitboxSet() );
	if( !set )
		return;

	for( int i = 0; i < set->numhitboxes; i++ )
	{
		mstudiobbox_t* hitbox = set->pHitbox( i );
		if( !hitbox )
			return;

		int bone = hitbox->bone;

		Vector vMaxUntransformed = hitbox->bbmax;
		Vector vMinUntransformed = hitbox->bbmin;

		if( hitbox->radius != -1.f )
		{
			vMinUntransformed -= Vector( hitbox->radius, hitbox->radius, hitbox->radius );
			vMaxUntransformed += Vector( hitbox->radius, hitbox->radius, hitbox->radius );
		}

		DrawHitbox( matrix, vMinUntransformed, vMaxUntransformed, hitbox->bone, color );
	}
}

void CVisuals::DrawCrosshair( Color color )
{
	int screenW, screenH;
	I::Engine->GetScreenSize( screenW, screenH );

	int crX = screenW / 2, crY = screenH / 2;
	int dy = screenH / G::FOV;
	int dx = screenW / G::FOV;
	int drX;
	int drY;

	if( Vars.Visuals.CrosshairType == 0 )
	{
		if( Vars.Visuals.Removals.VisualRecoil )
		{
			drX = crX - ( int )( dx * ( G::LocalPlayer->GetPunch().y * 2 ) );
			drY = crY + ( int )( dy * ( G::LocalPlayer->GetPunch().x * 2 ) );
		}
		else
		{
			drX = crX - ( int )( dx * ( ( ( G::LocalPlayer->GetPunch().y * 2.f ) * 0.45f ) + G::LocalPlayer->GetViewPunch().y ) );
			drY = crY + ( int )( dy * ( ( ( G::LocalPlayer->GetPunch().x * 2.f ) * 0.45f ) + G::LocalPlayer->GetViewPunch().x ) );
		}
	}
	else
	{
		drX = crX;
		drY = crY;
	}

	if( Vars.Visuals.CrosshairStyle == 0 )
	{
		D::DrawLine( drX - 5, drY, drX + 5, drY, color );
		D::DrawLine( drX, drY - 5, drX, drY + 5, color );
	}

	if( Vars.Visuals.CrosshairStyle == 1 )
	{
		D::DrawCircle( drX, drY, 5, 30, color );
	}

	if( Vars.Visuals.CrosshairStyle == 2 )
	{
		D::DrawOutlinedRect( drX - 5, drY - 5, 10, 10, color );
	}

	if( Vars.Visuals.CrosshairStyle == 3 )
	{
		D::DrawLine( drX, drY, drX, drY - 5, color );
		D::DrawLine( drX, drY - 5, drX + 5, drY - 5, color );

		D::DrawLine( drX, drY, drX + 5, drY, color );
		D::DrawLine( drX + 5, drY, drX + 5, drY + 5, color );

		D::DrawLine( drX, drY, drX, drY + 5, color );
		D::DrawLine( drX, drY + 5, drX - 5, drY + 5, color );

		D::DrawLine( drX, drY, drX - 5, drY, color );
		D::DrawLine( drX - 5, drY, drX - 5, drY - 5, color );
	}
}
