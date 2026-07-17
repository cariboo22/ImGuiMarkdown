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
        int tableCounter = 0;
        std::string tableName = "";
    };

    static TableState s_tableState {};
    static int s_quoteDepth = 0;
    static bool s_softBR = false;

    /* === Block definitions === */
    
    inline void BLOCK_QUOTE(bool enter)
    {
        if (enter)
        {
            s_quoteDepth++;
            ImGui::Indent(ImGuiMarkdown::config.indentSize);
        }
        else
        {
            ImGui::Unindent(ImGuiMarkdown::config.indentSize);
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
    inline void BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL* detail, bool enter)
    {
        if (enter)
        {
            s_tableState.tableName = "##markdown_table_" + std::to_string(s_tableState.tableCounter++);

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
