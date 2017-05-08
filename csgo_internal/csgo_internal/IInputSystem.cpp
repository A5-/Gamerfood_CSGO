#include "sdk.h"

void IInputSystem::EnableInput( bool bEnable )
{
	typedef void ( __thiscall* OriginalFn )( void*, bool );
	return U::GetVFunc< OriginalFn >( this, 11 )( this, bEnable );
}

void IInputSystem::ResetInputState()
{
	typedef void ( __thiscall* OriginalFn )( void* );
	return U::GetVFunc< OriginalFn >( this, 39 )( this );
}

bool IInputSystem::IsButtonDown( ButtonCode_t code )
{
	typedef bool ( __thiscall* OriginalFn )( void*, ButtonCode_t );
	return U::GetVFunc< OriginalFn >( this, 15 )( this, code );
}

void IInputSystem::GetCursorPosition( int* m_pX, int* m_pY )
{
	typedef void ( __thiscall* OriginalFn )( void*, int*, int* );
	return U::GetVFunc< OriginalFn >( this, 56 )( this, m_pX, m_pY );
}

ButtonCode_t IInputSystem::VirtualKeyToButtonCode( int nVirtualKey )
{
	typedef ButtonCode_t ( __thiscall* OriginalFn )( void*, int );
	return U::GetVFunc< OriginalFn >( this, 44 )( this, nVirtualKey );
}

int IInputSystem::ButtonCodeToVirtualKey( ButtonCode_t code )
{
	typedef int ( __thiscall* OriginalFn )( void*, ButtonCode_t );
	return U::GetVFunc< OriginalFn >( this, 45 )( this, code );
}
