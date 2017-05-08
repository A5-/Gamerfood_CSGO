#pragma once

class IVModelRender
{
public:
	void ForcedMaterialOverride( IMaterial* mat );
	void DrawModelExecute( void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL );
};
