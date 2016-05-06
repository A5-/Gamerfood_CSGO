#include "Cheat.h"

void CInput::GetClicked()
{
	if ( GetAsyncKeyState( VK_LBUTTON ) && GetActiveWindow() == FindWindowA(charenc("Valve001"), NULL) )
	{
		bClicked = false;
		bMouseD = true;
	}
	else if ( bMouseD )
	{
		bClicked = true;
		bMouseD = false;
	}
	else if ( bClicked )
	{
		bClicked = false;
	}
}

bool CInput::Holding( int iXStart, int iYStart, int iWidth, int iHeight )
{
	if ( GetAsyncKeyState( VK_LBUTTON ) && GetActiveWindow() == FindWindowA( charenc( "Valve001" ), NULL ) )
		if ( Hovering( iXStart, iYStart, iWidth, iHeight ) )
			return true;

	return false;
}

bool CInput::Hovering( int iXStart, int iYStart, int iWidth, int iHeight )
{
	static int iMouseX, iMouseY;
	I::InputSystem->GetCursorPosition( &iMouseX, &iMouseY );

	if ( iMouseX >= iXStart && iXStart + iWidth >= iMouseX )
		if ( iMouseY >= iYStart && iYStart + iHeight >= iMouseY )
			return true;

	return false;
}

bool CInput::Clicked( int iXStart, int iYStart, int iWidth, int iHeight )
{
	if ( bClicked )
		if ( Hovering( iXStart, iYStart, iWidth, iHeight ) )
			return true;

	return false;
}

CInput* Input = new CInput;