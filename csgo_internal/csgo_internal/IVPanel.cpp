#include "sdk.h"

const char* IVPanel::GetName( int iIndex )
{
	typedef const char*(__thiscall* OriginalFn)( void*, int );
	return U::GetVFunc< OriginalFn >( this, 36 )( this, iIndex );
}
