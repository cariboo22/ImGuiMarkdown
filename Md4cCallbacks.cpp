#include "Md4cCallbacks.h"

#include <imgui.h>
#include <md4c.h>

#include <string>

#include "ImGuiMarkdown.h"

void MD4CCallbacks::RenderRichText()
{
    ImVec2 pos = ImGui::GetCursorScreenPos();
    float wrapWidth = ImGui::GetContentRegionAvail().x;

    const float startX = pos.x;
    const float startY = pos.y;

    float x = pos.x;
    float y = pos.y;

    float maxHeight = 0.0f;
    ImDrawList* imDrawList = ImGui::GetWindowDrawList();

    for (const auto& span : m_spanStack)
    {
        if (span.buffer == "")
            continue;

        ImFont* font = span.font ? span.font : ImGui::GetFont();
        float fontSize = span.fontSize > 0 ? span.fontSize : ImGui::GetFontSize();

        float lineHeight = fontSize;
        maxHeight = std::max(maxHeight, lineHeight);

        std::string current;

        auto flushWord = [&]()
        {
            if (current.empty())
                return;

            ImVec2 size = font->CalcTextSizeA(
                fontSize,
                FLT_MAX,
                0.0f,
                current.c_str());

            if (x > startX &&
                x + size.x > startX + wrapWidth)
            {
                x = startX;
                y += lineHeight;
            }

            imDrawList->AddText(
                font,
                fontSize,
                ImVec2(x, y),
                span.color,
                current.c_str());

            x += size.x;
            current.clear();
        };
        auto addSpace = [&]()
        {
            ImVec2 size = font->CalcTextSizeA(
                fontSize,
                FLT_MAX,
                0.0f,
                " ");

            if (x > startX &&
                x + size.x > startX + wrapWidth)
            {
                x = startX;
                y += lineHeight;
            }
            else
            {
                x += size.x;
            }
        };

        for (const auto& c : span.buffer)
        {
            switch (c)
            {
                case ' ':
                    flushWord();
                    addSpace();
                    break;

                case '\n':
                    flushWord();
                    x = startX;
                    y += lineHeight;
                    break;

                default:
                    current += c;
                    break;
            }            
        }

        flushWord();
    }

    ImGui::Dummy(ImVec2(0.0f, y + maxHeight - startY));
}

void MD4CCallbacks::DrawQuote(ImVec2 startPos, ImVec2 endPos)
{
    // Draw small quote rect
    ImVec2 startRectPos = ImVec2(startPos.x - (ImGuiMarkdown::s_config.indentSize/2.0f - ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                                 startPos.y - ImGui::GetStyle().ItemSpacing.y/2.0f);
    ImVec2 endRectPos = ImVec2(endPos.x - (ImGuiMarkdown::s_config.indentSize/2.0f + ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                               endPos.y);

    ImGui::GetWindowDrawList()->AddRectFilled(
        startRectPos,
        endRectPos,
        ImGuiMarkdown::s_config.quoteColor
    );

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

/* === Block definitions === */

void MD4CCallbacks::BLOCK_DEFAULT(bool enter)
{
    if (enter)
    {
        MD4CCallbacks::m_spanStack.clear();
        MD4CCallbacks::m_spanStack.push_back({});
    }
    else
    {
        MD4CCallbacks::RenderRichText();
        MD4CCallbacks::m_spanStack.clear();
    }
}

void MD4CCallbacks::BLOCK_QUOTE(bool enter)
{
    if (enter)
    {
        ImGui::Indent(ImGuiMarkdown::s_config.indentSize);
        m_quoteStack.push_back({.startPos = ImGui::GetCursorScreenPos(), .depth = m_quoteStack.size()+1});
    }
    else
    {
        DrawQuote(m_quoteStack.back().startPos, ImGui::GetCursorScreenPos());

        m_quoteStack.pop_back();
        ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);
    }
}

void MD4CCallbacks::BLOCK_UL(MD_BLOCK_UL_DETAIL detail, bool enter)
{
    if (enter)
    {
        // Renders the text that was potentialy in a previous BLOCK_LI
        // In the case of a multiple stage list
        RenderRichText();
        m_spanStack.clear();

        if (m_listStack.size() > 0)
            ImGui::Indent(ImGuiMarkdown::s_config.indentSize);

        m_listStack.push_back({.isUnordered = true, .mark = detail.mark});
    }
    else
    {
        if (m_listStack.size() > 1)
            ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);

        m_listStack.pop_back();

        m_spanStack.clear();
    }
}

void MD4CCallbacks::BLOCK_OL(MD_BLOCK_OL_DETAIL detail, bool enter)
{
    if (enter)
    {
        // Renders the text that was potentialy in a previous BLOCK_LI
        // In the case of a multiple stage list
        RenderRichText();
        m_spanStack.clear();

        if (m_listStack.size() > 0)
            ImGui::Indent(ImGuiMarkdown::s_config.indentSize);
        
        m_listStack.push_back({.isUnordered = false,
                               .start = detail.start,
                               .mark = detail.mark_delimiter});
    }
    else
    {
        if (m_listStack.size() > 1)
            ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);

        m_listStack.pop_back();

        m_spanStack.clear();
    }
}

void MD4CCallbacks::BLOCK_LI(MD_BLOCK_LI_DETAIL, bool enter)
{
    if (enter)
    {
        auto& listBack = m_listStack.back();

        if (listBack.isUnordered)
        {
            ImGui::Bullet();

            m_spanStack.clear();
            m_spanStack.push_back({});
        }
        else
        {
            m_spanStack.clear();
            m_spanStack.push_back({});

            m_spanStack.back().buffer = std::to_string(listBack.start + listBack.counter++) + listBack.mark + " ";
        }

    }
    else
    {
        RenderRichText();
        m_spanStack.clear();
    }
}

void MD4CCallbacks::BLOCK_HR(bool enter)
{
    if (!enter)
    {
        ImGui::NewLine();
        ImGui::Separator();
    }
}

void MD4CCallbacks::BLOCK_H(MD_BLOCK_H_DETAIL detail, bool enter)
{
    if (enter)
    {
        ImGui::NewLine();
        m_spanStack.clear();

        ImGui::PushFont(ImGuiMarkdown::s_config.GetFont(MarkdownConfig::FONT_H1 + detail.level-1));
        m_spanStack.push_back({});
    }
    else
    {
        RenderRichText();
        m_spanStack.clear();

        ImGui::PopFont();
        
        if (detail.level <= 2)
        {
            ImGui::Separator();
            ImGui::NewLine();
        }
    }
}

void MD4CCallbacks::BLOCK_CODE(const MD_BLOCK_CODE_DETAIL, bool enter)
{
    if (enter)
    {
        m_isInCodeBlock = true;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGuiMarkdown::s_config.codeBlockCornerRadius);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGuiMarkdown::s_config.codeBlockBGColor);

        ImGui::BeginChild(("##code_" + std::to_string(m_counter.code++)).c_str(), ImVec2(0.0f, 0.0f), 
            ImGuiChildFlags_Borders | 
            ImGuiChildFlags_AutoResizeY
            );
        m_codeTextBuffer = "";
    }
    else
    {
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted(m_codeTextBuffer.c_str());
        ImGui::PopTextWrapPos();
        
        m_codeTextBuffer = "";
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::EndChild();

        m_isInCodeBlock = false;
    }
}

void MD4CCallbacks::BLOCK_P(bool enter)
{
    if (enter)
    {
        m_spanStack.push_back({});
    }
    else
    {
        RenderRichText();
        m_spanStack.clear();
    }
}

void MD4CCallbacks::BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL detail, bool enter)
{
    if (enter)
    {
        m_tableState.tableName = "##table_" + std::to_string(m_counter.table++);

        ImGui::BeginTable(
            m_tableState.tableName.c_str(),
            static_cast<int>(detail.col_count),
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

void MD4CCallbacks::BLOCK_THEAD(bool enter)
{
    if (enter)
    {
        m_tableState.header = true;
    }
    else
    {
        m_tableState.header = false;
    }
}

void MD4CCallbacks::BLOCK_TR(bool enter)
{
    if (enter && !m_tableState.header)
    {
        ImGui::TableNextRow();
    }
    else if (!enter && m_tableState.header)
    {
        ImGui::TableHeadersRow();
    }
}

void MD4CCallbacks::BLOCK_TD(const MD_BLOCK_TD_DETAIL, bool enter)
{
    if (enter)
    {
        ImGui::TableNextColumn();
        m_spanStack.clear();
        m_spanStack.push_back({});
    }
    else
    {
        RenderRichText();
        m_spanStack.clear();
    }
}

/* === Span definitions === */

void MD4CCallbacks::SPAN_EM(bool enter)
{
    if (enter)
    {
        m_italic = true;
        if (m_italic && m_bold)
            ImGui::PushFont(ImGuiMarkdown::s_config.GetFont(MarkdownConfig::FONT_BOLDITALIC));
        else
            ImGui::PushFont(ImGuiMarkdown::s_config.GetFont(MarkdownConfig::FONT_ITALIC));
    }
    else
    {
        m_italic = false;
        ImGui::PopFont();
    }
}

void MD4CCallbacks::SPAN_STRONG(bool enter)
{
    if (enter)
    {
        m_bold = true;
        if (m_italic && m_bold)
            ImGui::PushFont(ImGuiMarkdown::s_config.GetFont(MarkdownConfig::FONT_BOLDITALIC));
        else
            ImGui::PushFont(ImGuiMarkdown::s_config.GetFont(MarkdownConfig::FONT_BOLD));
    }
    else
    {
        m_bold = false;
        ImGui::PopFont();
    }
}

void MD4CCallbacks::SPAN_CODE(bool enter)
{
    if (enter)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGuiMarkdown::s_config.codeSpanTextColor);
    }
    else
    {
        ImGui::PopStyleColor();
        m_codeTextBuffer = "";
    }
}
