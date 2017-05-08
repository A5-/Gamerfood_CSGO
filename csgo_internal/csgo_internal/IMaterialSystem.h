#pragma once

class IMaterialSystem
{
public:
	IMaterial* CreateMaterial( bool flat, bool ignorez, bool wireframed );
	IMaterial* FindMaterial( char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = NULL );
};
