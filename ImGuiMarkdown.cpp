#include "ImGuiMarkdown.h"

#include <cstddef>
#include <imgui.h>
#include <md4c.h>
#include <string>

#include "Md4cCallbacks.h"

#ifdef DEBUG
#include <iostream>
#endif

ImGuiMarkdown::ImGuiMarkdown()
{
    m_Parser.abi_version = 0;
    m_Parser.flags = MD_FLAG_TABLES;

    m_Parser.enter_block = EnterBlock;
    m_Parser.leave_block = LeaveBlock;

    m_Parser.enter_span = EnterSpan;
    m_Parser.leave_span = LeaveSpan;

    m_Parser.text = Text;

    m_Parser.debug_log = nullptr;
    m_Parser.syntax = nullptr;
}

void ImGuiMarkdown::Parse(const char* text, const size_t size)
{
    MD4CCallbacks::s_tableState = {};

#ifdef DEBUG
    std::cout << "================================================" << '\n';
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
    std::cout << "================================================" << '\n';
#else
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
#endif
}


ImFont* ImGuiMarkdown::GetFont(unsigned header)
{
    if (header < 3)
        return m_H[header];
    else
        return nullptr;
}

int ImGuiMarkdown::Block(MD_BLOCKTYPE type, void* detail, bool enter)
{
    switch (type)
    {
        case MD_BLOCK_QUOTE:
            MD4CCallbacks::BLOCK_QUOTE(enter);
            break;
        case MD_BLOCK_HR:
            MD4CCallbacks::BLOCK_HR(enter);
            break;
        case MD_BLOCK_H:
            MD4CCallbacks::BLOCK_H((MD_BLOCK_H_DETAIL*)detail, enter);
            break;
        case MD_BLOCK_TABLE:
            MD4CCallbacks::BLOCK_TABLE((MD_BLOCK_TABLE_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_THEAD:
            MD4CCallbacks::BLOCK_THEAD(enter);
            break;
        case MD_BLOCK_TBODY:
            break;
        case MD_BLOCK_TR:
            MD4CCallbacks::BLOCK_TR(enter);
            break;
        case MD_BLOCK_TH:
            break;
        case MD_BLOCK_TD:
            MD4CCallbacks::BLOCK_TD((MD_BLOCK_TD_DETAIL*) detail, enter);
            break;
        default:
            break;
    }
#ifdef DEBUG
    if (enter)
    {
        std::cout << "enter " << block_debug[type] << '\n';
    }
    else
    {
        std::cout << "exit " << block_debug[type] << '\n';
    }
#endif

    return 0;
}

int ImGuiMarkdown::Span(MD_SPANTYPE type, void* detail, bool enter)
{
#ifdef DEBUG
    if (enter)
    {
        std::cout << "enter " << span_debug[type] << '\n';
    }
    else
    {
        std::cout << "exit " << span_debug[type] << '\n';
    }
#endif
    return 0;
}

void renderText(const char* text, const std::size_t size)
{
    std::string s(text, size);

    // Rendering quote rectangle and background
    if (MD4CCallbacks::s_quoteDepth > 0 && !MD4CCallbacks::s_softBR)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        // Draw small quite rect
        ImVec4 rectColor = ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab];
        ImVec2 startRectPos = ImVec2(p.x - (ImGuiMarkdown::config.indentSize/2.0f - ImGuiMarkdown::config.quoteRectThickness/2.0f),
                                     p.y - ImGui::GetStyle().ItemSpacing.y/2.0f);
        ImVec2 endRectPos = ImVec2(p.x - (ImGuiMarkdown::config.indentSize/2.0f + ImGuiMarkdown::config.quoteRectThickness/2.0f),
                                   p.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y/2.0f);

        for (int d = 0; d < MD4CCallbacks::s_quoteDepth; d++)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                startRectPos,
                endRectPos,
                ImGui::ColorConvertFloat4ToU32(rectColor)
            );

            startRectPos.x -= ImGuiMarkdown::config.indentSize;
            endRectPos.x -= ImGuiMarkdown::config.indentSize;
        }

        // Background rect
        // ImVec4 rectColorBG = ImGui::GetStyle().Colors[ImGuiCol_Separator];
        // ImGui::GetWindowDrawList()->AddRectFilled(
        //     startRectPos,
        //     ImVec2(p.x + ImGui::GetContentRegionAvail().x,
        //            // + static_cast<float>(MD4CCallbacks::s_quoteDepth-1) * ImGuiMarkdown::config.indentSize,
        //            p.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y/2.0f),
        //     ImGui::ColorConvertFloat4ToU32(rectColorBG)
        // );
    }

    if (MD4CCallbacks::s_softBR)
        MD4CCallbacks::s_softBR = false;

    // Handle table header text writing
    if (MD4CCallbacks::s_tableState.header)
    {
        ImGui::TableSetupColumn(s.c_str());
    }
    else
    {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted(s.c_str());
        ImGui::PopTextWrapPos();
    }
#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

int ImGuiMarkdown::Text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void*)
{
#ifdef DEBUG
    std::cout << "type " << text_debug[type] << '\n';
#endif

    switch (type)
    {
        case MD_TEXT_NORMAL:
            renderText(text, size);
            break;
        case MD_TEXT_CODE:
            renderText(text, size);
            break;
        case MD_TEXT_SOFTBR:
            ImGui::SameLine();
            MD4CCallbacks::s_softBR = true;
            break;
        default:
            break;
    }

    return 0;
}
