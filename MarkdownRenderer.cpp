#include "MarkdownRenderer.h"

#include <imgui.h>
#include <md4c.h>

#include <string>

#ifdef DEBUG
#include <iostream>
#endif

MarkdownRenderer::MarkdownRenderer()
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

void MarkdownRenderer::Parse(const char* text, const size_t size)
{
    m_events.clear();
#ifdef DEBUG
    std::cout << "================================================" << '\n';
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
    std::cout << "================================================" << '\n';
#else
    md_parse(text, static_cast<MD_SIZE>(size), &m_Parser, this);
#endif
}

void MarkdownRenderer::Render()
{
    m_md4cCallbacks.Reset();
    
    for (auto& e : m_events)
    {
        switch (e.type) 
        {
            case EventType::Block:
                Block(e.blockType, e.detail, e.enter);
                break;
            case EventType::Span:
                Span(e.spanType, e.detail, e.enter);
                break;
            case EventType::Text:
                Text(e.textType, e.text);
                break;
        }
    }
}

int MarkdownRenderer::BlockCallback(MD_BLOCKTYPE type, void* detail, void* userdata, bool enter) 
{
    auto* renderer = static_cast<MarkdownRenderer*>(userdata);
    Event event {
            .type = EventType::Block,
            .enter = enter,
            .blockType = type,
    };

    switch (type)
    {
        case MD_BLOCK_UL:
            event.detail = *static_cast<MD_BLOCK_UL_DETAIL*>(detail);
            break;
        case MD_BLOCK_OL:
            event.detail = *static_cast<MD_BLOCK_OL_DETAIL*>(detail);
            break;
        case MD_BLOCK_LI:
            event.detail = *static_cast<MD_BLOCK_LI_DETAIL*>(detail);
            break;
        case MD_BLOCK_H:
            event.detail = *static_cast<MD_BLOCK_H_DETAIL*>(detail);
            break;
        case MD_BLOCK_CODE:
            event.detail = *static_cast<MD_BLOCK_CODE_DETAIL*>(detail);
            break;
        case MD_BLOCK_TABLE:
            event.detail = *static_cast<MD_BLOCK_TABLE_DETAIL*>(detail);
            break;
        case MD_BLOCK_TH:
            event.detail = *static_cast<MD_BLOCK_TD_DETAIL*>(detail);
            break;
        case MD_BLOCK_TD:
            event.detail = *static_cast<MD_BLOCK_TD_DETAIL*>(detail);
            break;
        default:
            break;
    }

    renderer->m_events.push_back(event);

    return 0;
}

int MarkdownRenderer::SpanCallback(MD_SPANTYPE type, void*, void* userdata, bool enter) 
{
    auto* renderer = static_cast<MarkdownRenderer*>(userdata);
    Event event {
            .type = EventType::Span,
            .enter = enter,
            .spanType = type,
    };

    renderer->m_events.push_back(event);

    return 0;
}

int MarkdownRenderer::TextCallback(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    auto* renderer = static_cast<MarkdownRenderer*>(userdata);
    std::string copy(text, size);

    Event event {
            .type = EventType::Text,
            .textType = type,
            .text = copy
    };

    renderer->m_events.push_back(event);

    return 0;
}

int MarkdownRenderer::Block(MD_BLOCKTYPE type, MD_DETAIL detail, bool enter)
{
    switch (type)
    {
        case MD_BLOCK_QUOTE:
            m_md4cCallbacks.BLOCK_QUOTE(enter);
            break;
        case MD_BLOCK_UL:
            m_md4cCallbacks.BLOCK_UL(std::get<MD_BLOCK_UL_DETAIL>(detail), enter);
            break;
        case MD_BLOCK_OL:
            m_md4cCallbacks.BLOCK_OL(std::get<MD_BLOCK_OL_DETAIL>(detail), enter);
            break;
        case MD_BLOCK_LI:
            m_md4cCallbacks.BLOCK_LI(std::get<MD_BLOCK_LI_DETAIL>(detail), enter);
            break;
        case MD_BLOCK_HR:
            m_md4cCallbacks.BLOCK_HR(enter);
            break;
        case MD_BLOCK_H:
            m_md4cCallbacks.BLOCK_H(std::get<MD_BLOCK_H_DETAIL>(detail), enter);
            break;
        case MD_BLOCK_CODE:
            m_md4cCallbacks.BLOCK_CODE(std::get<MD_BLOCK_CODE_DETAIL>(detail), enter);
            break;
        case MD_BLOCK_P:
            m_md4cCallbacks.BLOCK_P(enter);
            break;
        case MD_BLOCK_TABLE:
            m_md4cCallbacks.BLOCK_TABLE(std::get<MD_BLOCK_TABLE_DETAIL>(detail), enter);
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
            m_md4cCallbacks.BLOCK_TD(std::get<MD_BLOCK_TD_DETAIL>(detail), enter);
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

int MarkdownRenderer::Span(MD_SPANTYPE type, MD_DETAIL, bool enter)
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

void MarkdownRenderer::renderText(const std::string& text)
{
    // Handle table header text writing
    // ?? Does the table header should also be rich text ??
    if (m_md4cCallbacks.m_tableState.header)
    {
        ImGui::TableSetupColumn(text.c_str());
    }
    // Default case
    else
    {
        m_md4cCallbacks.m_spanStack.back().buffer += text;
    }

#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

void MarkdownRenderer::renderCode(const std::string& text)
{
    if (m_md4cCallbacks.m_isInCodeBlock)
        m_md4cCallbacks.m_codeTextBuffer += text;
    else
        m_md4cCallbacks.m_spanStack.back().buffer += text;

#ifdef DEBUG
    std::cout << s << '\n';
#endif
}

int MarkdownRenderer::Text(MD_TEXTTYPE type, const std::string& text)
{
#ifdef DEBUG
    std::cout << "type " << text_debug[type] << '\n';
#endif

    switch (type)
    {
        case MD_TEXT_NORMAL:
            renderText(text);
            break;
        case MD_TEXT_CODE:
            renderCode(text);
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
