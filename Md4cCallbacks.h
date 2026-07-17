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

    static TableState m_imgui_table {};

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
    inline void BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL* d, bool e)
    {
        if (e)
        {
            m_imgui_table.tableName = "##markdown_table_" + std::to_string(m_imgui_table.tableCounter++);

            ImGui::BeginTable(
                m_imgui_table.tableName.c_str(),
                static_cast<int>(d->col_count),
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

    inline void BLOCK_THEAD(bool e)
    {
        if (e)
        {
            m_imgui_table.header = true;
        }
        else
        {
            m_imgui_table.header = false;
        }
    }

    inline void BLOCK_TR(bool e)
    {
        if (e && !m_imgui_table.header)
        {
            ImGui::TableNextRow();
        }
        else if (!e && m_imgui_table.header)
        {
            ImGui::TableHeadersRow();
        }
    }

    inline void BLOCK_TD(const MD_BLOCK_TD_DETAIL*, bool e)
    {
        if (e)
        {
            ImGui::TableNextColumn();
        }
    }
}
