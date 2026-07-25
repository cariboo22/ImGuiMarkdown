#include "ImGuiMarkdown.h"

#include <cstddef>
#include <imgui.h>
#include <md4c.h>
#include <string>

#ifdef DEBUG
#include <iostream>
#endif

ImGuiMarkdown::ImGuiMarkdown()
{
    m_Parser.abi_version = 0;
    m_Parser.flags = s_config.parserFlags;

    m_Parser.enter_block = EnterBlockCallback;
    m_Parser.leave_block = LeaveBlockCallback;

    m_Parser.enter_span = EnterSpanCallback;
    m_Parser.leave_span = LeaveSpanCallback;

    m_Parser.text = TextCallback;

    m_Parser.debug_log = nullptr;
    m_Parser.syntax = nullptr;
}

void ImGuiMarkdown::Parse(const char* text, const size_t size)
{
    m_md4cCallbacks.Reset();

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
            m_md4cCallbacks.BLOCK_QUOTE(enter);
            break;
        case MD_BLOCK_UL:
            m_md4cCallbacks.BLOCK_UL((MD_BLOCK_UL_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_OL:
            m_md4cCallbacks.BLOCK_OL((MD_BLOCK_OL_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_LI:
            m_md4cCallbacks.BLOCK_LI((MD_BLOCK_LI_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_HR:
            m_md4cCallbacks.BLOCK_HR(enter);
            break;
        case MD_BLOCK_H:
            m_md4cCallbacks.BLOCK_H((MD_BLOCK_H_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_CODE:
            m_md4cCallbacks.BLOCK_CODE((MD_BLOCK_CODE_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_P:
            m_md4cCallbacks.BLOCK_P(enter);
            break;
        case MD_BLOCK_TABLE:
            m_md4cCallbacks.BLOCK_TABLE((MD_BLOCK_TABLE_DETAIL*) detail, enter);
            break;
        case MD_BLOCK_THEAD:
            m_md4cCallbacks.BLOCK_THEAD(enter);
            break;
        case MD_BLOCK_TBODY:
            break;
        case MD_BLOCK_TR:
            m_md4cCallbacks.BLOCK_TR(enter);
            break;
        case MD_BLOCK_TH:
            break;
        case MD_BLOCK_TD:
            m_md4cCallbacks.BLOCK_TD((MD_BLOCK_TD_DETAIL*) detail, enter);
            break;
        default:
            m_md4cCallbacks.BLOCK_DEFAULT(enter);
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

int ImGuiMarkdown::Span(MD_SPANTYPE type, void*, bool enter)
{
    switch (type)
    {
        case MD_SPAN_EM:
            m_md4cCallbacks.SPAN_EM(enter);
            break;
        case MD_SPAN_STRONG:
            m_md4cCallbacks.SPAN_STRONG(enter);
            break;
        case MD_SPAN_CODE:
            m_md4cCallbacks.SPAN_CODE(enter);
            break;
        default:
            break;
    }

    m_md4cCallbacks.m_spanStack.push_back({});

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

void ImGuiMarkdown::renderText(const char* text, const std::size_t size)
{
    std::string s(text, size);

    // Handle table header text writing
    // ?? Does the table header should also be rich text ??
    if (m_md4cCallbacks.m_tableState.header)
    {
        ImGui::TableSetupColumn(s.c_str());
    }
    // Default case
    else
    {
        m_md4cCallbacks.m_spanStack.back().buffer += s;
    }

#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

void ImGuiMarkdown::renderCode(const char* text, const std::size_t size)
{
    std::string s(text, size);
    if (m_md4cCallbacks.m_isInCodeBlock)
        m_md4cCallbacks.m_codeTextBuffer += s;
    else
        m_md4cCallbacks.m_spanStack.back().buffer += s;

#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

int ImGuiMarkdown::Text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size)
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
            m_md4cCallbacks.m_spanStack.back().buffer += " ";
            break;
        case MD_TEXT_BR:
            m_md4cCallbacks.m_spanStack.back().buffer += "\n";
            break;
        default:
            break;
    }

    return 0;
}
