#pragma once

#include <imgui.h>
#include <md4c.h>
#include <string>

#include "ImGuiMarkdown.h"

namespace MD4CCallbacks
{
    struct TableState
    {
        bool header = false;
        std::string tableName = "";
    };

    struct Counter
    {
        int table = 0;
        int code = 0;
    };

    static TableState s_tableState {};
    static Counter s_counter {};
    static int s_quoteDepth = 0;
    static bool s_softBR = false;

    /* === Block definitions === */
    
    inline void BLOCK_QUOTE(bool enter)
    {
        if (enter)
        {
            s_quoteDepth++;
            ImGui::Indent(ImGuiMarkdown::s_config.indentSize);
        }
        else
        {
            ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);
            s_quoteDepth--;
        }
    }

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

    inline void BLOCK_CODE(const MD_BLOCK_CODE_DETAIL*, bool enter)
    {
        if (enter)
        {
            ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg];
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGuiMarkdown::s_config.codeBlockCornerRadius);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertFloat4ToU32(bgColor));

            ImGui::BeginChild(("##code_" + std::to_string(s_counter.code++)).c_str(), ImVec2(0.0f, 0.0f), 
                ImGuiChildFlags_Borders | 
                ImGuiChildFlags_AutoResizeY
                );
        }
        else
        {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::EndChild();
        }
    }

    inline void BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL* detail, bool enter)
    {
        if (enter)
        {
            s_tableState.tableName = "##table_" + std::to_string(s_counter.table++);

            ImGui::BeginTable(
                s_tableState.tableName.c_str(),
                static_cast<int>(detail->col_count),
                ImGuiTableFlags_Borders |
                ImGuiTableFlags_RowBg |
                ImGuiTableFlags_Resizable |
                ImGuiTableFlags_SizingStretchProp
            );
        }
        else
        {
            ImGui::EndTable();
            ImGui::NewLine();
        }
    }

    inline void BLOCK_THEAD(bool enter)
    {
        if (enter)
        {
            s_tableState.header = true;
        }
        else
        {
            s_tableState.header = false;
        }
    }

    inline void BLOCK_TR(bool enter)
    {
        if (enter && !s_tableState.header)
        {
            ImGui::TableNextRow();
        }
        else if (!enter && s_tableState.header)
        {
            ImGui::TableHeadersRow();
        }
    }

    inline void BLOCK_TD(const MD_BLOCK_TD_DETAIL*, bool enter)
    {
        if (enter)
        {
            ImGui::TableNextColumn();
        }
    }
}
