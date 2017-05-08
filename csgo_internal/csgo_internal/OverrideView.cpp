#include "Cheat.h"

OverrideViewFn oOverrideView;

void __stdcall Hooks::OverrideView( CViewSetup* vsView )
{
	G::FOV = vsView->fov;

	oOverrideView( vsView );
}
