#include "Cheat.h"

FrameStageNotifyFn oFrameStageNotify;

void __stdcall Hooks::FrameStageNotify(ClientFrameStage_t stage)
{
    QAngle aim_punch_old;
    QAngle view_punch_old;

    QAngle* aim_punch = nullptr;
    QAngle* view_punch = nullptr;

    if(I::Engine->IsInGame())
    {
        if(stage == FRAME_RENDER_START)
        {
            if(G::LocalPlayer && G::LocalPlayer->GetAlive()
                && Vars.Visuals.Removals.VisualRecoil)
            {
                aim_punch = 
                    (QAngle*)((DWORD)G::LocalPlayer + offsets.m_aimPunchAngle);

                view_punch = 
                    (QAngle*)((DWORD)G::LocalPlayer + offsets.m_viewPunchAngle);

                aim_punch_old = *aim_punch;
                view_punch_old = *view_punch;

                *aim_punch = QAngle(0, 0, 0);
                *view_punch = QAngle(0, 0, 0);
            }

            for(int i = 1; i <= I::Globals->maxClients; i++)
            {
                if(i == I::Engine->GetLocalPlayer())
                    continue;

                CBaseEntity* pCurEntity = I::ClientEntList->GetClientEntity(i);

                if(!pCurEntity)
                    continue;

                *(int*)((uintptr_t)pCurEntity + 0xA30) = I::Globals->framecount;
                *(int*)((uintptr_t)pCurEntity + 0xA28) = 0;
            }
        }
    }

    oFrameStageNotify(stage);

    if(aim_punch && view_punch && Vars.Visuals.Removals.VisualRecoil)
    {
        *aim_punch = aim_punch_old;
        *view_punch = view_punch_old;
    }
}
