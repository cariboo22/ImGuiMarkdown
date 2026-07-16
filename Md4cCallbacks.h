#pragma once

#include <imgui.h>
#include <md4c.h>

#include "ImGuiMarkdown.h"

namespace MD4CCallbacks
{
    inline void BLOCK_HR(bool enter)
    {
        if (!enter)
        {
            ImGui::NewLine();
            ImGui::Separator();
        }
    }

    inline void BLOCK_H(MD_BLOCK_H_DETAIL* detail, bool enter)
    {
        if (enter)
        {
            ImGui::NewLine();
            if (detail->level <= 3)
                ImGui::PushFont(ImGuiMarkdown::GetFont(detail->level-1));
        }
        else
        {
            if (detail->level <= 3)
                ImGui::PopFont();

            if (detail->level <= 2)
            {
                ImGui::Separator();
                ImGui::NewLine();
            }
        }
    }
}

