#include "Cheat.h"

void CConfig::Setup()
{
	SetupValue( Vars.Ragebot.Enabled, false, strenc( "Ragebot" ), strenc( "Enabled" ) );
	SetupValue( Vars.Ragebot.AutoFire, false, strenc( "Ragebot" ), strenc( "AutoFire" ) );
	SetupValue( Vars.Ragebot.Hold, false, strenc( "Ragebot" ), strenc( "Hold" ) );
	SetupValue( Vars.Ragebot.HoldKey, 0, strenc( "Ragebot" ), strenc( "HoldKey" ) );
	SetupValue( Vars.Ragebot.FOV, 180.f, strenc( "Ragebot" ), strenc( "FOV" ) );
	SetupValue( Vars.Ragebot.Silent, true, strenc( "Ragebot" ), strenc( "Silent" ) );
	SetupValue( Vars.Ragebot.TargetMethod, 0, strenc( "Ragebot" ), strenc( "TargetMethod" ) );
	SetupValue( Vars.Ragebot.Hitbox, 6, strenc( "Ragebot" ), strenc( "Hitbox" ) );
	SetupValue( Vars.Ragebot.FriendlyFire, false, strenc( "Ragebot" ), strenc( "FriendlyFire" ) );
	SetupValue( Vars.Ragebot.AutoStop, false, strenc( "Ragebot" ), strenc( "AutoStop" ) );
	SetupValue( Vars.Ragebot.AutoCrouch, false, strenc( "Ragebot" ), strenc( "AutoCrouch" ) );
	SetupValue( Vars.Ragebot.AutoWall, true, strenc( "Ragebot" ), strenc( "AutoWall" ) );
	SetupValue( Vars.Ragebot.AutoWallDmg, 10.f, strenc( "Ragebot" ), strenc( "AutoWallDamage" ) );
	SetupValue( Vars.Ragebot.HitChance, false, strenc( "Ragebot" ), strenc( "HitChance" ) );
	SetupValue( Vars.Ragebot.HitChanceAmt, 50.f, strenc( "Ragebot" ), strenc( "HitChanceAmount" ) );
	SetupValue( Vars.Ragebot.Aimstep, false, strenc( "Ragebot" ), strenc( "Aimstep" ) );
	SetupValue( Vars.Ragebot.AimstepAmount, 15.f, strenc( "Ragebot" ), strenc( "AimstepAmount" ) );
	SetupValue( Vars.Ragebot.HitScanAll, false, strenc( "Ragebot" ), strenc( "HitScanAll" ) );
	SetupValue( Vars.Ragebot.HitScanHitbox, false, strenc( "Ragebot" ), strenc( "HitScanHitbox" ) );
	SetupValue( Vars.Ragebot.UntrustedCheck, true, strenc( "Ragebot" ), strenc( "Anti-Untrust" ) );
	SetupValue( Vars.Ragebot.Antiaim.Enabled, false, strenc( "Ragebot" ), strenc( "AntiaimEnabled" ) );
	SetupValue( Vars.Ragebot.Antiaim.Pitch, 0, strenc( "Ragebot" ), strenc( "AntiaimPitch" ) );
	SetupValue( Vars.Ragebot.Antiaim.Yaw, 0, strenc( "Ragebot" ), strenc( "AntiaimYaw" ) );
	SetupValue( Vars.Ragebot.Antiaim.PSilent, true, strenc( "Ragebot" ), strenc( "AntiaimYaw" ) );


	SetupValue( Vars.Legitbot.Aimbot.Enabled, false, strenc( "Legitbot" ), strenc( "Enabled" ) );
	SetupValue( Vars.Legitbot.Aimbot.AlwaysOn, false, strenc( "Legitbot" ), strenc( "AlwaysOn" ) );
	SetupValue( Vars.Legitbot.Aimbot.Key, 0, strenc( "Legitbot" ), strenc( "Key" ) );
	SetupValue( Vars.Legitbot.Aimbot.OnKey, false, strenc( "Legitbot" ), strenc( "OnKey" ) );
	SetupValue( Vars.Legitbot.Aimbot.FriendlyFire, false, strenc( "Legitbot" ), strenc( "FriendlyFire" ) );
	SetupValue( Vars.Legitbot.Aimbot.Hitbox, 6, strenc( "Legitbot" ), strenc( "Hitbox" ) );
	SetupValue( Vars.Legitbot.Aimbot.FOV, 10.f, strenc( "Legitbot" ), strenc( "FOV" ) );
	SetupValue( Vars.Legitbot.Aimbot.Speed, 5.f, strenc( "Legitbot" ), strenc( "Speed" ) );
	SetupValue( Vars.Legitbot.Aimbot.RCS, true, strenc( "Legitbot" ), strenc( "RCS" ) );
	SetupValue( Vars.Legitbot.Aimbot.RCSAmount, 100.f, strenc( "Legitbot" ), strenc( "RCSAmount" ) );

	SetupValue( Vars.Legitbot.Triggerbot.Enabled, false, strenc( "Triggerbot" ), strenc( "Enabled" ) );
	SetupValue( Vars.Legitbot.Triggerbot.AutoFire, false, strenc( "Triggerbot" ), strenc( "AutoFire" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Key, 0, strenc( "Triggerbot" ), strenc( "Key" ) );
	SetupValue( Vars.Legitbot.Triggerbot.HitChance, false, strenc( "Triggerbot" ), strenc( "HitChance" ) );
	SetupValue( Vars.Legitbot.Triggerbot.HitChanceAmt, 60.f, strenc( "Triggerbot" ), strenc( "HitChanceAmt" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Head, false, strenc( "Triggerbot" ), strenc( "FilterHead" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Chest, false, strenc( "Triggerbot" ), strenc( "FilterChest" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Stomach, false, strenc( "Triggerbot" ), strenc( "FilterStomach" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Arms, false, strenc( "Triggerbot" ), strenc( "FilterArms" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Legs, false, strenc( "Triggerbot" ), strenc( "FilterLegs" ) );
	SetupValue( Vars.Legitbot.Triggerbot.Filter.Friendly, false, strenc( "Triggerbot" ), strenc( "FilterFriendly" ) );

	SetupValue( Vars.Visuals.Enabled, true, strenc( "Visuals" ), strenc( "Enabled" ) );
	SetupValue( Vars.Visuals.Box, true, strenc( "Visuals" ), strenc( "Box" ) );
	SetupValue( Vars.Visuals.Skeleton, false, strenc( "Visuals" ), strenc( "Skeleton" ) );
	SetupValue( Vars.Visuals.BulletTrace, false, strenc( "Visuals" ), strenc( "BulletTrace" ) );
	SetupValue( Vars.Visuals.BulletTraceLength, 200.f, strenc( "Visuals" ), strenc( "BulletTraceLength" ) );
	SetupValue( Vars.Visuals.Glow, true, strenc( "Visuals" ), strenc( "Glow" ) );
	SetupValue( Vars.Visuals.CrosshairOn, true, strenc( "Visuals" ), strenc( "CrosshairOn" ) );
	SetupValue( Vars.Visuals.CrosshairType, 0, strenc( "Visuals" ), strenc( "CrosshairType" ) );
	SetupValue( Vars.Visuals.CrosshairStyle, 0, strenc( "Visuals" ), strenc( "CrosshairStyle" ) );
	SetupValue( Vars.Visuals.Info.Name, true, strenc( "Visuals" ), strenc( "InfoName" ) );
	SetupValue( Vars.Visuals.Info.Health, true, strenc( "Visuals" ), strenc( "InfoHealth" ) );
	SetupValue( Vars.Visuals.Info.Weapon, true, strenc( "Visuals" ), strenc( "InfoWeapon" ) );
	SetupValue( Vars.Visuals.Info.Flashed, true, strenc( "Visuals" ), strenc( "InfoFlashed" ) );
	SetupValue( Vars.Visuals.Filter.Enemies, true, strenc( "Visuals" ), strenc( "FilterEnemies" ) );
	SetupValue( Vars.Visuals.Filter.Friendlies, true, strenc( "Visuals" ), strenc( "FilterFriendlies" ) );
	SetupValue( Vars.Visuals.Filter.Weapons, true, strenc( "Visuals" ), strenc( "FilterWeapons" ) );
	SetupValue( Vars.Visuals.Filter.Decoy, true, strenc( "Visuals" ), strenc( "FilterDecoy" ) );
	SetupValue( Vars.Visuals.Filter.C4, true, strenc( "Visuals" ), strenc( "FilterC4" ) );
	SetupValue( Vars.Visuals.Chams.Enabled, true, strenc( "Visuals" ), strenc( "ChamsEnabled" ) );
	SetupValue( Vars.Visuals.Chams.Mode, 0, strenc( "Visuals" ), strenc( "ChamsMode" ) );
	SetupValue( Vars.Visuals.Chams.XQZ, true, strenc( "Visuals" ), strenc( "ChamsXQZ" ) );
	SetupValue( Vars.Visuals.Chams.Wireframe, true, strenc( "Visuals" ), strenc( "ChamsWireframe" ) );
	SetupValue( Vars.Visuals.Removals.Hands, false, strenc( "Visuals" ), strenc( "RemoveHands" ) );
	SetupValue( Vars.Visuals.Removals.Weapon, false, strenc( "Visuals" ), strenc( "RemoveWeapon" ) );
	SetupValue( Vars.Visuals.Removals.VisualRecoil, true, strenc( "Visuals" ), strenc( "RemoveRecoil" ) );
	SetupValue( Vars.Visuals.Removals.Flash, true, strenc( "Visuals" ), strenc( "RemoveFlash" ) );
	SetupValue(Vars.Visuals.Dlights, true, strenc("Visuals"), strenc("DLightEnabled"));

	SetupValue( Vars.Misc.Bhop, true, strenc( "Misc" ), strenc( "Bhop" ) );
	SetupValue( Vars.Misc.AutoStrafe, false, strenc( "Misc" ), strenc( "AutoStrafe" ) );
	SetupValue( Vars.Misc.Watermark, true, strenc( "Misc" ), strenc( "Watermark" ) );
	SetupValue( Vars.Misc.Ranks, true, strenc( "Misc" ), strenc( "Ranks" ) );
	SetupValue( Vars.Misc.AutoAccept, true, strenc( "Misc" ), strenc( "AutoAccept" ) );
	SetupValue( Vars.Misc.AirStuck, false, strenc( "Misc" ), strenc( "AirStuck" ) );
	SetupValue( Vars.Misc.AirStuckKey, 0, strenc( "Misc" ), strenc( "AirStuckKey" ) );
}

void CConfig::SetupValue( int &value, int def, std::string category, std::string name )
{
	value = def;
	ints.push_back( new ConfigValue<int>( category, name, &value ) );
}

void CConfig::SetupValue( float &value, float def, std::string category, std::string name )
{
	value = def;
	floats.push_back( new ConfigValue<float>( category, name, &value ) );
}

void CConfig::SetupValue( bool &value, bool def, std::string category, std::string name )
{
	value = def;
	bools.push_back( new ConfigValue<bool>( category, name, &value ) );
}

void CConfig::Save()
{
	static TCHAR path[ MAX_PATH ];
	std::string folder, file;

	if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + strenc( "\\notacheat\\" );
		file = std::string( path ) + strenc( "\\notacheat\\gamerfood.ini" );
	}

	CreateDirectory( folder.c_str(), NULL );

	for ( auto value : ints )
		WritePrivateProfileString( value->category.c_str(), value->name.c_str(), std::to_string( *value->value ).c_str(), file.c_str() );

	for ( auto value : floats )
		WritePrivateProfileString( value->category.c_str(), value->name.c_str(), std::to_string( *value->value ).c_str(), file.c_str() );

	for ( auto value : bools )
		WritePrivateProfileString( value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str() );
}

void CConfig::Load()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + strenc( "\\notacheat\\" );
		file = std::string( path ) + strenc( "\\notacheat\\gamerfood.ini" );
	}

	CreateDirectory( folder.c_str(), NULL );

	char value_l[32] = { '\0' };

	for ( auto value : ints )
	{
		GetPrivateProfileString( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = atoi( value_l );
	}

	for ( auto value : floats )
	{
		GetPrivateProfileString( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = atof( value_l );
	}

	for ( auto value : bools )
	{
		GetPrivateProfileString( value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str() );
		*value->value = !strcmp( value_l, "true" );
	}
}

CConfig* Config = new CConfig();
Variables Vars;
