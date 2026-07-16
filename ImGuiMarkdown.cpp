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
        case MD_BLOCK_HR:
            MD4CCallbacks::BLOCK_HR(enter);
            break;
        case MD_BLOCK_H:
            MD4CCallbacks::BLOCK_H((MD_BLOCK_H_DETAIL*)detail, enter);
            break;

        default:
            break;
    }
#ifdef DEBUG
    if (enter)
    {
        std::cout << "enter block " << block_debug[type] << '\n';
    }
    else
    {
        std::cout << "exit block " << block_debug[type] << '\n';
    }
#endif

    return 0;
}

int ImGuiMarkdown::Span(MD_SPANTYPE type, void* detail, bool enter)
{
    return 0;
}

int ImGuiMarkdown::Text(MD_TEXTTYPE t, const MD_CHAR* text, MD_SIZE size, void* u)
{
    std::string s(text, size);

    ImGui::TextUnformatted(s.c_str());

#ifdef DEBUG
    std::cout << s << '\n';
#endif

    return 0;
}
