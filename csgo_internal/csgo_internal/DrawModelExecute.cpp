#include "Cheat.h"
DrawModelExecuteFn oDrawModelExecute;

void ForceMaterial( IMaterial* material, Color color )
{
	if( material != NULL )
	{
		I::RenderView->SetColorModulation( color.Base() );
		I::ModelRender->ForcedMaterialOverride( material );
	}
}

void __stdcall Hooks::DrawModelExecute( void* context, void* state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld )
{
	H::ModelRender->UnHook();

	if( I::Engine->IsInGame() && G::LocalPlayer )
	{
		if( info.pModel )
		{
			std::string modelName = I::ModelInfo->GetModelName( info.pModel );

			if( modelName.find( strenc( "models/player" ) ) != std::string::npos && Vars.Visuals.Chams.Enabled )
			{
				CBaseEntity* pModelEntity = ( CBaseEntity* )I::ClientEntList->GetClientEntity( info.entity_index );
				if( pModelEntity && pModelEntity->GetAlive() && !pModelEntity->GetDormant() )
				{
					Color render_color_hidden = pModelEntity->GetTeam() == G::LocalPlayer->GetTeam() ? Color( 72, 219, 75 ) : Color( 232, 209, 32 );
					Color render_color_visible = pModelEntity->GetTeam() == G::LocalPlayer->GetTeam() ? Color( 84, 167, 255 ) : Color( 200, 60, 60 );

					if( Vars.Visuals.Chams.XQZ )
					{
						ForceMaterial( Vars.Visuals.Chams.Mode == 0 ? hidden_flat : hidden_tex, render_color_hidden );
						I::ModelRender->DrawModelExecute( context, state, info, pCustomBoneToWorld );

						if( Vars.Visuals.Chams.Mode == 0 )
							hidden_flat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
						else
							hidden_tex->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
					}

					if( Vars.Visuals.Chams.Mode == 0 )
						visible_flat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
					else
						visible_tex->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );

					ForceMaterial( Vars.Visuals.Chams.Mode == 0 ? visible_flat : visible_tex, render_color_visible );
					I::ModelRender->DrawModelExecute( context, state, info, pCustomBoneToWorld );
				}
			}

			else if( modelName.find( strenc( "flash" ) ) != std::string::npos && Vars.Visuals.Removals.Flash )
			{
				IMaterial* flash = I::MaterialSystem->FindMaterial( charenc( "effects\\flashbang" ), charenc( TEXTURE_GROUP_CLIENT_EFFECTS ) );
				IMaterial* flashWhite = I::MaterialSystem->FindMaterial( charenc( "effects\\flashbang_white" ), charenc( TEXTURE_GROUP_CLIENT_EFFECTS ) );
				flash->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
				flashWhite->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
				I::ModelRender->ForcedMaterialOverride( flash );
				I::ModelRender->ForcedMaterialOverride( flashWhite );
			}

			else if( modelName.find( "arms" ) != std::string::npos && Vars.Visuals.Removals.Hands )
			{
				IMaterial* Hands = I::MaterialSystem->FindMaterial( modelName.c_str(), charenc( TEXTURE_GROUP_MODEL ) );
				Hands->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
				I::ModelRender->ForcedMaterialOverride( Hands );
			}

			else if( modelName.find( "weapon" ) != std::string::npos && Vars.Visuals.Removals.Weapon )
			{
				if( !( modelName.find( "arms" ) != std::string::npos ) )
				{
					IMaterial* Weapon = I::MaterialSystem->FindMaterial( modelName.c_str(), charenc( TEXTURE_GROUP_MODEL ) );
					Weapon->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, true );
					I::ModelRender->ForcedMaterialOverride( Weapon );
				}
			}
		}
	}
	I::ModelRender->DrawModelExecute( context, state, info, pCustomBoneToWorld );
	I::ModelRender->ForcedMaterialOverride( NULL );
	H::ModelRender->ReHook();
}
