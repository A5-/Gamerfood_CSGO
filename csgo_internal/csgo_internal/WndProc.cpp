#include "Cheat.h"

WNDPROC Hooks::oldWindowProc;
void OpenMenu();

extern LRESULT ImGui_ImplDX9_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

LRESULT __stdcall Hooks::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			G::PressedKeys[ VK_LBUTTON ] = true;
			break;
		case WM_LBUTTONUP:
			G::PressedKeys[ VK_LBUTTON ] = false;
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			G::PressedKeys[ VK_RBUTTON ] = true;
			break;
		case WM_RBUTTONUP:
			G::PressedKeys[ VK_RBUTTON ] = false;
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
			G::PressedKeys[ VK_MBUTTON ] = true;
			break;
		case WM_MBUTTONUP:
			G::PressedKeys[ VK_MBUTTON ] = false;
			break;
		case WM_XBUTTONDOWN:
		case WM_XBUTTONDBLCLK:
		{
			UINT button = GET_XBUTTON_WPARAM( wParam );
			if( button == XBUTTON1 )
			{
				G::PressedKeys[ VK_XBUTTON1 ] = true;
			}
			else if( button == XBUTTON2 )
			{
				G::PressedKeys[ VK_XBUTTON2 ] = true;
			}
			break;
		}
		case WM_XBUTTONUP:
		{
			UINT button = GET_XBUTTON_WPARAM( wParam );
			if( button == XBUTTON1 )
			{
				G::PressedKeys[ VK_XBUTTON1 ] = false;
			}
			else if( button == XBUTTON2 )
			{
				G::PressedKeys[ VK_XBUTTON2 ] = false;
			}
			break;
		}
		case WM_KEYDOWN:
		{
			G::PressedKeys[ wParam ] = true;
			
			if ( wParam == VK_INSERT )
				Vars.Menu.Opened = !Vars.Menu.Opened;
			
			break;
		}
		case WM_KEYUP:
			G::PressedKeys[ wParam ] = false;
			break;
		case WM_SYSKEYDOWN:
			if (wParam == VK_MENU)
				G::PressedKeys[VK_MENU] = true;
			break;
		case WM_SYSKEYUP:
			if (wParam == VK_MENU)
				G::PressedKeys[VK_MENU] = false;
			break;
		default: break;
	}
	
	if( G::d3dinit && Vars.Menu.Opened && ImGui_ImplDX9_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return true;

	return CallWindowProc( oldWindowProc, hWnd, uMsg, wParam, lParam );
}
