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
    MD4CCallbacks::s_counter = {};

#ifdef DEBUG
    std::cout << "================================================" << '\n';
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
    std::cout << "================================================" << '\n';
#else
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
#endif
}


ImFont* ImGuiMarkdown::GetFont(unsigned int id)
{
    if (id < std::size(s_Fonts) && s_Fonts[id] != nullptr)
    {
        return s_Fonts[id];
    }
    else
    {
        return ImGui::GetFont();
    }
}

int ImGuiMarkdown::Block(MD_BLOCKTYPE type, void* detail, bool enter)
{
    switch (type)
    {
        case MD_BLOCK_QUOTE:
            MD4CCallbacks::BLOCK_QUOTE(enter);
            break;
        case MD_BLOCK_UL:
            MD4CCallbacks::BLOCK_UL((MD_BLOCK_UL_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_OL:
            MD4CCallbacks::BLOCK_OL((MD_BLOCK_OL_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_LI:
            MD4CCallbacks::BLOCK_LI((MD_BLOCK_LI_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_HR:
            MD4CCallbacks::BLOCK_HR(enter);
            break;
        case MD_BLOCK_H:
            MD4CCallbacks::BLOCK_H((MD_BLOCK_H_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_CODE:
            MD4CCallbacks::BLOCK_CODE((MD_BLOCK_CODE_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_P:
            MD4CCallbacks::BLOCK_P(enter);
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
            MD4CCallbacks::BLOCK_DEFAULT(enter);
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
    switch (type)
    {
        case MD_SPAN_EM:
            MD4CCallbacks::SPAN_EM(enter);
            break;
        case MD_SPAN_STRONG:
            MD4CCallbacks::SPAN_STRONG(enter);
            break;
        case MD_SPAN_CODE:
            MD4CCallbacks::SPAN_CODE(enter);
            break;
        default:
            break;
    }

    MD4CCallbacks::s_SpanStack.push_back({});

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
    if (MD4CCallbacks::s_quoteDepth > 0)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();

        // Draw small quite rect
        ImVec4 rectColor = ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab];
        ImVec2 startRectPos = ImVec2(p.x - (ImGuiMarkdown::s_config.indentSize/2.0f - ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                                     p.y - ImGui::GetStyle().ItemSpacing.y/2.0f);
        ImVec2 endRectPos = ImVec2(p.x - (ImGuiMarkdown::s_config.indentSize/2.0f + ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                                   p.y + ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y/2.0f);

        for (int d = 0; d < MD4CCallbacks::s_quoteDepth; d++)
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                startRectPos,
                endRectPos,
                ImGui::ColorConvertFloat4ToU32(rectColor)
            );

            startRectPos.x -= ImGuiMarkdown::s_config.indentSize;
            endRectPos.x -= ImGuiMarkdown::s_config.indentSize;
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

    // Handle table header text writing
    // ?? Does the table header should also be rich text ??
    if (MD4CCallbacks::s_tableState.header)
    {
        ImGui::TableSetupColumn(s.c_str());
    }
    // Default case
    else
    {
        MD4CCallbacks::s_SpanStack.back().buffer += s;
    }

#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

void renderCode(const char* text, const std::size_t size)
{
    std::string s(text, size);
    if (MD4CCallbacks::s_isInCodeBlock)
        MD4CCallbacks::s_codeTextBuffer += s;
    else
        MD4CCallbacks::s_SpanStack.back().buffer += s;

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
            renderCode(text, size);
            break;
        case MD_TEXT_SOFTBR:
            MD4CCallbacks::s_SpanStack.back().buffer += " ";
            break;
        case MD_TEXT_BR:
            MD4CCallbacks::s_SpanStack.back().buffer += "\n";
            break;
        default:
            break;
    }

    return 0;
}
