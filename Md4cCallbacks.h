#pragma once

#include <imgui.h>
#include <md4c.h>

#include <string>
#include <vector>

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

    struct List
    {
        bool isUnordered = false;
        unsigned int start = 0;
        unsigned int counter = 0;
        char mark;
    };

    struct Quote
    {
        ImVec2 startPos {};
        unsigned int depth = 0;
    };

    static TableState s_tableState {};
    static bool s_isInCodeBlock = false;

    static Counter s_counter {};
    static std::string s_codeTextBuffer = "";

    static unsigned int s_quoteDepth = 0;
    static std::vector<Quote> s_quoteStack {};

    static int s_listDepth = 0;
    static std::vector<List> s_listStack {};

    static bool s_italic = false;
    static bool s_bold = false;

    struct Span
    {
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
        ImFont* font = ImGui::GetFont();
        float fontSize = ImGui::GetFontSize();
        // ?? Is std::string good for string buffer ??
        std::string buffer = "";
    };
    static std::vector<Span> s_SpanStack {};

    inline void RenderRichText()
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float wrapWidth = ImGui::GetContentRegionAvail().x;

        const float startX = pos.x;
        const float startY = pos.y;

        float x = pos.x;
        float y = pos.y;

        float maxHeight = 0.0f;
        ImDrawList* imDrawList = ImGui::GetWindowDrawList();

        for (const auto& span : s_SpanStack)
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

    inline void DrawQuote(ImVec2 startPos, ImVec2 endPos)
    {
        // Draw small quote rect
        ImVec4 rectColor = ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab];
        ImVec2 startRectPos = ImVec2(startPos.x - (ImGuiMarkdown::s_config.indentSize/2.0f - ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                                     startPos.y - ImGui::GetStyle().ItemSpacing.y/2.0f);
        ImVec2 endRectPos = ImVec2(endPos.x - (ImGuiMarkdown::s_config.indentSize/2.0f + ImGuiMarkdown::s_config.quoteRectThickness/2.0f),
                                   endPos.y);

        ImGui::GetWindowDrawList()->AddRectFilled(
            startRectPos,
            endRectPos,
            ImGui::ColorConvertFloat4ToU32(rectColor)
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
    
    inline void BLOCK_DEFAULT(bool enter)
    {
        if (enter)
        {
            MD4CCallbacks::s_SpanStack.clear();
            MD4CCallbacks::s_SpanStack.push_back({});
        }
        else
        {
            MD4CCallbacks::RenderRichText();
            MD4CCallbacks::s_SpanStack.clear();
        }
    }

    inline void BLOCK_QUOTE(bool enter)
    {
        if (enter)
        {
            s_quoteDepth++;
            ImGui::Indent(ImGuiMarkdown::s_config.indentSize);
            s_quoteStack.push_back({.startPos = ImGui::GetCursorScreenPos(), .depth = s_quoteDepth});
        }
        else
        {
            DrawQuote(s_quoteStack.back().startPos, ImGui::GetCursorScreenPos());

            s_quoteStack.pop_back();
            ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);
            s_quoteDepth--;
        }
    }

    inline void BLOCK_UL(MD_BLOCK_UL_DETAIL* detail, bool enter)
    {
        if (enter)
        {
            // Renders the text that was potentialy in a previous BLOCK_LI
            // In the case of a multiple stage list
            RenderRichText();
            s_SpanStack.clear();

            if (s_listDepth > 0)
                ImGui::Indent(ImGuiMarkdown::s_config.indentSize);

            s_listDepth++;
            s_listStack.push_back({.isUnordered = true, .mark = detail->mark});
        }
        else
        {
            if (s_listDepth > 1)
                ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);

            s_listDepth--;
            s_listStack.pop_back();

            s_SpanStack.clear();
        }
    }

    inline void BLOCK_OL(MD_BLOCK_OL_DETAIL* detail, bool enter)
    {
        if (enter)
        {
            // Renders the text that was potentialy in a previous BLOCK_LI
            // In the case of a multiple stage list
            RenderRichText();
            s_SpanStack.clear();

            if (s_listDepth > 0)
                ImGui::Indent(ImGuiMarkdown::s_config.indentSize);
            
            s_listDepth++;
            s_listStack.push_back({.isUnordered = false,
                                   .start = detail->start,
                                   .mark = detail->mark_delimiter});
        }
        else
        {
            if (s_listDepth > 1)
                ImGui::Unindent(ImGuiMarkdown::s_config.indentSize);

            s_listDepth--;
            s_listStack.pop_back();

            s_SpanStack.clear();
        }
    }

    inline void BLOCK_LI(MD_BLOCK_LI_DETAIL*, bool enter)
    {
        if (enter)
        {
            auto& listBack = s_listStack.back();

            if (listBack.isUnordered)
            {
                ImGui::Bullet();

                s_SpanStack.clear();
                s_SpanStack.push_back({});
            }
            else
            {
                s_SpanStack.clear();
                s_SpanStack.push_back({});

                s_SpanStack.back().buffer = std::to_string(listBack.start + listBack.counter++) + listBack.mark + " ";
            }

        }
        else
        {
            RenderRichText();
            s_SpanStack.clear();
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
            s_SpanStack.clear();

            ImGui::PushFont(ImGuiMarkdown::GetFont(ImGuiMarkdown::FONT_H1 + detail->level-1));
            s_SpanStack.push_back({});
        }
        else
        {
            RenderRichText();
            s_SpanStack.clear();

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
            s_isInCodeBlock = true;

            ImVec4 bgColor = ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg];
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGuiMarkdown::s_config.codeBlockCornerRadius);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertFloat4ToU32(bgColor));

            ImGui::BeginChild(("##code_" + std::to_string(s_counter.code++)).c_str(), ImVec2(0.0f, 0.0f), 
                ImGuiChildFlags_Borders | 
                ImGuiChildFlags_AutoResizeY
                );
            s_codeTextBuffer = "";
        }
        else
        {
            ImGui::PushTextWrapPos(0.0f);
            ImGui::TextUnformatted(s_codeTextBuffer.c_str());
            ImGui::PopTextWrapPos();
            
            s_codeTextBuffer = "";
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::EndChild();

            s_isInCodeBlock = false;
        }
    }

    inline void BLOCK_P(bool enter)
    {
        if (enter)
        {
            s_SpanStack.push_back({});
        }
        else
        {
            RenderRichText();
            s_SpanStack.clear();
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
            s_SpanStack.clear();
            s_SpanStack.push_back({});
        }
        else
        {
            RenderRichText();
            s_SpanStack.clear();
        }
    }

    /* === Span definitions === */

    inline void SPAN_EM(bool enter)
    {
        if (enter)
        {
            s_italic = true;
            if (s_italic && s_bold)
                ImGui::PushFont(ImGuiMarkdown::GetFont(ImGuiMarkdown::FONT_BOLDITALIC));
            else
                ImGui::PushFont(ImGuiMarkdown::GetFont(ImGuiMarkdown::FONT_ITALIC));
        }
        else
        {
            s_italic = false;
            ImGui::PopFont();
        }
    }

    inline void SPAN_STRONG(bool enter)
    {
        if (enter)
        {
            s_bold = true;
            if (s_italic && s_bold)
                ImGui::PushFont(ImGuiMarkdown::GetFont(ImGuiMarkdown::FONT_BOLDITALIC));
            else
                ImGui::PushFont(ImGuiMarkdown::GetFont(ImGuiMarkdown::FONT_BOLD));
        }
        else
        {
            s_bold = false;
            ImGui::PopFont();
        }
    }

    inline void SPAN_CODE(bool enter)
    {
        if (enter)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        }
        else
        {
            ImGui::PopStyleColor();
            s_codeTextBuffer = "";
        }
    }
}

