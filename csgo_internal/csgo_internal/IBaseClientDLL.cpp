#include "sdk.h"

ClientClass* IBaseClientDll::GetAllClasses()
{
	typedef ClientClass*(__thiscall* OriginalFn)( PVOID );
	return U::GetVFunc< OriginalFn >( this, 8 )( this );
}
