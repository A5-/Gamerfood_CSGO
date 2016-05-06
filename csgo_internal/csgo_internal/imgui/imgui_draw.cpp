// dear imgui, v1.49 WIP
// (drawing and font code)

// Contains implementation for
// - ImDrawList
// - ImDrawData
// - ImFontAtlas
// - ImFont
// - Default font data

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui_internal.h"

#include <stdio.h>      // vsnprintf, sscanf, printf
#if !defined(alloca) && !defined(__FreeBSD__) && !defined(__DragonFly__)
#ifdef _WIN32
#include <malloc.h>     // alloca
#else
#include <alloca.h>     // alloca
#endif
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 4996) // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#define snprintf _snprintf
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"            // warning : comparing floating point with == or != is unsafe   // storing and comparing against same constants ok.
#pragma clang diagnostic ignored "-Wglobal-constructors"    // warning : declaration requires a global destructor           // similar to above, not sure what the exact difference it.
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning : implicit conversion changes signedness             //
//#pragma clang diagnostic ignored "-Wreserved-id-macro"    // warning : macro name is a reserved identifier                //
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#endif

//-------------------------------------------------------------------------
// STB libraries implementation
//-------------------------------------------------------------------------

//#define IMGUI_STB_NAMESPACE     ImGuiStb
//#define IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
//#define IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION

#ifdef IMGUI_STB_NAMESPACE
namespace IMGUI_STB_NAMESPACE
{
#endif

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4456) // declaration of 'xx' hides previous local declaration
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning : use of old-style cast                              // yes, they are more terse.
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

#define STBRP_ASSERT(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#include "stb_rect_pack.h"

#define STBTT_malloc(x,u)  ((void)(u), ImGui::MemAlloc(x))
#define STBTT_free(x,u)    ((void)(u), ImGui::MemFree(x))
#define STBTT_assert(x)    IM_ASSERT(x)
#ifndef IMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#else
#define STBTT_DEF extern
#endif
#include "stb_truetype.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef IMGUI_STB_NAMESPACE
} // namespace ImGuiStb
using namespace IMGUI_STB_NAMESPACE;
#endif

//-----------------------------------------------------------------------------
// ImDrawList
//-----------------------------------------------------------------------------

static const ImVec4 GNullClipRect(-8192.0f, -8192.0f, +8192.0f, +8192.0f); // Large values that are easy to encode in a few bits+shift

void ImDrawList::Clear()
{
    CmdBuffer.resize(0);
    IdxBuffer.resize(0);
    VtxBuffer.resize(0);
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.resize(0);
    _TextureIdStack.resize(0);
    _Path.resize(0);
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    // NB: Do not clear channels so our allocations are re-used after the first frame.
}

void ImDrawList::ClearFreeMemory()
{
    CmdBuffer.clear();
    IdxBuffer.clear();
    VtxBuffer.clear();
    _VtxCurrentIdx = 0;
    _VtxWritePtr = NULL;
    _IdxWritePtr = NULL;
    _ClipRectStack.clear();
    _TextureIdStack.clear();
    _Path.clear();
    _ChannelsCurrent = 0;
    _ChannelsCount = 1;
    for (int i = 0; i < _Channels.Size; i++)
    {
        if (i == 0) memset(&_Channels[0], 0, sizeof(_Channels[0]));  // channel 0 is a copy of CmdBuffer/IdxBuffer, don't destruct again
        _Channels[i].CmdBuffer.clear();
        _Channels[i].IdxBuffer.clear();
    }
    _Channels.clear();
}

// Use macros because C++ is a terrible language, we want guaranteed inline, no code in header, and no overhead in Debug mode
#define GetCurrentClipRect()    (_ClipRectStack.Size ? _ClipRectStack.Data[_ClipRectStack.Size-1]  : GNullClipRect)
#define GetCurrentTextureId()   (_TextureIdStack.Size ? _TextureIdStack.Data[_TextureIdStack.Size-1] : NULL)

void ImDrawList::AddDrawCmd()
{
    ImDrawCmd draw_cmd;
    draw_cmd.ClipRect = GetCurrentClipRect();
    draw_cmd.TextureId = GetCurrentTextureId();

    IM_ASSERT(draw_cmd.ClipRect.x <= draw_cmd.ClipRect.z && draw_cmd.ClipRect.y <= draw_cmd.ClipRect.w);
    CmdBuffer.push_back(draw_cmd);
}

void ImDrawList::AddCallback(ImDrawCallback callback, void* callback_data)
{
    ImDrawCmd* current_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!current_cmd || current_cmd->ElemCount != 0 || current_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        current_cmd = &CmdBuffer.back();
    }
    current_cmd->UserCallback = callback;
    current_cmd->UserCallbackData = callback_data;

    AddDrawCmd(); // Force a new command after us (see comment below)
}

// Our scheme may appears a bit unusual, basically we want the most-common calls AddLine AddRect etc. to not have to perform any check so we always have a command ready in the stack.
// The cost of figuring out if a new command has to be added or if we can merge is paid in those Update** functions only.
void ImDrawList::UpdateClipRect()
{
    // If current command is used with different settings we need to add a new command
    const ImVec4 curr_clip_rect = GetCurrentClipRect();
    ImDrawCmd* curr_cmd = CmdBuffer.Size > 0 ? &CmdBuffer.Data[CmdBuffer.Size-1] : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && memcmp(&curr_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) != 0) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (prev_cmd && memcmp(&prev_cmd->ClipRect, &curr_clip_rect, sizeof(ImVec4)) == 0 && prev_cmd->TextureId == GetCurrentTextureId() && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->ClipRect = curr_clip_rect;
}

void ImDrawList::UpdateTextureID()
{
    // If current command is used with different settings we need to add a new command
    const ImTextureID curr_texture_id = GetCurrentTextureId();
    ImDrawCmd* curr_cmd = CmdBuffer.Size ? &CmdBuffer.back() : NULL;
    if (!curr_cmd || (curr_cmd->ElemCount != 0 && curr_cmd->TextureId != curr_texture_id) || curr_cmd->UserCallback != NULL)
    {
        AddDrawCmd();
        return;
    }

    // Try to merge with previous command if it matches, else use current command
    ImDrawCmd* prev_cmd = CmdBuffer.Size > 1 ? curr_cmd - 1 : NULL;
    if (prev_cmd && prev_cmd->TextureId == curr_texture_id && memcmp(&prev_cmd->ClipRect, &GetCurrentClipRect(), sizeof(ImVec4)) == 0 && prev_cmd->UserCallback == NULL)
        CmdBuffer.pop_back();
    else
        curr_cmd->TextureId = curr_texture_id;
}

#undef GetCurrentClipRect
#undef GetCurrentTextureId

// Scissoring. The values in clip_rect are x1, y1, x2, y2. Only apply to rendering! Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
void ImDrawList::PushClipRect(const ImVec4& clip_rect)
{
    _ClipRectStack.push_back(clip_rect);
    UpdateClipRect();
}

void ImDrawList::PushClipRectFullScreen()
{
    PushClipRect(GNullClipRect);

    // FIXME-OPT: This would be more correct but we're not supposed to access ImGuiState from here?
    //ImGuiState& g = *GImGui;
    //PushClipRect(GetVisibleRect());
}

void ImDrawList::PopClipRect()
{
    IM_ASSERT(_ClipRectStack.Size > 0);
    _ClipRectStack.pop_back();
    UpdateClipRect();
}

void ImDrawList::PushTextureID(const ImTextureID& texture_id)
{
    _TextureIdStack.push_back(texture_id);
    UpdateTextureID();
}

void ImDrawList::PopTextureID()
{
    IM_ASSERT(_TextureIdStack.Size > 0);
    _TextureIdStack.pop_back();
    UpdateTextureID();
}

void ImDrawList::ChannelsSplit(int channels_count)
{
    IM_ASSERT(_ChannelsCurrent == 0 && _ChannelsCount == 1);
    int old_channels_count = _Channels.Size;
    if (old_channels_count < channels_count)
        _Channels.resize(channels_count);
    _ChannelsCount = channels_count;

    // _Channels[] (24 bytes each) hold storage that we'll swap with this->_CmdBuffer/_IdxBuffer
    // The content of _Channels[0] at this point doesn't matter. We clear it to make state tidy in a debugger but we don't strictly need to.
    // When we switch to the next channel, we'll copy _CmdBuffer/_IdxBuffer into _Channels[0] and then _Channels[1] into _CmdBuffer/_IdxBuffer
    memset(&_Channels[0], 0, sizeof(ImDrawChannel));
    for (int i = 1; i < channels_count; i++)
    {
        if (i >= old_channels_count)
        {
            IM_PLACEMENT_NEW(&_Channels[i]) ImDrawChannel();
        }
        else
        {
            _Channels[i].CmdBuffer.resize(0);
            _Channels[i].IdxBuffer.resize(0);
        }
        if (_Channels[i].CmdBuffer.Size == 0)
        {
            ImDrawCmd draw_cmd;
            draw_cmd.ClipRect = _ClipRectStack.back();
            draw_cmd.TextureId = _TextureIdStack.back();
            _Channels[i].CmdBuffer.push_back(draw_cmd);
        }
    }
}

void ImDrawList::ChannelsMerge()
{
    // Note that we never use or rely on channels.Size because it is merely a buffer that we never shrink back to 0 to keep all sub-buffers ready for use.
    if (_ChannelsCount <= 1)
        return;

    ChannelsSetCurrent(0);
    if (CmdBuffer.Size && CmdBuffer.back().ElemCount == 0)
        CmdBuffer.pop_back();

    int new_cmd_buffer_count = 0, new_idx_buffer_count = 0;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (ch.CmdBuffer.Size && ch.CmdBuffer.back().ElemCount == 0)
            ch.CmdBuffer.pop_back();
        new_cmd_buffer_count += ch.CmdBuffer.Size;
        new_idx_buffer_count += ch.IdxBuffer.Size;
    }
    CmdBuffer.resize(CmdBuffer.Size + new_cmd_buffer_count);
    IdxBuffer.resize(IdxBuffer.Size + new_idx_buffer_count);

    ImDrawCmd* cmd_write = CmdBuffer.Data + CmdBuffer.Size - new_cmd_buffer_count;
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size - new_idx_buffer_count;
    for (int i = 1; i < _ChannelsCount; i++)
    {
        ImDrawChannel& ch = _Channels[i];
        if (int sz = ch.CmdBuffer.Size) { memcpy(cmd_write, ch.CmdBuffer.Data, sz * sizeof(ImDrawCmd)); cmd_write += sz; }
        if (int sz = ch.IdxBuffer.Size) { memcpy(_IdxWritePtr, ch.IdxBuffer.Data, sz * sizeof(ImDrawIdx)); _IdxWritePtr += sz; }
    }
    AddDrawCmd();
    _ChannelsCount = 1;
}

void ImDrawList::ChannelsSetCurrent(int idx)
{
    IM_ASSERT(idx < _ChannelsCount);
    if (_ChannelsCurrent == idx) return;
    memcpy(&_Channels.Data[_ChannelsCurrent].CmdBuffer, &CmdBuffer, sizeof(CmdBuffer)); // copy 12 bytes, four times
    memcpy(&_Channels.Data[_ChannelsCurrent].IdxBuffer, &IdxBuffer, sizeof(IdxBuffer));
    _ChannelsCurrent = idx;
    memcpy(&CmdBuffer, &_Channels.Data[_ChannelsCurrent].CmdBuffer, sizeof(CmdBuffer));
    memcpy(&IdxBuffer, &_Channels.Data[_ChannelsCurrent].IdxBuffer, sizeof(IdxBuffer));
    _IdxWritePtr = IdxBuffer.Data + IdxBuffer.Size;
}

// NB: this can be called with negative count for removing primitives (as long as the result does not underflow)
void ImDrawList::PrimReserve(int idx_count, int vtx_count)
{
    ImDrawCmd& draw_cmd = CmdBuffer.Data[CmdBuffer.Size-1];
    draw_cmd.ElemCount += idx_count;

    int vtx_buffer_size = VtxBuffer.Size;
    VtxBuffer.resize(vtx_buffer_size + vtx_count);
    _VtxWritePtr = VtxBuffer.Data + vtx_buffer_size;

    int idx_buffer_size = IdxBuffer.Size;
    IdxBuffer.resize(idx_buffer_size + idx_count);
    _IdxWritePtr = IdxBuffer.Data + idx_buffer_size;
}

// Fully unrolled with inline call to keep our debug builds decently fast.
void ImDrawList::PrimRect(const ImVec2& a, const ImVec2& c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv(GImGui->FontTexUvWhitePixel);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimRectUV(const ImVec2& a, const ImVec2& c, const ImVec2& uv_a, const ImVec2& uv_c, ImU32 col)
{
    ImVec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

void ImDrawList::PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col)
{
    ImDrawIdx idx = (ImDrawIdx)_VtxCurrentIdx;
    _IdxWritePtr[0] = idx; _IdxWritePtr[1] = (ImDrawIdx)(idx+1); _IdxWritePtr[2] = (ImDrawIdx)(idx+2);
    _IdxWritePtr[3] = idx; _IdxWritePtr[4] = (ImDrawIdx)(idx+2); _IdxWritePtr[5] = (ImDrawIdx)(idx+3);
    _VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
    _VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
    _VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
    _VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
    _VtxWritePtr += 4;
    _VtxCurrentIdx += 4;
    _IdxWritePtr += 6;
}

// TODO: Thickness anti-aliased lines cap are missing their AA fringe.
void ImDrawList::AddPolyline(const ImVec2* points, const int points_count, ImU32 col, bool closed, float thickness, bool anti_aliased)
{
    if (points_count < 2)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedLines;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    int count = points_count;
    if (!closed)
        count = points_count-1;

    const bool thick_line = thickness > 1.0f;
    if (anti_aliased)
    {
        // Anti-aliased stroke
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & 0x00ffffff;

        const int idx_count = thick_line ? count*18 : count*12;
        const int vtx_count = thick_line ? points_count*4 : points_count*3;
        PrimReserve(idx_count, vtx_count);

        // Temporary buffer
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(ImVec2));
        ImVec2* temp_points = temp_normals + points_count;

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            ImVec2 diff = points[i2] - points[i1];
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i1].x = diff.y;
            temp_normals[i1].y = -diff.x;
        }
        if (!closed)
            temp_normals[points_count-1] = temp_normals[points_count-2];

        if (!thick_line)
        {
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
                temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
                temp_points[(points_count-1)*2+0] = points[points_count-1] + temp_normals[points_count-1] * AA_SIZE;
                temp_points[(points_count-1)*2+1] = points[points_count-1] - temp_normals[points_count-1] * AA_SIZE;
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+3;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                dm *= AA_SIZE;
                temp_points[i2*2+0] = points[i2] + dm;
                temp_points[i2*2+1] = points[i2] - dm;

                // Add indexes
                _IdxWritePtr[0] = (ImDrawIdx)(idx2+0); _IdxWritePtr[1] = (ImDrawIdx)(idx1+0); _IdxWritePtr[2] = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3] = (ImDrawIdx)(idx1+2); _IdxWritePtr[4] = (ImDrawIdx)(idx2+2); _IdxWritePtr[5] = (ImDrawIdx)(idx2+0);
                _IdxWritePtr[6] = (ImDrawIdx)(idx2+1); _IdxWritePtr[7] = (ImDrawIdx)(idx1+1); _IdxWritePtr[8] = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9] = (ImDrawIdx)(idx1+0); _IdxWritePtr[10]= (ImDrawIdx)(idx2+0); _IdxWritePtr[11]= (ImDrawIdx)(idx2+1);
                _IdxWritePtr += 12;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
                _VtxWritePtr[1].pos = temp_points[i*2+0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
                _VtxWritePtr[2].pos = temp_points[i*2+1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
                _VtxWritePtr += 3;
            }
        }
        else
        {
            const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
            if (!closed)
            {
                temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
                temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
                temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+0] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
                temp_points[(points_count-1)*4+1] = points[points_count-1] + temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+2] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness);
                temp_points[(points_count-1)*4+3] = points[points_count-1] - temp_normals[points_count-1] * (half_inner_thickness + AA_SIZE);
            }

            // FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
            unsigned int idx1 = _VtxCurrentIdx;
            for (int i1 = 0; i1 < count; i1++)
            {
                const int i2 = (i1+1) == points_count ? 0 : i1+1;
                unsigned int idx2 = (i1+1) == points_count ? _VtxCurrentIdx : idx1+4;

                // Average normals
                ImVec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
                float dmr2 = dm.x*dm.x + dm.y*dm.y;
                if (dmr2 > 0.000001f)
                {
                    float scale = 1.0f / dmr2;
                    if (scale > 100.0f) scale = 100.0f;
                    dm *= scale;
                }
                ImVec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
                ImVec2 dm_in = dm * half_inner_thickness;
                temp_points[i2*4+0] = points[i2] + dm_out;
                temp_points[i2*4+1] = points[i2] + dm_in;
                temp_points[i2*4+2] = points[i2] - dm_in;
                temp_points[i2*4+3] = points[i2] - dm_out;

                // Add indexes
                _IdxWritePtr[0]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[1]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[2]  = (ImDrawIdx)(idx1+2);
                _IdxWritePtr[3]  = (ImDrawIdx)(idx1+2); _IdxWritePtr[4]  = (ImDrawIdx)(idx2+2); _IdxWritePtr[5]  = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[6]  = (ImDrawIdx)(idx2+1); _IdxWritePtr[7]  = (ImDrawIdx)(idx1+1); _IdxWritePtr[8]  = (ImDrawIdx)(idx1+0);
                _IdxWritePtr[9]  = (ImDrawIdx)(idx1+0); _IdxWritePtr[10] = (ImDrawIdx)(idx2+0); _IdxWritePtr[11] = (ImDrawIdx)(idx2+1);
                _IdxWritePtr[12] = (ImDrawIdx)(idx2+2); _IdxWritePtr[13] = (ImDrawIdx)(idx1+2); _IdxWritePtr[14] = (ImDrawIdx)(idx1+3);
                _IdxWritePtr[15] = (ImDrawIdx)(idx1+3); _IdxWritePtr[16] = (ImDrawIdx)(idx2+3); _IdxWritePtr[17] = (ImDrawIdx)(idx2+2);
                _IdxWritePtr += 18;

                idx1 = idx2;
            }

            // Add vertexes
            for (int i = 0; i < points_count; i++)
            {
                _VtxWritePtr[0].pos = temp_points[i*4+0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
                _VtxWritePtr[1].pos = temp_points[i*4+1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
                _VtxWritePtr[2].pos = temp_points[i*4+2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
                _VtxWritePtr[3].pos = temp_points[i*4+3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
                _VtxWritePtr += 4;
            }
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Stroke
        const int idx_count = count*6;
        const int vtx_count = count*4;      // FIXME-OPT: Not sharing edges
        PrimReserve(idx_count, vtx_count);

        for (int i1 = 0; i1 < count; i1++)
        {
            const int i2 = (i1+1) == points_count ? 0 : i1+1;
            const ImVec2& p1 = points[i1];
            const ImVec2& p2 = points[i2];
            ImVec2 diff = p2 - p1;
            diff *= ImInvLength(diff, 1.0f);

            const float dx = diff.x * (thickness * 0.5f);
            const float dy = diff.y * (thickness * 0.5f);
            _VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
            _VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
            _VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
            _VtxWritePtr += 4;

            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+2);
            _IdxWritePtr[3] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (ImDrawIdx)(_VtxCurrentIdx+2); _IdxWritePtr[5] = (ImDrawIdx)(_VtxCurrentIdx+3);
            _IdxWritePtr += 6;
            _VtxCurrentIdx += 4;
        }
    }
}

void ImDrawList::AddConvexPolyFilled(const ImVec2* points, const int points_count, ImU32 col, bool anti_aliased)
{
    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    anti_aliased &= GImGui->Style.AntiAliasedShapes;
    //if (ImGui::GetIO().KeyCtrl) anti_aliased = false; // Debug

    if (anti_aliased)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const ImU32 col_trans = col & 0x00ffffff;
        const int idx_count = (points_count-2)*3 + points_count*6;
        const int vtx_count = (points_count*2);
        PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = _VtxCurrentIdx;
        unsigned int vtx_outer_idx = _VtxCurrentIdx+1;
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+((i-1)<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx+(i<<1));
            _IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2));
        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 diff = p1 - p0;
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i0].x = diff.y;
            temp_normals[i0].y = -diff.x;
        }

        for (int i0 = points_count-1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            ImVec2 dm = (n0 + n1) * 0.5f;
            float dmr2 = dm.x*dm.x + dm.y*dm.y;
            if (dmr2 > 0.000001f)
            {
                float scale = 1.0f / dmr2;
                if (scale > 100.0f) scale = 100.0f;
                dm *= scale;
            }
            dm *= AA_SIZE * 0.5f;

            // Add vertices
            _VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
            _VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
            _VtxWritePtr += 2;

            // Add indexes for fringes
            _IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx+(i1<<1)); _IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx+(i0<<1)); _IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx+(i0<<1));
            _IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx+(i0<<1)); _IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx+(i1<<1)); _IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx+(i1<<1));
            _IdxWritePtr += 6;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count-2)*3;
        const int vtx_count = points_count;
        PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            _VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
            _VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            _IdxWritePtr[0] = (ImDrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (ImDrawIdx)(_VtxCurrentIdx+i-1); _IdxWritePtr[2] = (ImDrawIdx)(_VtxCurrentIdx+i);
            _IdxWritePtr += 3;
        }
        _VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}

void ImDrawList::PathArcToFast(const ImVec2& centre, float radius, int amin, int amax)
{
    static ImVec2 circle_vtx[12];
    static bool circle_vtx_builds = false;
    const int circle_vtx_count = IM_ARRAYSIZE(circle_vtx);
    if (!circle_vtx_builds)
    {
        for (int i = 0; i < circle_vtx_count; i++)
        {
            const float a = ((float)i / (float)circle_vtx_count) * 2*IM_PI;
            circle_vtx[i].x = cosf(a);
            circle_vtx[i].y = sinf(a);
        }
        circle_vtx_builds = true;
    }

    if (amin > amax) return;
    if (radius == 0.0f)
    {
        _Path.push_back(centre);
    }
    else
    {
        _Path.reserve(_Path.Size + (amax - amin + 1));
        for (int a = amin; a <= amax; a++)
        {
            const ImVec2& c = circle_vtx[a % circle_vtx_count];
            _Path.push_back(ImVec2(centre.x + c.x * radius, centre.y + c.y * radius));
        }
    }
}

void ImDrawList::PathArcTo(const ImVec2& centre, float radius, float amin, float amax, int num_segments)
{
    if (radius == 0.0f)
        _Path.push_back(centre);
    _Path.reserve(_Path.Size + (num_segments + 1));
    for (int i = 0; i <= num_segments; i++)
    {
        const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
        _Path.push_back(ImVec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
    }
}

static void PathBezierToCasteljau(ImVector<ImVec2>* path, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level)
{
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = ((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = ((x3 - x4) * dy - (y3 - y4) * dx);
    d2 = (d2 >= 0) ? d2 : -d2;
    d3 = (d3 >= 0) ? d3 : -d3;
    if ((d2+d3) * (d2+d3) < tess_tol * (dx*dx + dy*dy))
    {
        path->push_back(ImVec2(x4, y4));
    }
    else if (level < 10)
    {
        float x12 = (x1+x2)*0.5f,       y12 = (y1+y2)*0.5f;
        float x23 = (x2+x3)*0.5f,       y23 = (y2+y3)*0.5f;
        float x34 = (x3+x4)*0.5f,       y34 = (y3+y4)*0.5f;
        float x123 = (x12+x23)*0.5f,    y123 = (y12+y23)*0.5f;
        float x234 = (x23+x34)*0.5f,    y234 = (y23+y34)*0.5f;
        float x1234 = (x123+x234)*0.5f, y1234 = (y123+y234)*0.5f;

        PathBezierToCasteljau(path, x1,y1,        x12,y12,    x123,y123,  x1234,y1234, tess_tol, level+1);
        PathBezierToCasteljau(path, x1234,y1234,  x234,y234,  x34,y34,    x4,y4,       tess_tol, level+1);
    }
}

void ImDrawList::PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments)
{
    ImVec2 p1 = _Path.back();
    if (num_segments == 0)
    {
        // Auto-tessellated
        PathBezierToCasteljau(&_Path, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y, GImGui->Style.CurveTessellationTol, 0);
    }
    else
    {
        float t_step = 1.0f / (float)num_segments;
        for (int i_step = 1; i_step <= num_segments; i_step++)
        {
            float t = t_step * i_step;
            float u = 1.0f - t;
            float w1 = u*u*u;
            float w2 = 3*u*u*t;
            float w3 = 3*u*t*t;
            float w4 = t*t*t;
            _Path.push_back(ImVec2(w1*p1.x + w2*p2.x + w3*p3.x + w4*p4.x, w1*p1.y + w2*p2.y + w3*p3.y + w4*p4.y));
        }
    }
}

void ImDrawList::PathRect(const ImVec2& a, const ImVec2& b, float rounding, int rounding_corners)
{
    float r = rounding;
    r = ImMin(r, fabsf(b.x-a.x) * ( ((rounding_corners&(1|2))==(1|2)) || ((rounding_corners&(4|8))==(4|8)) ? 0.5f : 1.0f ) - 1.0f);
    r = ImMin(r, fabsf(b.y-a.y) * ( ((rounding_corners&(1|8))==(1|8)) || ((rounding_corners&(2|4))==(2|4)) ? 0.5f : 1.0f ) - 1.0f);

    if (r <= 0.0f || rounding_corners == 0)
    {
        PathLineTo(a);
        PathLineTo(ImVec2(b.x,a.y));
        PathLineTo(b);
        PathLineTo(ImVec2(a.x,b.y));
    }
    else
    {
        const float r0 = (rounding_corners & 1) ? r : 0.0f;
        const float r1 = (rounding_corners & 2) ? r : 0.0f;
        const float r2 = (rounding_corners & 4) ? r : 0.0f;
        const float r3 = (rounding_corners & 8) ? r : 0.0f;
        PathArcToFast(ImVec2(a.x+r0,a.y+r0), r0, 6, 9);
        PathArcToFast(ImVec2(b.x-r1,a.y+r1), r1, 9, 12);
        PathArcToFast(ImVec2(b.x-r2,b.y-r2), r2, 0, 3);
        PathArcToFast(ImVec2(a.x+r3,b.y-r3), r3, 3, 6);
    }
}

void ImDrawList::AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
{
    if ((col >> 24) == 0)
        return;
    PathLineTo(a + ImVec2(0.5f,0.5f));
    PathLineTo(b + ImVec2(0.5f,0.5f));
    PathStroke(col, false, thickness);
}

// a: upper-left, b: lower-right. we don't render 1 px sized rectangles properly.
void ImDrawList::AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners, float thickness)
{
    if ((col >> 24) == 0)
        return;
    PathRect(a + ImVec2(0.5f,0.5f), b - ImVec2(0.5f,0.5f), rounding, rounding_corners);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int rounding_corners)
{
    if ((col >> 24) == 0)
        return;
    if (rounding > 0.0f)
    {
        PathRect(a, b, rounding, rounding_corners);
        PathFill(col);
    }
    else
    {
        PrimReserve(6, 4);
        PrimRect(a, b, col);
    }
}

void ImDrawList::AddRectFilledMultiColor(const ImVec2& a, const ImVec2& c, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) >> 24) == 0)
        return;

    const ImVec2 uv = GImGui->FontTexUvWhitePixel;
    PrimReserve(6, 4);
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+1)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2));
    PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+2)); PrimWriteIdx((ImDrawIdx)(_VtxCurrentIdx+3));
    PrimWriteVtx(a, uv, col_upr_left);
    PrimWriteVtx(ImVec2(c.x, a.y), uv, col_upr_right);
    PrimWriteVtx(c, uv, col_bot_right);
    PrimWriteVtx(ImVec2(a.x, c.y), uv, col_bot_left);
}

void ImDrawList::AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness)
{
    if ((col >> 24) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col)
{
    if ((col >> 24) == 0)
        return;

    PathLineTo(a);
    PathLineTo(b);
    PathLineTo(c);
    PathFill(col);
}

void ImDrawList::AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments, float thickness)
{
    if ((col >> 24) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius-0.5f, 0.0f, a_max, num_segments);
    PathStroke(col, true, thickness);
}

void ImDrawList::AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments)
{
    if ((col >> 24) == 0)
        return;

    const float a_max = IM_PI*2.0f * ((float)num_segments - 1.0f) / (float)num_segments;
    PathArcTo(centre, radius, 0.0f, a_max, num_segments);
    PathFill(col);
}

void ImDrawList::AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments)
{
    if ((col >> 24) == 0)
        return;

    PathLineTo(pos0);
    PathBezierCurveTo(cp0, cp1, pos1, num_segments);
    PathStroke(col, false, thickness);
}

void ImDrawList::AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
    if ((col >> 24) == 0)
        return;

    if (text_end == NULL)
        text_end = text_begin + strlen(text_begin);
    if (text_begin == text_end)
        return;

    // Note: This is one of the few instance of breaking the encapsulation of ImDrawList, as we pull this from ImGui state, but it is just SO useful.
    // Might just move Font/FontSize to ImDrawList?
    if (font == NULL)
        font = GImGui->Font;
    if (font_size == 0.0f)
        font_size = GImGui->FontSize;

    IM_ASSERT(font->ContainerAtlas->TexID == _TextureIdStack.back());  // Use high-level ImGui::PushFont() or low-level ImDrawList::PushTextureId() to change font.

    // reserve vertices for worse case (over-reserving is useful and easily amortized)
    const int char_count = (int)(text_end - text_begin);
    const int vtx_count_max = char_count * 4;
    const int idx_count_max = char_count * 6;
    const int vtx_begin = VtxBuffer.Size;
    const int idx_begin = IdxBuffer.Size;
    PrimReserve(idx_count_max, vtx_count_max);

    ImVec4 clip_rect = _ClipRectStack.back();
    if (cpu_fine_clip_rect)
    {
        clip_rect.x = ImMax(clip_rect.x, cpu_fine_clip_rect->x);
        clip_rect.y = ImMax(clip_rect.y, cpu_fine_clip_rect->y);
        clip_rect.z = ImMin(clip_rect.z, cpu_fine_clip_rect->z);
        clip_rect.w = ImMin(clip_rect.w, cpu_fine_clip_rect->w);
    }
    font->RenderText(font_size, pos, col, clip_rect, text_begin, text_end, this, wrap_width, cpu_fine_clip_rect != NULL);

    // give back unused vertices
    // FIXME-OPT: clean this up
    VtxBuffer.resize((int)(_VtxWritePtr - VtxBuffer.Data));
    IdxBuffer.resize((int)(_IdxWritePtr - IdxBuffer.Data));
    int vtx_unused = vtx_count_max - (VtxBuffer.Size - vtx_begin);
    int idx_unused = idx_count_max - (IdxBuffer.Size - idx_begin);
    CmdBuffer.back().ElemCount -= idx_unused;
    _VtxWritePtr -= vtx_unused;
    _IdxWritePtr -= idx_unused;
    _VtxCurrentIdx = (unsigned int)VtxBuffer.Size;
}

void ImDrawList::AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end)
{
    AddText(GImGui->Font, GImGui->FontSize, pos, col, text_begin, text_end);
}

void ImDrawList::AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv0, const ImVec2& uv1, ImU32 col)
{
    if ((col >> 24) == 0)
        return;

    // FIXME-OPT: This is wasting draw calls.
    const bool push_texture_id = _TextureIdStack.empty() || user_texture_id != _TextureIdStack.back();
    if (push_texture_id)
        PushTextureID(user_texture_id);

    PrimReserve(6, 4);
    PrimRectUV(a, b, uv0, uv1, col);

    if (push_texture_id)
        PopTextureID();
}

//-----------------------------------------------------------------------------
// ImDrawData
//-----------------------------------------------------------------------------

// For backward compatibility: convert all buffers from indexed to de-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
void ImDrawData::DeIndexAllBuffers()
{
    ImVector<ImDrawVert> new_vtx_buffer;
    TotalVtxCount = TotalIdxCount = 0;
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        if (cmd_list->IdxBuffer.empty())
            continue;
        new_vtx_buffer.resize(cmd_list->IdxBuffer.Size);
        for (int j = 0; j < cmd_list->IdxBuffer.Size; j++)
            new_vtx_buffer[j] = cmd_list->VtxBuffer[cmd_list->IdxBuffer[j]];
        cmd_list->VtxBuffer.swap(new_vtx_buffer);
        cmd_list->IdxBuffer.resize(0);
        TotalVtxCount += cmd_list->VtxBuffer.Size;
    }
}

// Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than ImGui expects, or if there is a difference between your window resolution and framebuffer resolution.
void ImDrawData::ScaleClipRects(const ImVec2& scale)
{
    for (int i = 0; i < CmdListsCount; i++)
    {
        ImDrawList* cmd_list = CmdLists[i];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            ImDrawCmd* cmd = &cmd_list->CmdBuffer[cmd_i];
            cmd->ClipRect = ImVec4(cmd->ClipRect.x * scale.x, cmd->ClipRect.y * scale.y, cmd->ClipRect.z * scale.x, cmd->ClipRect.w * scale.y);
        }
    }
}

//-----------------------------------------------------------------------------
// ImFontAtlas
//-----------------------------------------------------------------------------

ImFontConfig::ImFontConfig()
{
    FontData = NULL;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 0;
    OversampleV = 0;
    PixelSnapH = false;
    GlyphExtraSpacing = ImVec2(0.0f, 0.0f);
    GlyphRanges = NULL;
    MergeMode = false;
    MergeGlyphCenterV = false;
    DstFont = NULL;
    memset(Name, 0, sizeof(Name));
}

ImFontAtlas::ImFontAtlas()
{
    TexID = NULL;
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
    TexWidth = TexHeight = TexDesiredWidth = 0;
    TexUvWhitePixel = ImVec2(0, 0);
}

ImFontAtlas::~ImFontAtlas()
{
    Clear();
}

void    ImFontAtlas::ClearInputData()
{
    for (int i = 0; i < ConfigData.Size; i++)
        if (ConfigData[i].FontData && ConfigData[i].FontDataOwnedByAtlas)
        {
            ImGui::MemFree(ConfigData[i].FontData);
            ConfigData[i].FontData = NULL;
        }

    // When clearing this we lose access to the font name and other information used to build the font.
    for (int i = 0; i < Fonts.Size; i++)
        if (Fonts[i]->ConfigData >= ConfigData.Data && Fonts[i]->ConfigData < ConfigData.Data + ConfigData.Size)
        {
            Fonts[i]->ConfigData = NULL;
            Fonts[i]->ConfigDataCount = 0;
        }
    ConfigData.clear();
}

void    ImFontAtlas::ClearTexData()
{
    if (TexPixelsAlpha8)
        ImGui::MemFree(TexPixelsAlpha8);
    if (TexPixelsRGBA32)
        ImGui::MemFree(TexPixelsRGBA32);
    TexPixelsAlpha8 = NULL;
    TexPixelsRGBA32 = NULL;
}

void    ImFontAtlas::ClearFonts()
{
    for (int i = 0; i < Fonts.Size; i++)
    {
        Fonts[i]->~ImFont();
        ImGui::MemFree(Fonts[i]);
    }
    Fonts.clear();
}

void    ImFontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}

void    ImFontAtlas::GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Build atlas on demand
    if (TexPixelsAlpha8 == NULL)
    {
        if (ConfigData.empty())
            AddFontDefault();
        Build();
    }

    *out_pixels = TexPixelsAlpha8;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void    ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
    // Convert to RGBA32 format on demand
    // Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
    if (!TexPixelsRGBA32)
    {
        unsigned char* pixels;
        GetTexDataAsAlpha8(&pixels, NULL, NULL);
        TexPixelsRGBA32 = (unsigned int*)ImGui::MemAlloc((size_t)(TexWidth * TexHeight * 4));
        const unsigned char* src = pixels;
        unsigned int* dst = TexPixelsRGBA32;
        for (int n = TexWidth * TexHeight; n > 0; n--)
            *dst++ = ((unsigned int)(*src++) << 24) | 0x00FFFFFF;
    }

    *out_pixels = (unsigned char*)TexPixelsRGBA32;
    if (out_width) *out_width = TexWidth;
    if (out_height) *out_height = TexHeight;
    if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

ImFont* ImFontAtlas::AddFont(const ImFontConfig* font_cfg)
{
    IM_ASSERT(font_cfg->FontData != NULL && font_cfg->FontDataSize > 0);
    IM_ASSERT(font_cfg->SizePixels > 0.0f);

    // Create new font
    if (!font_cfg->MergeMode)
    {
        ImFont* font = (ImFont*)ImGui::MemAlloc(sizeof(ImFont));
        IM_PLACEMENT_NEW(font) ImFont();
        Fonts.push_back(font);
    }

    ConfigData.push_back(*font_cfg);
    ImFontConfig& new_font_cfg = ConfigData.back();
    new_font_cfg.DstFont = Fonts.back();
    if (!new_font_cfg.FontDataOwnedByAtlas)
    {
        new_font_cfg.FontData = ImGui::MemAlloc(new_font_cfg.FontDataSize);
        new_font_cfg.FontDataOwnedByAtlas = true;
        memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
    }

    // Invalidate texture
    ClearTexData();
    return Fonts.back();
}

// Default font TTF is compressed with stb_compress then base85 encoded (see extra_fonts/binary_to_compressed_c.cpp for encoder)
static unsigned int stb_decompress_length(unsigned char *input);
static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length);
static const char*  GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c)                                    { return c >= '\\' ? c-36 : c-35; }
static void         Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0]) + 85*(Decode85Byte(src[1]) + 85*(Decode85Byte(src[2]) + 85*(Decode85Byte(src[3]) + 85*Decode85Byte(src[4]))));
        dst[0] = ((tmp >> 0) & 0xFF); dst[1] = ((tmp >> 8) & 0xFF); dst[2] = ((tmp >> 16) & 0xFF); dst[3] = ((tmp >> 24) & 0xFF);   // We can't assume little-endianess.
        src += 5;
        dst += 4;
    }
}

// Load embedded ProggyClean.ttf at size 13, disable oversampling
ImFont* ImFontAtlas::AddFontDefault(const ImFontConfig* font_cfg_template)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (!font_cfg_template)
    {
        font_cfg.OversampleH = font_cfg.OversampleV = 1;
        font_cfg.PixelSnapH = true;
    }
    if (font_cfg.Name[0] == '\0') strcpy(font_cfg.Name, "<default>");

    const char* ttf_compressed_base85 = GetDefaultCompressedFontDataTTFBase85();
    ImFont* font = AddFontFromMemoryCompressedBase85TTF(ttf_compressed_base85, 15.0f, &font_cfg, GetGlyphRangesDefault());
    return font;
}

ImFont* ImFontAtlas::AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    int data_size = 0;
    void* data = ImLoadFileToMemory(filename, "rb", &data_size, 0);
    if (!data)
    {
        IM_ASSERT(0); // Could not load file.
        return NULL;
    }
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    if (font_cfg.Name[0] == '\0')
    {
        // Store a short copy of filename into into the font name for convenience
        const char* p;
        for (p = filename + strlen(filename); p > filename && p[-1] != '/' && p[-1] != '\\'; p--) {}
        snprintf(font_cfg.Name, IM_ARRAYSIZE(font_cfg.Name), "%s", p);
    }
    return AddFontFromMemoryTTF(data, data_size, size_pixels, &font_cfg, glyph_ranges);
}

// NBM Transfer ownership of 'ttf_data' to ImFontAtlas, unless font_cfg_template->FontDataOwnedByAtlas == false. Owned TTF buffer will be deleted after Build().
ImFont* ImFontAtlas::AddFontFromMemoryTTF(void* ttf_data, int ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontData = ttf_data;
    font_cfg.FontDataSize = ttf_size;
    font_cfg.SizePixels = size_pixels;
    if (glyph_ranges)
        font_cfg.GlyphRanges = glyph_ranges;
    return AddFont(&font_cfg);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedTTF(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((unsigned char*)compressed_ttf_data);
    unsigned char* buf_decompressed_data = (unsigned char *)ImGui::MemAlloc(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

    ImFontConfig font_cfg = font_cfg_template ? *font_cfg_template : ImFontConfig();
    IM_ASSERT(font_cfg.FontData == NULL);
    font_cfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, font_cfg_template, glyph_ranges);
}

ImFont* ImFontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressed_ttf_data_base85, float size_pixels, const ImFontConfig* font_cfg, const ImWchar* glyph_ranges)
{
    int compressed_ttf_size = (((int)strlen(compressed_ttf_data_base85) + 4) / 5) * 4;
    void* compressed_ttf = ImGui::MemAlloc((size_t)compressed_ttf_size);
    Decode85((const unsigned char*)compressed_ttf_data_base85, (unsigned char*)compressed_ttf);
    ImFont* font = AddFontFromMemoryCompressedTTF(compressed_ttf, compressed_ttf_size, size_pixels, font_cfg, glyph_ranges);
    ImGui::MemFree(compressed_ttf);
    return font;
}

bool    ImFontAtlas::Build()
{
    IM_ASSERT(ConfigData.Size > 0);

    TexID = NULL;
    TexWidth = TexHeight = 0;
    TexUvWhitePixel = ImVec2(0, 0);
    ClearTexData();

    struct ImFontTempBuildData
    {
        stbtt_fontinfo      FontInfo;
        stbrp_rect*         Rects;
        stbtt_pack_range*   Ranges;
        int                 RangesCount;
    };
    ImFontTempBuildData* tmp_array = (ImFontTempBuildData*)ImGui::MemAlloc((size_t)ConfigData.Size * sizeof(ImFontTempBuildData));

    // Initialize font information early (so we can error without any cleanup) + count glyphs
    int total_glyph_count = 0;
    int total_glyph_range_count = 0;
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];

        IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == this));
        const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
        IM_ASSERT(font_offset >= 0);
        if (!stbtt_InitFont(&tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
            return false;

        // Count glyphs
        if (!cfg.GlyphRanges)
            cfg.GlyphRanges = GetGlyphRangesDefault();
        for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
        {
            total_glyph_count += (in_range[1] - in_range[0]) + 1;
            total_glyph_range_count++;
        }
    }

    // Start packing. We need a known width for the skyline algorithm. Using a cheap heuristic here to decide of width. User can override TexDesiredWidth if they wish.
    // After packing is done, width shouldn't matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
    TexWidth = (TexDesiredWidth > 0) ? TexDesiredWidth : (total_glyph_count > 4000) ? 4096 : (total_glyph_count > 2000) ? 2048 : (total_glyph_count > 1000) ? 1024 : 512;
    TexHeight = 0;
    const int max_tex_height = 1024*32;
    stbtt_pack_context spc;
    stbtt_PackBegin(&spc, NULL, TexWidth, max_tex_height, 0, 1, NULL);

    // Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
    ImVector<stbrp_rect> extra_rects;
    RenderCustomTexData(0, &extra_rects);
    stbtt_PackSetOversampling(&spc, 1, 1);
    stbrp_pack_rects((stbrp_context*)spc.pack_info, &extra_rects[0], extra_rects.Size);
    for (int i = 0; i < extra_rects.Size; i++)
        if (extra_rects[i].was_packed)
            TexHeight = ImMax(TexHeight, extra_rects[i].y + extra_rects[i].h);

    // Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
    int buf_packedchars_n = 0, buf_rects_n = 0, buf_ranges_n = 0;
    stbtt_packedchar* buf_packedchars = (stbtt_packedchar*)ImGui::MemAlloc(total_glyph_count * sizeof(stbtt_packedchar));
    stbrp_rect* buf_rects = (stbrp_rect*)ImGui::MemAlloc(total_glyph_count * sizeof(stbrp_rect));
    stbtt_pack_range* buf_ranges = (stbtt_pack_range*)ImGui::MemAlloc(total_glyph_range_count * sizeof(stbtt_pack_range));
    memset(buf_packedchars, 0, total_glyph_count * sizeof(stbtt_packedchar));
    memset(buf_rects, 0, total_glyph_count * sizeof(stbrp_rect));              // Unnecessary but let's clear this for the sake of sanity.
    memset(buf_ranges, 0, total_glyph_range_count * sizeof(stbtt_pack_range));

    // First font pass: pack all glyphs (no rendering at this point, we are working with rectangles in an infinitely tall texture at this point)
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];

        // Setup ranges
        int glyph_count = 0;
        int glyph_ranges_count = 0;
        for (const ImWchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
        {
            glyph_count += (in_range[1] - in_range[0]) + 1;
            glyph_ranges_count++;
        }
        tmp.Ranges = buf_ranges + buf_ranges_n;
        tmp.RangesCount = glyph_ranges_count;
        buf_ranges_n += glyph_ranges_count;
        for (int i = 0; i < glyph_ranges_count; i++)
        {
            const ImWchar* in_range = &cfg.GlyphRanges[i * 2];
            stbtt_pack_range& range = tmp.Ranges[i];
            range.font_size = cfg.SizePixels;
            range.first_unicode_codepoint_in_range = in_range[0];
            range.num_chars = (in_range[1] - in_range[0]) + 1;
            range.chardata_for_range = buf_packedchars + buf_packedchars_n;
            buf_packedchars_n += range.num_chars;
        }

        // Pack
        tmp.Rects = buf_rects + buf_rects_n;
        buf_rects_n += glyph_count;
        stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
        int n = stbtt_PackFontRangesGatherRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
        stbrp_pack_rects((stbrp_context*)spc.pack_info, tmp.Rects, n);

        // Extend texture height
        for (int i = 0; i < n; i++)
            if (tmp.Rects[i].was_packed)
                TexHeight = ImMax(TexHeight, tmp.Rects[i].y + tmp.Rects[i].h);
    }
    IM_ASSERT(buf_rects_n == total_glyph_count);
    IM_ASSERT(buf_packedchars_n == total_glyph_count);
    IM_ASSERT(buf_ranges_n == total_glyph_range_count);

    // Create texture
    TexHeight = ImUpperPowerOfTwo(TexHeight);
    TexPixelsAlpha8 = (unsigned char*)ImGui::MemAlloc(TexWidth * TexHeight);
    memset(TexPixelsAlpha8, 0, TexWidth * TexHeight);
    spc.pixels = TexPixelsAlpha8;
    spc.height = TexHeight;

    // Second pass: render characters
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];
        stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
        stbtt_PackFontRangesRenderIntoRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
        tmp.Rects = NULL;
    }

    // End packing
    stbtt_PackEnd(&spc);
    ImGui::MemFree(buf_rects);
    buf_rects = NULL;

    // Third pass: setup ImFont and glyphs for runtime
    for (int input_i = 0; input_i < ConfigData.Size; input_i++)
    {
        ImFontConfig& cfg = ConfigData[input_i];
        ImFontTempBuildData& tmp = tmp_array[input_i];
        ImFont* dst_font = cfg.DstFont;

        float font_scale = stbtt_ScaleForPixelHeight(&tmp.FontInfo, cfg.SizePixels);
        int unscaled_ascent, unscaled_descent, unscaled_line_gap;
        stbtt_GetFontVMetrics(&tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

        float ascent = unscaled_ascent * font_scale;
        float descent = unscaled_descent * font_scale;
        if (!cfg.MergeMode)
        {
            dst_font->ContainerAtlas = this;
            dst_font->ConfigData = &cfg;
            dst_font->ConfigDataCount = 0;
            dst_font->FontSize = cfg.SizePixels;
            dst_font->Ascent = ascent;
            dst_font->Descent = descent;
            dst_font->Glyphs.resize(0);
        }
        dst_font->ConfigDataCount++;
        float off_y = (cfg.MergeMode && cfg.MergeGlyphCenterV) ? (ascent - dst_font->Ascent) * 0.5f : 0.0f;

        dst_font->FallbackGlyph = NULL; // Always clear fallback so FindGlyph can return NULL. It will be set again in BuildLookupTable()
        for (int i = 0; i < tmp.RangesCount; i++)
        {
            stbtt_pack_range& range = tmp.Ranges[i];
            for (int char_idx = 0; char_idx < range.num_chars; char_idx += 1)
            {
                const stbtt_packedchar& pc = range.chardata_for_range[char_idx];
                if (!pc.x0 && !pc.x1 && !pc.y0 && !pc.y1)
                    continue;

                const int codepoint = range.first_unicode_codepoint_in_range + char_idx;
                if (cfg.MergeMode && dst_font->FindGlyph((unsigned short)codepoint))
                    continue;

                stbtt_aligned_quad q;
                float dummy_x = 0.0f, dummy_y = 0.0f;
                stbtt_GetPackedQuad(range.chardata_for_range, TexWidth, TexHeight, char_idx, &dummy_x, &dummy_y, &q, 0);

                dst_font->Glyphs.resize(dst_font->Glyphs.Size + 1);
                ImFont::Glyph& glyph = dst_font->Glyphs.back();
                glyph.Codepoint = (ImWchar)codepoint;
                glyph.X0 = q.x0; glyph.Y0 = q.y0; glyph.X1 = q.x1; glyph.Y1 = q.y1;
                glyph.U0 = q.s0; glyph.V0 = q.t0; glyph.U1 = q.s1; glyph.V1 = q.t1;
                glyph.Y0 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
                glyph.Y1 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
                glyph.XAdvance = (pc.xadvance + cfg.GlyphExtraSpacing.x);  // Bake spacing into XAdvance
                if (cfg.PixelSnapH)
                    glyph.XAdvance = (float)(int)(glyph.XAdvance + 0.5f);
            }
        }
        cfg.DstFont->BuildLookupTable();
    }

    // Cleanup temporaries
    ImGui::MemFree(buf_packedchars);
    ImGui::MemFree(buf_ranges);
    ImGui::MemFree(tmp_array);

    // Render into our custom data block
    RenderCustomTexData(1, &extra_rects);

    return true;
}

void ImFontAtlas::RenderCustomTexData(int pass, void* p_rects)
{
    // A work of art lies ahead! (. = white layer, X = black layer, others are blank)
    // The white texels on the top left are the ones we'll use everywhere in ImGui to render filled shapes.
    const int TEX_DATA_W = 90;
    const int TEX_DATA_H = 27;
    const char texture_data[TEX_DATA_W*TEX_DATA_H+1] =
    {
        "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX"
        "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X"
        "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X"
        "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X"
        "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X"
        "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X"
        "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX"
        "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      "
        "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       "
        "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        "
        "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         "
        "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          "
        "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           "
        "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            "
        "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           "
        "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          "
        "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          "
        "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       ------------------------------------"
        "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           "
        "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           "
        "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           "
        "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           "
        "------------        -    X    -           X           -X.....................X-           "
        "                    ----------------------------------- X...XXXXXXXXXXXXX...X -           "
        "                                                      -  X..X           X..X  -           "
        "                                                      -   X.X           X.X   -           "
        "                                                      -    XX           XX    -           "
    };

    ImVector<stbrp_rect>& rects = *(ImVector<stbrp_rect>*)p_rects;
    if (pass == 0)
    {
        // Request rectangles
        stbrp_rect r;
        memset(&r, 0, sizeof(r));
        r.w = (TEX_DATA_W*2)+1;
        r.h = TEX_DATA_H+1;
        rects.push_back(r);
    }
    else if (pass == 1)
    {
        // Render/copy pixels
        const stbrp_rect& r = rects[0];
        for (int y = 0, n = 0; y < TEX_DATA_H; y++)
            for (int x = 0; x < TEX_DATA_W; x++, n++)
            {
                const int offset0 = (int)(r.x + x) + (int)(r.y + y) * TexWidth;
                const int offset1 = offset0 + 1 + TEX_DATA_W;
                TexPixelsAlpha8[offset0] = texture_data[n] == '.' ? 0xFF : 0x00;
                TexPixelsAlpha8[offset1] = texture_data[n] == 'X' ? 0xFF : 0x00;
            }
        const ImVec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
        TexUvWhitePixel = ImVec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);

        // Setup mouse cursors
        const ImVec2 cursor_datas[ImGuiMouseCursor_Count_][3] =
        {
            // Pos ........ Size ......... Offset ......
            { ImVec2(0,3),  ImVec2(12,19), ImVec2( 0, 0) }, // ImGuiMouseCursor_Arrow
            { ImVec2(13,0), ImVec2(7,16),  ImVec2( 4, 8) }, // ImGuiMouseCursor_TextInput
            { ImVec2(31,0), ImVec2(23,23), ImVec2(11,11) }, // ImGuiMouseCursor_Move
            { ImVec2(21,0), ImVec2( 9,23), ImVec2( 5,11) }, // ImGuiMouseCursor_ResizeNS
            { ImVec2(55,18),ImVec2(23, 9), ImVec2(11, 5) }, // ImGuiMouseCursor_ResizeEW
            { ImVec2(73,0), ImVec2(17,17), ImVec2( 9, 9) }, // ImGuiMouseCursor_ResizeNESW
            { ImVec2(55,0), ImVec2(17,17), ImVec2( 9, 9) }, // ImGuiMouseCursor_ResizeNWSE
        };

        for (int type = 0; type < ImGuiMouseCursor_Count_; type++)
        {
            ImGuiMouseCursorData& cursor_data = GImGui->MouseCursorData[type];
            ImVec2 pos = cursor_datas[type][0] + ImVec2((float)r.x, (float)r.y);
            const ImVec2 size = cursor_datas[type][1];
            cursor_data.Type = type;
            cursor_data.Size = size;
            cursor_data.HotOffset = cursor_datas[type][2];
            cursor_data.TexUvMin[0] = (pos) * tex_uv_scale;
            cursor_data.TexUvMax[0] = (pos + size) * tex_uv_scale;
            pos.x += TEX_DATA_W+1;
            cursor_data.TexUvMin[1] = (pos) * tex_uv_scale;
            cursor_data.TexUvMax[1] = (pos + size) * tex_uv_scale;
        }
    }
}

// Retrieve list of range (2 int per range, values are inclusive)
const ImWchar*   ImFontAtlas::GetGlyphRangesDefault()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesKorean()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3131, 0x3163, // Korean alphabets
        0xAC00, 0xD79D, // Korean characters
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesChinese()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x4e00, 0x9FAF, // CJK Ideograms
        0,
    };
    return &ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesJapanese()
{
    // Store the 1946 ideograms code points as successive offsets from the initial unicode codepoint 0x4E00. Each offset has an implicit +1.
    // This encoding helps us reduce the source code size.
    static const short offsets_from_0x4E00[] =
    {
        -1,0,1,3,0,0,0,0,1,0,5,1,1,0,7,4,6,10,0,1,9,9,7,1,3,19,1,10,7,1,0,1,0,5,1,0,6,4,2,6,0,0,12,6,8,0,3,5,0,1,0,9,0,0,8,1,1,3,4,5,13,0,0,8,2,17,
        4,3,1,1,9,6,0,0,0,2,1,3,2,22,1,9,11,1,13,1,3,12,0,5,9,2,0,6,12,5,3,12,4,1,2,16,1,1,4,6,5,3,0,6,13,15,5,12,8,14,0,0,6,15,3,6,0,18,8,1,6,14,1,
        5,4,12,24,3,13,12,10,24,0,0,0,1,0,1,1,2,9,10,2,2,0,0,3,3,1,0,3,8,0,3,2,4,4,1,6,11,10,14,6,15,3,4,15,1,0,0,5,2,2,0,0,1,6,5,5,6,0,3,6,5,0,0,1,0,
        11,2,2,8,4,7,0,10,0,1,2,17,19,3,0,2,5,0,6,2,4,4,6,1,1,11,2,0,3,1,2,1,2,10,7,6,3,16,0,8,24,0,0,3,1,1,3,0,1,6,0,0,0,2,0,1,5,15,0,1,0,0,2,11,19,
        1,4,19,7,6,5,1,0,0,0,0,5,1,0,1,9,0,0,5,0,2,0,1,0,3,0,11,3,0,2,0,0,0,0,0,9,3,6,4,12,0,14,0,0,29,10,8,0,14,37,13,0,31,16,19,0,8,30,1,20,8,3,48,
        21,1,0,12,0,10,44,34,42,54,11,18,82,0,2,1,2,12,1,0,6,2,17,2,12,7,0,7,17,4,2,6,24,23,8,23,39,2,16,23,1,0,5,1,2,15,14,5,6,2,11,0,8,6,2,2,2,14,
        20,4,15,3,4,11,10,10,2,5,2,1,30,2,1,0,0,22,5,5,0,3,1,5,4,1,0,0,2,2,21,1,5,1,2,16,2,1,3,4,0,8,4,0,0,5,14,11,2,16,1,13,1,7,0,22,15,3,1,22,7,14,
        22,19,11,24,18,46,10,20,64,45,3,2,0,4,5,0,1,4,25,1,0,0,2,10,0,0,0,1,0,1,2,0,0,9,1,2,0,0,0,2,5,2,1,1,5,5,8,1,1,1,5,1,4,9,1,3,0,1,0,1,1,2,0,0,
        2,0,1,8,22,8,1,0,0,0,0,4,2,1,0,9,8,5,0,9,1,30,24,2,6,4,39,0,14,5,16,6,26,179,0,2,1,1,0,0,0,5,2,9,6,0,2,5,16,7,5,1,1,0,2,4,4,7,15,13,14,0,0,
        3,0,1,0,0,0,2,1,6,4,5,1,4,9,0,3,1,8,0,0,10,5,0,43,0,2,6,8,4,0,2,0,0,9,6,0,9,3,1,6,20,14,6,1,4,0,7,2,3,0,2,0,5,0,3,1,0,3,9,7,0,3,4,0,4,9,1,6,0,
        9,0,0,2,3,10,9,28,3,6,2,4,1,2,32,4,1,18,2,0,3,1,5,30,10,0,2,2,2,0,7,9,8,11,10,11,7,2,13,7,5,10,0,3,40,2,0,1,6,12,0,4,5,1,5,11,11,21,4,8,3,7,
        8,8,33,5,23,0,0,19,8,8,2,3,0,6,1,1,1,5,1,27,4,2,5,0,3,5,6,3,1,0,3,1,12,5,3,3,2,0,7,7,2,1,0,4,0,1,1,2,0,10,10,6,2,5,9,7,5,15,15,21,6,11,5,20,
        4,3,5,5,2,5,0,2,1,0,1,7,28,0,9,0,5,12,5,5,18,30,0,12,3,3,21,16,25,32,9,3,14,11,24,5,66,9,1,2,0,5,9,1,5,1,8,0,8,3,3,0,1,15,1,4,8,1,2,7,0,7,2,
        8,3,7,5,3,7,10,2,1,0,0,2,25,0,6,4,0,10,0,4,2,4,1,12,5,38,4,0,4,1,10,5,9,4,0,14,4,2,5,18,20,21,1,3,0,5,0,7,0,3,7,1,3,1,1,8,1,0,0,0,3,2,5,2,11,
        6,0,13,1,3,9,1,12,0,16,6,2,1,0,2,1,12,6,13,11,2,0,28,1,7,8,14,13,8,13,0,2,0,5,4,8,10,2,37,42,19,6,6,7,4,14,11,18,14,80,7,6,0,4,72,12,36,27,
        7,7,0,14,17,19,164,27,0,5,10,7,3,13,6,14,0,2,2,5,3,0,6,13,0,0,10,29,0,4,0,3,13,0,3,1,6,51,1,5,28,2,0,8,0,20,2,4,0,25,2,10,13,10,0,16,4,0,1,0,
        2,1,7,0,1,8,11,0,0,1,2,7,2,23,11,6,6,4,16,2,2,2,0,22,9,3,3,5,2,0,15,16,21,2,9,20,15,15,5,3,9,1,0,0,1,7,7,5,4,2,2,2,38,24,14,0,0,15,5,6,24,14,
        5,5,11,0,21,12,0,3,8,4,11,1,8,0,11,27,7,2,4,9,21,59,0,1,39,3,60,62,3,0,12,11,0,3,30,11,0,13,88,4,15,5,28,13,1,4,48,17,17,4,28,32,46,0,16,0,
        18,11,1,8,6,38,11,2,6,11,38,2,0,45,3,11,2,7,8,4,30,14,17,2,1,1,65,18,12,16,4,2,45,123,12,56,33,1,4,3,4,7,0,0,0,3,2,0,16,4,2,4,2,0,7,4,5,2,26,
        2,25,6,11,6,1,16,2,6,17,77,15,3,35,0,1,0,5,1,0,38,16,6,3,12,3,3,3,0,9,3,1,3,5,2,9,0,18,0,25,1,3,32,1,72,46,6,2,7,1,3,14,17,0,28,1,40,13,0,20,
        15,40,6,38,24,12,43,1,1,9,0,12,6,0,6,2,4,19,3,7,1,48,0,9,5,0,5,6,9,6,10,15,2,11,19,3,9,2,0,1,10,1,27,8,1,3,6,1,14,0,26,0,27,16,3,4,9,6,2,23,
        9,10,5,25,2,1,6,1,1,48,15,9,15,14,3,4,26,60,29,13,37,21,1,6,4,0,2,11,22,23,16,16,2,2,1,3,0,5,1,6,4,0,0,4,0,0,8,3,0,2,5,0,7,1,7,3,13,2,4,10,
        3,0,2,31,0,18,3,0,12,10,4,1,0,7,5,7,0,5,4,12,2,22,10,4,2,15,2,8,9,0,23,2,197,51,3,1,1,4,13,4,3,21,4,19,3,10,5,40,0,4,1,1,10,4,1,27,34,7,21,
        2,17,2,9,6,4,2,3,0,4,2,7,8,2,5,1,15,21,3,4,4,2,2,17,22,1,5,22,4,26,7,0,32,1,11,42,15,4,1,2,5,0,19,3,1,8,6,0,10,1,9,2,13,30,8,2,24,17,19,1,4,
        4,25,13,0,10,16,11,39,18,8,5,30,82,1,6,8,18,77,11,13,20,75,11,112,78,33,3,0,0,60,17,84,9,1,1,12,30,10,49,5,32,158,178,5,5,6,3,3,1,3,1,4,7,6,
        19,31,21,0,2,9,5,6,27,4,9,8,1,76,18,12,1,4,0,3,3,6,3,12,2,8,30,16,2,25,1,5,5,4,3,0,6,10,2,3,1,0,5,1,19,3,0,8,1,5,2,6,0,0,0,19,1,2,0,5,1,2,5,
        1,3,7,0,4,12,7,3,10,22,0,9,5,1,0,2,20,1,1,3,23,30,3,9,9,1,4,191,14,3,15,6,8,50,0,1,0,0,4,0,0,1,0,2,4,2,0,2,3,0,2,0,2,2,8,7,0,1,1,1,3,3,17,11,
        91,1,9,3,2,13,4,24,15,41,3,13,3,1,20,4,125,29,30,1,0,4,12,2,21,4,5,5,19,11,0,13,11,86,2,18,0,7,1,8,8,2,2,22,1,2,6,5,2,0,1,2,8,0,2,0,5,2,1,0,
        2,10,2,0,5,9,2,1,2,0,1,0,4,0,0,10,2,5,3,0,6,1,0,1,4,4,33,3,13,17,3,18,6,4,7,1,5,78,0,4,1,13,7,1,8,1,0,35,27,15,3,0,0,0,1,11,5,41,38,15,22,6,
        14,14,2,1,11,6,20,63,5,8,27,7,11,2,2,40,58,23,50,54,56,293,8,8,1,5,1,14,0,1,12,37,89,8,8,8,2,10,6,0,0,0,4,5,2,1,0,1,1,2,7,0,3,3,0,4,6,0,3,2,
        19,3,8,0,0,0,4,4,16,0,4,1,5,1,3,0,3,4,6,2,17,10,10,31,6,4,3,6,10,126,7,3,2,2,0,9,0,0,5,20,13,0,15,0,6,0,2,5,8,64,50,3,2,12,2,9,0,0,11,8,20,
        109,2,18,23,0,0,9,61,3,0,28,41,77,27,19,17,81,5,2,14,5,83,57,252,14,154,263,14,20,8,13,6,57,39,38,
    };
    static ImWchar base_ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x3000, 0x30FF, // Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
    };
    static bool full_ranges_unpacked = false;
    static ImWchar full_ranges[IM_ARRAYSIZE(base_ranges) + IM_ARRAYSIZE(offsets_from_0x4E00)*2 + 1];
    if (!full_ranges_unpacked)
    {
        // Unpack
        int codepoint = 0x4e00;
        memcpy(full_ranges, base_ranges, sizeof(base_ranges));
        ImWchar* dst = full_ranges + IM_ARRAYSIZE(base_ranges);;
        for (int n = 0; n < IM_ARRAYSIZE(offsets_from_0x4E00); n++, dst += 2)
            dst[0] = dst[1] = (ImWchar)(codepoint += (offsets_from_0x4E00[n] + 1));
        dst[0] = 0;
        full_ranges_unpacked = true;
    }
    return &full_ranges[0];
}

const ImWchar*  ImFontAtlas::GetGlyphRangesCyrillic()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

//-----------------------------------------------------------------------------
// ImFont
//-----------------------------------------------------------------------------

ImFont::ImFont()
{
    Scale = 1.0f;
    FallbackChar = (ImWchar)'?';
    Clear();
}

ImFont::~ImFont()
{
    // Invalidate active font so that the user gets a clear crash instead of a dangling pointer.
    // If you want to delete fonts you need to do it between Render() and NewFrame().
    // FIXME-CLEANUP
    /*
    ImGuiState& g = *GImGui;
    if (g.Font == this)
        g.Font = NULL;
    */
    Clear();
}

void    ImFont::Clear()
{
    FontSize = 0.0f;
    DisplayOffset = ImVec2(0.0f, 1.0f);
    ConfigData = NULL;
    ConfigDataCount = 0;
    Ascent = Descent = 0.0f;
    ContainerAtlas = NULL;
    Glyphs.clear();
    FallbackGlyph = NULL;
    FallbackXAdvance = 0.0f;
    IndexXAdvance.clear();
    IndexLookup.clear();
}

void ImFont::BuildLookupTable()
{
    int max_codepoint = 0;
    for (int i = 0; i != Glyphs.Size; i++)
        max_codepoint = ImMax(max_codepoint, (int)Glyphs[i].Codepoint);

    IndexXAdvance.clear();
    IndexXAdvance.resize(max_codepoint + 1);
    IndexLookup.clear();
    IndexLookup.resize(max_codepoint + 1);
    for (int i = 0; i < max_codepoint + 1; i++)
    {
        IndexXAdvance[i] = -1.0f;
        IndexLookup[i] = -1;
    }
    for (int i = 0; i < Glyphs.Size; i++)
    {
        int codepoint = (int)Glyphs[i].Codepoint;
        IndexXAdvance[codepoint] = Glyphs[i].XAdvance;
        IndexLookup[codepoint] = i;
    }

    // Create a glyph to handle TAB
    // FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
    if (FindGlyph((unsigned short)' '))
    {
        if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
            Glyphs.resize(Glyphs.Size + 1);
        ImFont::Glyph& tab_glyph = Glyphs.back();
        tab_glyph = *FindGlyph((unsigned short)' ');
        tab_glyph.Codepoint = '\t';
        tab_glyph.XAdvance *= 4;
        IndexXAdvance[(int)tab_glyph.Codepoint] = (float)tab_glyph.XAdvance;
        IndexLookup[(int)tab_glyph.Codepoint] = (int)(Glyphs.Size-1);
    }

    FallbackGlyph = NULL;
    FallbackGlyph = FindGlyph(FallbackChar);
    FallbackXAdvance = FallbackGlyph ? FallbackGlyph->XAdvance : 0.0f;
    for (int i = 0; i < max_codepoint + 1; i++)
        if (IndexXAdvance[i] < 0.0f)
            IndexXAdvance[i] = FallbackXAdvance;
}

void ImFont::SetFallbackChar(ImWchar c)
{
    FallbackChar = c;
    BuildLookupTable();
}

const ImFont::Glyph* ImFont::FindGlyph(unsigned short c) const
{
    if (c < IndexLookup.Size)
    {
        const int i = IndexLookup[c];
        if (i != -1)
            return &Glyphs[i];
    }
    return FallbackGlyph;
}

const char* ImFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const
{
    // Simple word-wrapping for English, not full-featured. Please submit failing cases!
    // FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);
        if (c == 0)
            break;

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < IndexXAdvance.Size) ? IndexXAdvance[(int)c] * scale : FallbackXAdvance;
        if (ImCharIsSpace(c))
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width >= wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    return s;
}

ImVec2 ImFont::CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end, const char** remaining) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

    const float line_height = size;
    const float scale = size / FontSize;

    ImVec2 text_size = ImVec2(0,0);
    float line_width = 0.0f;

    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    const char* s = text_begin;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                if (text_size.x < line_width)
                    text_size.x = line_width;
                text_size.y += line_height;
                line_width = 0.0f;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        const char* prev_s = s;
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0)
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                text_size.x = ImMax(text_size.x, line_width);
                text_size.y += line_height;
                line_width = 0.0f;
                continue;
            }
            if (c == '\r')
                continue;
        }

        const float char_width = ((int)c < IndexXAdvance.Size ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
        if (line_width + char_width >= max_width)
        {
            s = prev_s;
            break;
        }

        line_width += char_width;
    }

    if (text_size.x < line_width)
        text_size.x = line_width;

    if (line_width > 0 || text_size.y == 0.0f)
        text_size.y += line_height;

    if (remaining)
        *remaining = s;

    return text_size;
}

void ImFont::RenderText(float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, ImDrawList* draw_list, float wrap_width, bool cpu_fine_clip) const
{
    if (!text_end)
        text_end = text_begin + strlen(text_begin);

    // Align to be pixel perfect
    pos.x = (float)(int)pos.x + DisplayOffset.x;
    pos.y = (float)(int)pos.y + DisplayOffset.y;
    float x = pos.x;
    float y = pos.y;
    if (y > clip_rect.w)
        return;

    const float scale = size / FontSize;
    const float line_height = FontSize * scale;
    const bool word_wrap_enabled = (wrap_width > 0.0f);
    const char* word_wrap_eol = NULL;

    ImDrawVert* vtx_write = draw_list->_VtxWritePtr;
    ImDrawIdx* idx_write = draw_list->_IdxWritePtr;
    unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

    const char* s = text_begin;
    if (!word_wrap_enabled && y + line_height < clip_rect.y)
        while (s < text_end && *s != '\n')  // Fast-forward to next line
            s++;
    while (s < text_end)
    {
        if (word_wrap_enabled)
        {
            // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
            if (!word_wrap_eol)
            {
                word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
                if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
                    word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
            }

            if (s >= word_wrap_eol)
            {
                x = pos.x;
                y += line_height;
                word_wrap_eol = NULL;

                // Wrapping skips upcoming blanks
                while (s < text_end)
                {
                    const char c = *s;
                    if (ImCharIsSpace(c)) { s++; } else if (c == '\n') { s++; break; } else { break; }
                }
                continue;
            }
        }

        // Decode and advance source
        unsigned int c = (unsigned int)*s;
        if (c < 0x80)
        {
            s += 1;
        }
        else
        {
            s += ImTextCharFromUtf8(&c, s, text_end);
            if (c == 0)
                break;
        }

        if (c < 32)
        {
            if (c == '\n')
            {
                x = pos.x;
                y += line_height;

                if (y > clip_rect.w)
                    break;
                if (!word_wrap_enabled && y + line_height < clip_rect.y)
                    while (s < text_end && *s != '\n')  // Fast-forward to next line
                        s++;
                continue;
            }
            if (c == '\r')
                continue;
        }

        float char_width = 0.0f;
        if (const Glyph* glyph = FindGlyph((unsigned short)c))
        {
            char_width = glyph->XAdvance * scale;

            // Arbitrarily assume that both space and tabs are empty glyphs as an optimization
            if (c != ' ' && c != '\t')
            {
                // We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
                float y1 = (float)(y + glyph->Y0 * scale);
                float y2 = (float)(y + glyph->Y1 * scale);

                float x1 = (float)(x + glyph->X0 * scale);
                float x2 = (float)(x + glyph->X1 * scale);
                if (x1 <= clip_rect.z && x2 >= clip_rect.x)
                {
                    // Render a character
                    float u1 = glyph->U0;
                    float v1 = glyph->V0;
                    float u2 = glyph->U1;
                    float v2 = glyph->V1;

                    // CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
                    if (cpu_fine_clip)
                    {
                        if (x1 < clip_rect.x)
                        {
                            u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
                            x1 = clip_rect.x;
                        }
                        if (y1 < clip_rect.y)
                        {
                            v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
                            y1 = clip_rect.y;
                        }
                        if (x2 > clip_rect.z)
                        {
                            u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
                            x2 = clip_rect.z;
                        }
                        if (y2 > clip_rect.w)
                        {
                            v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
                            y2 = clip_rect.w;
                        }
                        if (y1 >= y2)
                        {
                            x += char_width;
                            continue;
                        }
                    }

                    // We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
                    // Inlined here:
                    {
                        idx_write[0] = (ImDrawIdx)(vtx_current_idx); idx_write[1] = (ImDrawIdx)(vtx_current_idx+1); idx_write[2] = (ImDrawIdx)(vtx_current_idx+2);
                        idx_write[3] = (ImDrawIdx)(vtx_current_idx); idx_write[4] = (ImDrawIdx)(vtx_current_idx+2); idx_write[5] = (ImDrawIdx)(vtx_current_idx+3);
                        vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
                        vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
                        vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
                        vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
                        vtx_write += 4;
                        vtx_current_idx += 4;
                        idx_write += 6;
                    }
                }
            }
        }

        x += char_width;
    }

    draw_list->_VtxWritePtr = vtx_write;
    draw_list->_VtxCurrentIdx = vtx_current_idx;
    draw_list->_IdxWritePtr = idx_write;
}

//-----------------------------------------------------------------------------
// DEFAULT FONT DATA
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array.
// Use the program in extra_fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------

static unsigned int stb_decompress_length(unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier, *stb__barrier2, *stb__barrier3, *stb__barrier4;
static unsigned char *stb__dout;
static void stb__match(unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    IM_ASSERT (stb__dout + length <= stb__barrier);
    if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if (data < stb__barrier4) { stb__dout = stb__barrier+1; return; }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(unsigned char *data, unsigned int length)
{
    IM_ASSERT (stb__dout + length <= stb__barrier);
    if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
    if (data < stb__barrier2) { stb__dout = stb__barrier+1; return; }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static unsigned char *stb_decompress_token(unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
        else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
        else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length)
{
    unsigned int olen;
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb__barrier2 = i;
    stb__barrier3 = i+length;
    stb__barrier = output + olen;
    stb__barrier4 = output;
    i += 16;

    stb__dout = output;
    for (;;) {
        unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                IM_ASSERT(stb__dout == output + olen);
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                    return 0;
                return olen;
            } else {
                IM_ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        IM_ASSERT(stb__dout <= output + olen);
        if (stb__dout > output + olen)
            return 0;
    }
}

// File: 'Karla.ttf' (16848 bytes)
// Exported using binary_to_compressed_c.cpp
static const char Karla_compressed_data_base85[ 15900 + 1 ] =
"7])#######5Z9sf'/###I),##bw9hLQXH##j$1S:'>pgLZvaAd668X%QUQl;*31Qg3R9s/N@4',:a`T9n&)Seo$,p8ZW%q/^6se=K9C[HP>JeE';G##Uu/>8i1i86<CDmLg@AS@'Tx-3"
"`Nc4qh2TlS^^ppAqV,-GZ6%GT0MXVRk3r]l%J%/GBgO2(%qY>-B)PY%,5LsC_Ta.SPQ)mfLbFe<b';9Cnfx2aceA&,r1dD4AJr9.s+`G2)a4&>V3dW.;h^`I^bcMU375esm]u.Uo6Q<B"
"UlLpWTxI/L1#gd<0iR/GTG/58ID^VIv,i?-qmnUCABcDr1V5lLbQWfq@C=GHX1(%7s;]M)l%1t%1)Q-GN6ArMBMx92MA>>#aEl-$8XW>-jge<6'..w#oZ@U.&MBeJM_Nj0V'2+8-0KP&"
"mHe]F4h2W%WgJe$_^.R38^5/v*>uu#?PO;.('I0#RGYn*w*]`X/n+aXhdWV$6IL^#$,>>#qbjV74iWh#I#g=uTB2L,dA1G`Bf%=(],LF%']T:#(K.+M9+`P/v/HP/nx&58U76:)SW=gG"
",Qj)#S^[*N_dq2#??`IMRmq@k(NG_&j@ZY#-D-F%31>F%1fN$KuK0A=sbtx+#_t%48S'299'QV6j]HV?r][]+';>J:LX9(,NJrx4FHcf(jWZY,R>05/a:2F%I;8>5U=<D3KlZc2T3HS7"
"S=W`3oF=A+Feco7Qg6R*L.c`*oKT]4WH)58C-ux4d82>5i8kl/]U;D<[5K026g[Y,l3HP8pj0R3#W8JCiw<D3unt.C2;l-$65[w'ZM`w'J::JCmAk-$9+WS%fB/F%,6<JCjoNk+XXYiB"
"M6i--LG-F%Yp0F%>gv9)WW&29v=$s$a8hu>k`df(_wNM',U&,2TB45&k-B-dB&xE7+$GJ14aD+rO%fc)<H9a40SnQWo-@M9(97JCZEt(3'wX>6%`kA#JA'k.w3X]#^J8r/Xd@H#`:ol#"
"4'Ff-EvY9V$cm-$oO*:2KJPJ:/mB5KHGUR'.rDSIP=45At?qCscA/Jh,Cq'##1j:mZJtCjbhfo@jNlEeD$ZEe0bp%4oxDG)*r]`*eY6H*)x=A+apxc3,^2.$Uu._Jfxa-6%ej-$gjd`*"
"1[3F%Ob_rZ8sk;-cDUxO5FUxOqx1'#IN5/$P].R3X2cD4'iu8.[gkl&-k@T&.+%]k+L;s%m=9^#&8D#Q^w3CMfhjfL6Ng-#V]0B#3Tu%2Tce@#7.N`#1rm(#KA<jLbd8[#qW.H#^j(c#"
"Pb+w$$%HS7tbtx+n7]`*siMW/F0:*#f####'S5s-nlRfLP:$##27FGMm8#&#kAt$M->`'#R-^g1XqH0#hQD4#+d46#:IQ&MTeM:#MsM<#nu4Y#kT_n#>'^R-;<o=-gP8wu#&###'oOk1"
"W1@8#$C0:#BHd;#B(t$OPt7=-KcEX%0.Z7vFUh3v:Q42vlQ`0ve?D0vYqc/vR_G/vZ)kTu7pC6l?avjkIZqUMAk-gXNq%s$YCN:/&)###^$v=PrwSfLWZaH$=R<T%ME#n&^8`0(n+FI)"
"(u,c*8hi%,HZO>-XM6W.i@sp/#4Y313'@L2Cp&f3Scc(5dUIA6tH0Z7.<ms8>/S6:Nx9O;_kvh<o^]+>)QCD?9D*^@I7gvACH4.##K0DImSJZK#O'##/l-UM?_jnNOQG1P`A%/QZZlHQ"
"+xm-$.%F2CR-(mSeHQJCW3'##SE(##`?_c.:,+rC0MV.GBObcH5SmAJ[[gDSWCc2_Z+Gb[NCf=(K,'##l8q'#,pr4#_9#F#=gkI#HKDO#Ub<T#F#aW#@<X]#as[d#iV4m#^$iv#N)x)$"
"#>C3$ID]T$C-X=$v=xL$1]4q$$PA]$5g9b$cJ=i$Y0Bm$fCC7%puVw$Efm(%I0K2%@0t9%L[:?%bX8E%ko0J%`0TM%JS4Q%2U3T%%)$_%iI-g%^tQ4&Q@&&&w$*-&3jIL&f+89&C4s@&"
"Ab8I&8a5R&T6RW&h'>^&11Nd&[C$0'6[hr&8dw%'?$p*'ng8M''fV<'a16C'I*<h'`i7P'u4mV'/$,b'8G`k'x1tx'#P0F($*n4(aHJA(m]jP(3T:_(LRbi(nWqr(mbVw(d`(,)sX%5)"
"9Ge@)oE6K)^pQS)Ym$[)Uti%*hCke)V5,j)*>gq);HLv)Y)YB*cf(0*R$J9*E5u^*M/5##nNJ=#c6:/#1f1$#mUE%$bwYd3aIcI)>?ex%V''Z-IsC^#7tVP_mfxiLsqJfLMR6##eY3T#"
"hhLxLUF5gLNIr8.X>l-$R&A<6YBs-$#A3rjQLO_/V1qu,n@)&=^7pu,l>,<.Rk&>.kJ(.$_`%E3WG5s-,O5gLIYqlLi/LB#==0h:SJJ/C[VS/C&$cNTGSK-D`nq-D>L.h:D#o3TqM,P4"
"nH4uL0sG:]YIFI]bA'XL4wl@X$9?VdS159/PU$##x^3t_9IE:./[v)4buA(4mCTBfR*i+4BYB+*U(NT/UXxU/w;xF4Uu/+*2]wC#uxUbGKdc#Jpxs`=*1C'72D**<s7&b@k$bJs&gC_,"
"(F&f;fVN&=<hb*?DBq/'fJ`e?teB@%p@Du&sr5F,OaDs6Vm,P:-CYvcl#f[$V1B(sC4+/L??(,)[=PV-01ol(ig'u$*W8f3c)qv-iL0+*;UKu.&X'B#O4ji-6BqTVJL,7*fH6R*E*sMq"
"q&kU:BoC^5T0PD39Wdbu$w@YY&_8I4xK&I4Pp+B,=Z/a##bvs8AQQW8*6e31++pQ0b@CxOcP_-O9=]U;))B[5)S$##*QCV?,YXJCt=PS.0AR]4*`$s$/0ihL1R/J3,@fBJl%u/MaYfF4"
"NECtUjsw<%V61A$0S<+3BgJf35`)K)9i<X(rPH`#B.4q0Y_]U/c#jP8I3MW/6b2qD&C>W'Do@>DWNT'9G,xS:/R%o_;h-h44#(JDC8*5A8IEN(#.f3:wtd,*q/d;;(^[g<-Qt@I(oxO="
"HXFrAm)4LdDC<Y7dfwI2D8B*[M),##_8I8#fP(xLrU),)7Rs)Fx,kZ#Z0%(Msk-##R9^M#FA':#>qbf(x?n5/hRQ65he_P5[$vWB^##YufKaZ?lub3@M),##c&l?-o4&R0s0`a4jj'u$"
"Lh3>N6l5I#F[/w?krOn?NdTw0F$>D#gKOe#3$M$#?9`a4Fb^=%:d*E**$4Q/t+F+E5/p)E6/^81H7K_#guxF4;+&G44tAKDrqh-6190I$2u/Poc>,F%bT/i)_kY)49,B+4F)_JfgwCT#"
"Qdk5h>GJ6aQP[^,j,j4]39bw0b-v>5j%>c4A=MA53#Y$66HWA+4JMq8g$]],W2m92tOH>#K<.29@=%_SP0fX-Y+`ZuI0't8B,Zl09NJ=#vB+.#@?$)*Ek/Q/-rgG3<@MW->[Jm/^(E%v"
"cVah#Nu=d4(bM8.P5r*uM;b9uW7YY#LiA(ssk&/Lk1?m'w=:E5,Zc8/>6S5/vu/+*)Pn.UArg%NjN.%>HEQh#(t60Y7MtRX+MKIeNnvEMu%iH-023Y..O_/#QF/e%950J3o)ct$&f,03"
"or9+4GMCN0q]7&+G5>>#d9%##f/a9Dcx###tYW+0OJ,G4RD^+4Koh_%kIuD4aIcI)=C[s$$vTM#(b/*><-m#B.1v$%e(sPaZ0ppKkS*R:5G$P(5^i#?J#=.3%`r<T5_jL)1We9Mi`)kb"
"2IpE@HY`$'wPe)*IQ4W-[HG3bs77<.6gE.3RxU_%l=Z[FLC1Bfe`98@EfDe=9cL*>0[3:'W#cu^0ZNEC<Uf_6SHD=BNZcpB;IarAgs.c==iWHmR/5##?5YY#hwH0#h_kgLT7Rq7n*/^-"
":[TD3Od,8FCPeF`CqE2oVD.PA+Wbg6$2r:?PxY$0?Y8&vp1Yu#[`R%#J_a=.1q3-vaK%J37oxv-qVkL)tspTVSQZuY/%:A#+a%e(eJY'I0,NQ<FqaV7bd+K#]=5-&jTRvBBGs:%P9^C/"
"XYd$vJ+ffLJa8%#G2o#/1m@d)'Fx;8;>Uv-6:Be?hb@s$k@Vo%>U^:/>:;n]J>lJCaT8#B$YT(J>9pYu#$;bG<GCfh%sW34fp;fFGZSQ#Q'@Q;iF.]8r09;8)DQ-Zm,Ro#IHY##:XR@#"
"[W5*nL[W=lg6q^u?,eV:w%C20NK#D/^r2%v&S>LMHO>&#a$(,)%F&T.Ynn8%a[eF%s+-J*cc>D/w7WT/<3rhLPW$lLIlx^dpp$^MWpk39j&frP7L$N;U%*Q24Wm60I@MI=9Yqr6gPC[@"
"Y,bF=?TFl9p$CpCfbE4)^IEE63EWn'NH:3(1>UVAD7[Y#lF]9M[aZ9M1rho.f3].3&V'f)hCh8.)NsI3d/ob4?_#V/BSc)%x_>?IK&xf3#;gX@D0P:28t=j=G0?F-D2#3BBO,#TGr(FG"
"sg^<1()5rrEd(J>Bh6@52EB)=;8v(?SFSa5U%^HFsh#_#sFAD*8uLf_n_d;-rSl)%(<PF%:x)Z-#%qG39pcG*jY,W-8rHF%M5YY#EY,SRY`e-6dE(,)He39.j%>c4x(4)FC;(<-@Xos8"
"_gkA#@Qk0&x+;'#<#mj#fGY##.r-W$TwJu.G+V=lxbR_u'O&a#R6=m6g[7)M^tH+#4wYN#;nHi$L<fX-bXaJ1t]4i^$o1i^&VNgaY$3wgv$x+2_jjiUCg(2)j6wP/&AgH#V:;Y3$x2ul"
":lfJ9:Shff^5R_,5]%>cj2%&+F7$##[3)KavnXI)dI+gL#1ue)C.@x6-l%$$P#MT%1n#W(YS`hbrV2,+[vp*<w93<7.tKv-BTX>-Y?Hp:Whwg3WJO**K@JU/e?=B6$7K_unr6@-ml+jL"
"vtqh.`[(($a)c(.B+vlL0Ua=.Yj,N0dYEF3NP,G45>PjLlT]s$<W/i)d+39/x/X,3UGK-QhEj;-en7T%*<gF4_R(f)0[.&6?:Ai3c5SK)6,T-WbC>:2A3d^hp'?<8-aemPB75h>5ONv#"
"9hoj:ILpD64;=&>>P2`553*I3J=iVAGF%22mdsa_M_>uRrj](7#>l^8%Lcv-A.DRBL_E=:Tt:g%rr###mlPVH4=35&jc;F3N`?<.cP/J3MbXsg2?r[4OFP^D$fBxODM)V#]>uu#3T&##"
"UXn92o)[`*MUWq22Y2K)3hD784>g;.I)2hl9P]L(XRq^oF@vMCoA:+6S+YEGc[jMo0$?<8.p889?7+k#u*kH43q5B=YV4n'XQIL2HxOD*NN$)#WH9D3QTPA4gm()*d`G-3(@n5/?;Rv$"
"wR`3=lcsI30Zc8/T[587q#W@>kF=J<9XR5;CLC_o#'+/L&@[w5hAFsP6WTEH[6hBYrBU&#.<^;-Sl'S-*')x.*Ep;.F(bT^N(r;.&4_.ZgNxW<Fd.?Kn=SVH[(Z7Z5-Cpfep/1Oqx8;-"
"^U<^-#((F7hMi^oX)XbNgt#I#tQ3T#$i,V#+(*h#IIqg8.Tbg$WT&##e5u92ZxV:2=gtMCtThLg$:J7nB<_<.sSkUmo1#Fn]_biK<o###o4+R-PdD^N%3i*4o;oO(]&Bg0/k:`KQjQK;"
"Jd/TDfPhoU3RMCIZ^VW[ceX(=+#H%Bi$6a3pAck=H(5^#_bS/.>0;hLi4@m/cHPQ#K>Jbupu]?0$[pc)ux*#,Y6e'/siIfL(9Fl]'rB#$Y)w<1&urnLEh?xOv(GW%_xol8Wt6Z.RT)*4"
",5^+4$-0@#OmUS<alsH<Im[E=u9GD#PEMmLDLT_-&*5:2uIJL(YwH>#fjHd)dNYQ#AuaLpM8D6r)DRVH$ZA8.X`.]Xlj6&mGnP&#U(I]-)UJ_&TvvdF@(+F3E^R[uA$E99D[1^#l6`T."
"JmV]+%*6x1^p^#$?.)hh4_-P<:2l'MW_Qu>XZDxOtw_+;*Jobrc@Y-?UN&##:o;dOxwSfLwe(B#n&X)bYpNC?Hbc3OGaN%tQ9Z&#,JDU2Kd^5/O.[0#.F278GKgv->;gF4i3F,%St]:/"
"$$4^SFn^&SHFvoCL::5D`kqjX18tnXPxO%:LGT<L@x:hG>orEIB(&FI_V###Sj'u$LHF:.%<b,36>PjLT%v4S8C^wJnpCW8&<pg#r7o>D8=Lfu6i<>9D;1@-O^we-4[%RN-&pu,.xHs$"
"b+WT/l*5[$=.e8.Oe4D#v/A`#wQiB>GihDRF$6^S%NUA,V:f_ulA2U3h*nY8_==;6A2.XM/*/U/cY18I33-NCP_ri0$K=p&i79'#g<3:.26;hL6+B.*dWcf%bf@`=3osNW'Q4@E]ctQ:"
";D;M#Y?PZuv(,YuV3$sCJSI]8'Tq)0jq]&v5$4]#NS/N-q43j$4fK+*=+pv-c,kL)9'04M`xnf3de_F*a*OtHUBM02a-#uJt&jc;0a5l2bsnh<'U'`%?.Ig4*9vK1*U)#Cmd_;8I&94("
":d$eOUSIkM``Z##Aau)MXjd##m66g)P)&=.7872fciVVH7jjp#l6fY-3$7kOtf'F.NK&F.%8hd4eR0N(,['u$]p*P(^m]6:)6cQjq;Lfu1r61<d<TiuNE-aIZ0&vLUG-##75YY#9Woc4"
"DmV]+)bM8.KMvD^28YIk3C9.qk6?xOlEP_/UM###m5v>RsO+.)+K7X1@ng#UP.U2?n`shPI[5R3g1Xrdf8cP&AW&8RR_9.6XAJwBTcWW/>G_;.2cg/)slEb7Jp'&=FUa(BKbXv]T@7tt"
"Y.<D#a/YY#=m$8#.+35&@#j]+VswV%&/vN(nkLGDf^(dlI=cHdOKaV-Xr0uuE?fuG?j.N:<TQD*c+?u%[x2D#_H7g)`0eX--QFeuaEH&Mh'Heu,[Oul`MVaue*J3M<l,*vA=6_#[Oc##"
")N/i)2-k-$F4-MI?D.DjN_GU9A,KwK,&U._*S4XS@.=F3+h#I<R)n<SPL?>#[vFiTKVAF7bqY)4B-k-$jJ/F7Kf/b#2u`-6Va.cr#<of:;SK-Q:]TU/-[7j6ou&dHo#)cuh.A=#5k4c4"
"I`v+VDH&v#[-wX-=:O`#Z[2359I<B/9lrO#*NU5M?@CF*w>&Vo:ZdS@$SPrL8W@M9K#'Dj'kc`*D1$##K.rv-@b>d3KjE.3-[w9.nLMs-Z3%.M76/M)k_+D#8p+C7?3;Q0-5lZ%nCrw@"
"^t1q9q`Ee5DdLu7u6gEFQEt]@ESY@@]D%I+dc.60-,g9Mff4JC13?)*1B52':i###9x^32*rE<%G-3l(2b;78:q;Q/;]RHJwa`h;'q(J<e5HD<8dxF#$NELlcti'>uR5xt>jD=82Ng_B"
"='J,3KMmre=x2g)-9TaldPSF4CM%+<6q;Q/FiLt'3;A>.tHr'HpRne>(PiK#XI4?%-un/X;Ohh;$=6ANbG.;6Oi_oofn#B#5thTr;lC,3C<1T%D59f37Lb9<&'_I;Kdc&Je9#i<36-)<"
"$?qf(KQrR1H<HxN)T,%C?m`X8r*94M5/F'MFMa$#argo.tS5)%@LG<q2k;W-1007/vBo8%@w=5&FE0Y&_;Q^I.bGt'JawWJ5$1p9a#N%?)]AImX=Wr:Oi.x&F(0qL,U-##U,>>#2Vo5#"
"DxS,MZrte)XxAk$8,B+4TxwJ*P)W/)5r/tB_[?1<o@dfCqnR3:]?iG#=XIq98[rB8Y5>##KuhZ#08<)#*^Q(#CkP]44Y6MMEJ^I*u9_=%x%'f).5lk/lm?d)%aE.3[hIW-Kb.@@?Pq;."
"U7-5/BN_V%V_D.3>OH22/o?xS6G)b5T,X8.KZ/51k)ao_9EwDoKPB`=,vEv-Od+D#3LG(7c=Rx6UR<&-.ux-<+Tf)4'SYq0'2sN*=FT(4N:LbGt,kE4(x0P:9B1^#7P6+8fXsU8NKGj0"
"56YY#+%&5#G$(,)t(6W-pxe9MfFIJ3xTvI>7=n;JcMfF#5er=B$t1&+rF&C@1c?=..&w4A0.n]cTp@3td'?e6Yax9.<50(miVOO<c=M`+93GA#%T9^#+$Gb+Vtp<1L2Puu-e1$#)1<u."
"CXI%#<im92a5E.3wkh8.I>+P(-eB:%@f)T/j9.l+6Xi@,01XH?afmu-BOQ87xxACubcF8.StqI-h,2w.$),##56YY#vG[uL[o,>%xYS$9x<_=%9;Jn7nhdYFGp-Hux23>Ybeae]P@f<L"
"M$Q&#'U%4:D^OAYk(4O+O),##5?uu#bUv;#]M.Q->^BK-E%Ok'n.rv-Lp5e5`>u(72h`9Jhc*x7qg)x770HD<S9&UDKt$&84pK3C)u1&+iY6*?um+%NRu.s$Tdi2D)hp_-QC[@9X$&.6"
"(Ct(3a6c.qhuTT&6]###3d(99OKv)4FJ`20a&SF4k`pZJ3)n-$)I,eO6C+V;a=R'JoP3bI&TUCBH5A<B5f4JCP87)*vn.g)6-O-3vbLs-:o0N(.d1@en?vg),/]>2>n1<_UiC#e3s#s:"
"^RZoRpEm=OE`,r#TfYvA`6V$7[?`*#HX.H3[38/1&+a/)UOU-Q)-/J3uUB+*8v19/aqMs-l.ad%vu/+*KD,G4?U^:/lBRq1IWa@,%fHE5YgC?K)#DG5P4W?#5Q(#@st%$3BM4W6g,EtS"
"rK2YO)KK(BTbH;@4LLfLVfp.C7k>gDoR72'F:Kv-o1bHZpv%J3vW9L4mj@G$A#p;@'D#Z,]fA,op#h8C[5RgLU]/;6Atl1g^ra?Tnd`.3fiCm91,BFcMaHX/G0qL;C2(e#f[En2c`>qA"
"vtr3a4`cb#=c't/,8_K#8G8e4<oW=.Iblw5w(KY8<21[6>Q,s.,*,##-FvV#=*S-#^rgo.NlOjLt<x9.IvsI3<;nQW@(FF3B+GPA_A1i)mH-1<e(-w.IeVH*M;l:.b^jiMk>v_jDt7_8"
"E=$`sTP>]kPv@_8`]d8/X$nO(^%*<-N,tI3X??YYx-9rBv0i,=#JdXAEuCH#MvO0Jrccsuw*LS-4VY8.>e^:#KLXg1`hqf?NZXR#xEqJ#R43jL9@c(N.]I`a:9f?RxDgo%EkefGERE5<"
"=w4lEp74R34P^fC;+=JLNckG<A,<.6vp;b7L4]9VXw2$#9f2T/V9>V/--/J3rE'/BEV$<^F#H:MQL,E^n@`)^(+u_.3lL?#0[qlLl]3r7(I1a4O0AhG78Ls-bBIF49>]L(5Q``QZtJW*"
"V4xcOuarx5%C*'/VN3`:0,vNFQP,lLra;xLCTDs%[x2D#ZH7g)&0fX-rb[[u((1fu$L7[uQ>,5AEFgCW;W)/5pwq+;E<9>>1Zwu,L]&+*xL0+*neP)4T,RI%]M(a4c4)=-BWKo/6V(]6"
"heu-)L>&s6CIql:pp,,2C;g/=Zd?m@7B;@./=Uu2x/8J=tcBiL]S,*vaukm#$RF?.;de;u*9PG-A.PG-O:PG-[i+c-4'$CJ]#G:.Xxk_._;W.3ln7Y-rmK.H+Iw1Mk2)c+(*pF>1Hk81"
"7hTm00/ZkLik=x@5;R-HEh](#)c<T#F5i$#`im;%;jE.3+2pb40<Tv-5jIjUXJ6[-0IVO+^Yd<.L+aW'K.Ux5pm*D5(tgv$wh[^08F.%#8LI[#e':Q9/dDh)N0D.3u3Cx$M3H)4^Mc)4"
"9EsI3;k0T%V0&;QB(_B5nLCm^1sH#>0R(duDF6<.=JwA@&*P?#:ui5:vRhw85b9h;fHDk1C46uBTaCnLfcZY#I6kP&lJb]F^r/eZUt7<-Iws7%p#BA,:<4#-_Kj],<W:#$=)cB,3RW-Z"
"[Ik%XfrNq0f^A8%OXvr-tf&9$cYghM@m;A+J($p.@3w;%Z.JfL*o6F<@/_#$cCAH<PA=mBlQ@_<Fv0B#?7GCG1o[H7C,^6E>a1,N8MdF7FUk?KK*$xK0Ti+M#e6b<K]1a4v7g(8[UQOL"
"ksUsNlJ*e/k.u2UZM&j:)k#^,L(hU/TbCE4dbL+*.G>c4i7Ls-q=;+R)?F)+]3Me3S<3D+xx7T&16f>6P[`m26U=F37N;o&I]eI25KmV%><Zp0gm8NMlX/%#UfJF-Hb2^%7IiAOC-rx7"
"gIe[BO`qDY^_6##3->>#kYi,#8Q,b<<=mg)8+U/)dL]d<dn`8AmZ[$=vG%krJ1$##F(rG3XVlL#Dt3Q/S3.Q1x4E_*o`6h8Nh7OGD7l-6Uu._JHVTGDRHoM(OoxA#g&@e6.VFLjrccrH"
"p=i5/UqKmh#EM/8)fB#$x/mS8v$Ag)i=(A9dJP$Pl3%#8/79I?.gq&-0F5gLToxw#5$CW$Q*nO(hK^:%NQ[L<)?8TT#Q^W$Q`qr$5nOJ(%)_G3$Mv`4g2D_/h0dTrAIpoMW>Nq-Q'E_/"
"O6D9rT(no%d-LhLWK.IM0sX?-_`o_%S,N&#E:C?$:E-)*JOe'&h^_c)Q6)^#CY9hLS#RaPGm?xOKxeM.3SSiB;UY2:mYL'J,v0X:F$nO(H3I*.Yrq:8qtkmB`^SA?6c:;=9x-W-d]6^Z"
"^X6##DTOVH$dN%t`Ph&(YO%I3pd+#>7>65&O8[`*o,U:.lH3Q/mjt9..u]erod=X--1sJs]195;LAb2(2+Wf4LW->%q-MP8En2aJGt)EJM:+F?&:E0kH8Q)M3>BwU[W987lL7T.*]ghM"
"r+TV-XXs-?5TsE[N*jG[lNJ;Q<LI#8V2N:K(PcD8iuHW&pA#OMIdbjL^ho,8^B7PbwgX,O*srjXn%VYdvFm<-TXXR-7:I*.RLSsRZtR_#HjE.3wkh8.G>_;Q3%wI8f%h=qd@HH4RCZY#"
"D2Hp.=u./(036FuQS5J*N5wF#qwf_o1t?`$p])&NLsB#:;5^+4@6;hLK?e8/@v2<%ZfHSL)RQC?cobG#@3MRo1axU9C^t@=>#CWJfPtJ-mEtJ-7A4vLrKYhN(Hf(W@]I&=I)1#?oA(6/"
"sTot#wXVmLY'g%#I<qC-Uu'A(WJ6/MPu?<QSP'%8M'C#$95Dm/o<c`*j*io.L<6'.jK^:%ufLn<;&-semZ[$=w^U^#M7GCGD[1).ev%HWZ3t1)v:&VSI*8G=M^C_*Cgw]/3gb.hL351#"
"+x-;n0ctS/r5fW8`v<^/-Ln03d7gq]TN/a=TnwSgMqCH-cs$*.u+cIWODk%$xmaq7c=rA5nq4&:fJU&T,E)E#^e,*(IG:;$A5l;-/wre.p&U'#;nx_9+w;Q1u3gK8T[m>Usq$Gi@*Gn/"
"MKSX#4tp:#piTK-aGYT+jK^:%RE#'=:GD9J4':p9N#:io?I$l`cXBMBacP29b0nwBvP<A+F3Xs-LaM^>DO)B42m&f)?S>c44C4D#RE<$8nZAq'BF9aIFBc[&8Y/8%/6I_*R5YYAXG(qN"
"),I7:*QY%7D_.*;l2YD4dEQGjVt*Rag4ATa;Z=_AL#`4br]:T.6)n63L=)P-M,jB%4&bo71qu(bS]Nv-QNssDK)Yq7'p&8oaM:<QY&=0(UG###p4(Djqs]<-^<Sn$(cgquhnBI9(XrS&"
"XxH,*G;93iPMZT/s4MRo,dQQG*@Es-1O8(M_<$b#%mH7:^xt?0xv`GM%RCX8;I]'/e0O]#xl5U,X(:3iptWB-G/n>'Ce/A=K(tYdYOho.(b$x-0[1N(MG#G4<)]L(m=]:/>0?L1ow282"
"m4+c#v-m/<jlf#Re:0aD7?]SoicN99$t1&+%eOa8B0n$$LiA(sIp/DjAE(,)AbB:^xu[D*eAmf(_3X9.fu]erg^Kq.t#173b>0dkPGn7:]@Y)4[R,gL6J(u$t0A8%^B#l=mQ>eO4u2]9"
"CBC_*jPQ`-:A`2ivq)M-Wq)M-jEoC9M2'MYvLLOLsf/NQ7RX,8)I>F[B,3F@=-idk2[sM-XW2j(X>s6a_/3-k]klG-46#=('_(3;>JBt#a;I*.PFJsRU7e+)b&7DtA>6<QosmI8#OU'#"
"'3V(voxO:$2puB=#@-.kpo[D*/vEFlw.)ioh^e_mZ[Z>#Lf)/#[5W[%rQnb4Jax9.vkh8.Ygx9.r:F.)4C4D#sbJI<xIg)Gnl4brBx5HDMO+1oX>:K#&^:KA2';f8%dI_Sk[%_Sx$)`H"
"f:U5%w(%q.:^7tL3gQxTNuf%8.gB#$qA3^#.j>%.J(W;R#[Kp(l:drnW%VZPN*tc'N,>>#3T&##d*<]k9f;9.$Bar-WfLk+mX+DWOY>>G.D###i$A+%VbCE4dCg/t[^B2'oO+01>q3]#"
"LAN-$;BP##s$:`$I$ZTMpG&$&gK?D*I4[]=.Y*,)cg=g$Xk[<-*tnv(689588FCB#]-I>#Hd@hF3:4D#4<GvC7>Lk+qtvQOwEDGD]j)l+cOf(Wdw(^G0x0wgDE*jL0R6J*].B+*2n6&5"
"4g]'5N:%],3q'j180X@,6JOA+RD.m-=./R3_vXYG;(/R3)gB.*Ynn8%3k#;/ni(?-2=(41#';$-od921s_rK*+DL1MYG;pLJ=Qu#6`?iL*k+_-37_F*?mZv$<7@W$)KsI3;O'>.-P39/"
"rl/H#%w5q']`M#G@smf(nUCq6i,<Poa@+P;_Q,p2.vi$H_D9At'0=F5=IRlL?_4D<lAp1Ta.,C#:%xC#txkQ#D-2*#?_&T.S3Zr#?SO$%o31H2LrUv-]WD.3&^ncHg(io@X;*/1g$ZSN"
"WmpoN3=;Eh9D)t%DIH[/2r^N2oo'f3BC@30p;g(aF2>>#d9%##UBQwBbr###)t_<L>M>c4a:M3a&t`L)=C[s$.lBJCZ$1O;3;nX7>8Fv>$f4OH5kh=BZ$Ni<WZMo&[JJl1W(&;BtbR_+"
"pcP9`g#sx+K9c1pA2sV.JJWv-2M0]?50%:1gJ8f3=dMX%sjE;$fun1*CSUJ5^skQLrS6c#V=j`?)B7p0YmVE#dSn*AG5_2)(OqG=HtM<9:]l@?nA=g3l+%##=bnwu,#4]#=PTK)6?J1F"
"-@CR#P[7t#49dY8K)pD#$m(G#;aTg-5fnQWfMkV-btEYfh>(u$T(Z:A[85L4eTB/`dlL_I+3-p;<;:/=2kBA#n*&X#'g9R`Cp`G5&VQ<:tM'nL*jn9D%4=#v4.Ro#7Nr5'2LdH#[%Sl#"
"Fx?+`l7@3D,Av]PfuE-Z6Y18.VB')3w(J1(pxW/)iYW0tK&xf3Hf+)4fUNmLmI+j=D$-+-Aaj5AdZs]Ae$%_H4k]P/0gNcutE@f>Cn?@579=g;A;v(?=f_^Himw=9k:''#H%M$#2xAgL"
"AE:sL0=`T.)AP##^4xU.,Mc##VA`T.PvK'#YsG<-N2:W.5rC$#:g`=-,fG<-?B`T.<.`$#LB`T.=xL$#]B`T.'C,+#q#XN0Lt*A#X5>##_RFgLb<tW#%A`T.0r_Z#k@`T.54.[#u@`T."
"'0Vc#)sG<-N2:W.D'F]#4g`=-,fG<-FB`T.NK'^#aB`T.M3X]#+C`T.w(20$*CfT%;$U`34cWdF>M_oD2<XG-MhW?-=Bnh2)/=eF%^TSDdZvc<(&QhFLF5p/)/JT%%]w%JnBrTC+^,LF"
"DZV=B5UOnBUHI31Ct7FHdZvRC:1k(IS7fP9@NXG-PmnUC$NR/;pvM=B5[HKF@][Q/gD/0FI-@j1885E='DvlEUE?U2wF6fGB0E,31-s]5fBDiL@IMiLg2NI34qLO1pb2_>->hoDRB*g2"
"73IL2qDfM10LB`]8:vLF0nrQWmJWnD1Zr&0lIrEH)6FVCfo8[5mHiTC$k`9C6P]:C:WI>HeZr`=(@YG<BK3oMAWPkM[]q?-4,6i2M3FGH,C%12+a^oDT.xr1E87r1#tS22Zm.+#+mk.#"
"cSq/#TH4.#HB+.#c:F&#Y#S-#Or:T.0E-(#M)B;-RfG<-9w'w-vZhpLM:SqL9_4rL'T#oLf_4rLge=rLg+m<-6.0_-'N&Y(uCVw0RMw9)i2s.CQe`uGBIcDFK1,XCutY'8s1L`E;cb9D"
"xLh%FKAu?B_qHe-4^,R<cj0A=]ZYMC>n)vH34=2CxLfP9e#J>Hsm;,<or3)Fa'(?-;0s+;Eje]Gq/C5BF(X>-EEDX1^Xk>--Ncq2Tk`'/YCrKGra4XCx'AVH#Vp]G:P26M?wXrL$*8qL"
"dJhkL%>UkL]$[lLmk?iMQE[]-;Rm--Z/;D3,gYc20GLe$nkv;-oi)M-6qJ7..T3rL<V?LME>UkL=_KC-Lv%?O8op*-2Zi+M@u^%83STp9K/w+#EY`=-iqmq&H-p92&r%;HQMw1BRDO-Q"
"a,ke-9e/F%8<_w')uW9Ve2s.CoLcYH#[B2CDt^>-^vUJDJo>a=dY9G;2djG<Cmkw0wmCW/C2oiLpU5.#:8K21N(rxp#r]JD4CH>HCn/^5+v4L#ooi34aLf?KKDWq)<u`f1s@VW&:e=rL"
"x5JqL4NpnLJU-+#1/dG%RkF6#>E^2#,7>U)A0]'/g3@##,AP##0Mc##4Yu##8f1$#<rC$#@(V$#D4i$#H@%%#LL7%#PXI%#Te[%#Xqn%#]'+&#8wdiL0FQ&#iKb&#mWt&#qd0'#upB'#"
"#'U'#'3h'#*qN+.]8^kLS8,H37Jr%4;cR]4?%4>5C=ku5GUKV6Kn,87O0do7SHDP8Wa%29>8*aj^+^*#b7p*#fC,+#jO>+#n[P+#rhc+#vtu+#$+2,#(7D,#,CV,#0Oi,#4[%-#8h7-#"
"`F31#?x&2#E7I8#D?S5#QB[8#Q0@8#i[*9#iOn8#X7o-#sb39#OE]5#b/k9##+b9#KN=.#MTF.#RZO.#Zst.#_/:/#c5C/#gG_/#lY$0#n`-0#sf60#wrH0#%/e0#S45##+C0:#4^+6#"
"=)lQ#m#jW#.Ml>#2Y(?#6f:?#:rL?#G$T6&-%S+H?fV.GoISZG)k%UC5EsXB0X66JCfRr1S:X7/o)6[&DK3kN;x?DIIdx5'-KFVCJa]p&6=ofGU,Tp&GM#mN35mvGOA,o*.=LVCKg/lN"
"#R:nDE>glNP*7oD5vI&#xhBB#$,P:vkiu'&AtjYd%4bkLn^Grd`F6LMOm;'UmmRfL0ecgLajA%#qr[fLS:-##rBg;->_Y-M8:>GMUIGHM,XPgLGV`'#gwCEHDTl##CwlhL&#TfLeZ7DE"
"<?*:)tx*GMD$JY-_GL_&PD>>#ZK'@'vW%##5d53#J`hR#SvTB#BDaD#Z7#F#HQ^q#I_e[#>eGj#%dFm#jT^q#L#js#rIuu#Gk*x#*j)%$_h(($Q)L+$,@p.$q>o1$*_mL-tHKC-OA=?/"
"rd0'#%^jjLH@6##OMYS.XP?(#O17`%^cR]4d;2>5i)po%Xj@M9#+v.:)A./(jxp(<23O`<AX')*pHoa45b9>5B9UY5kn8@6SrBJ:r,LV?imgr?i#HS@9ehB#GZpRnXFLS.;x,F.&'ru5"
"RPDX(-#gl8+hcP9,q(m9k0C2:qZHj:I47A=wG4&>w`K>?4P$Z?u9?_8(I,8@*@Xw96Nv9)%(AJ1)d5a4<QxKG<kNq;<R7X:DC`3Fv804F5^'v#/Vacse1]9Mj7c9D<Y2eH'/hQj*>'XU"
"Ac<X(MGHJ16>IwK=FDM9vv`i9ma%/:tjQN:9)^f:sMT`<2j8)=uAxD=(,Q]=wG4&>>iCB>1YMY>qx-;?.8hY?+5-v?m]c;@Mr(kkr0co@a`pP'T5x`kB]=%tSbD,3in=p88xW'AXuW'A"
"XuW'AoYBJ:C#8j:WF`Kl*inx=]*4>><;h3=2;72'KVi-QE[Q`k,[Q`k%r=Ji.+2>muwH,3n,-a+h3)/`-qolpL^^'J&K$)3CE+d3V8lD4kt)kkaf*XL]Y*XL]Y*XL]Y*XLfV(kkY2(kk"
"XAq%=DOY3OfV(kkiqqKP6WvE@6WvE@_xiu>6(9_Aa.JV?Q'gr?$&H;@SgkV@a>1s@Tg@?$W^LxXU`Fe-v5Ks-u$ffLoVZ)M:97;#3#`AMA`:%Mc&h(M`pp_MDuY<-i0;t-.7+gL54X<#"
"@IKC-XP;e.T=Z;#&fj/1cIm;#sH5+#tNi,#4Y5<-ns:P-x:cG-s*NM-GX3B-2Lx>-&A;=-)N#<-3S]F-Spj3MP-b^M(4pfL=#5$MC(i:#kUA(MCYL[M*;.&MX^T_MRnx#MK_e&MwZ,oL"
"*G.$v'lv9/><5'vG[B)Nu(h(M/Lt'M[x_%MD@@;#;fr=-)rY<->wWB-Cnls-%1xfLiRm`M404%vF6)=-0_W#.M+ofLk&h(M^-F'MCeB(Miv#`M*e>wNA2N*Mu>6)MT(i%Mi8H`Mx3o,8"
"Ud'B#g+BP8'@AaFum].#UgUx$0BAqDBG$T.-/5##$;ClLS?TG2&),##Rww%######$####WDK[k^=91#";

static const char* GetDefaultCompressedFontDataTTFBase85()
{
    return Karla_compressed_data_base85;
}
