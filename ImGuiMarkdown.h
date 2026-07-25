#pragma once

#include <md4c.h>
#include <imgui.h>

#include <cstddef>

#include "Md4cCallbacks.h"

// #define DEBUG

#ifdef DEBUG
#include <vector>
#include <string>
#endif

struct MarkdownConfig
{
    float indentSize = 20.0f;
    float quoteRectThickness = 4.0f;
    float codeBlockCornerRadius = 5.0f;
    unsigned int parserFlags = MD_FLAG_TABLES;
};


class ImGuiMarkdown
{
public:
    typedef enum Fonts {
        FONT_REGULAR = 0,
        FONT_ITALIC,
        FONT_BOLD,
        FONT_BOLDITALIC,
        FONT_H1,
        FONT_H2,
        FONT_H3,
        FONT_H4,
        FONT_H5,
        FONT_H6
    } Fonts;

    ImGuiMarkdown();
    ~ImGuiMarkdown() = default;

    void Parse(const char* text, const size_t size);
    void SetFonts(ImFont* regular, ImFont* italic, ImFont* bold, ImFont* boldItalic, 
                  ImFont* H1, ImFont* H2, ImFont* H3,
                  ImFont* H4 = nullptr, ImFont* H5 = nullptr, ImFont* H6 = nullptr) 
                  { s_Fonts[FONT_REGULAR] = regular; s_Fonts[FONT_ITALIC] = italic; 
                    s_Fonts[FONT_BOLD] = bold; s_Fonts[FONT_BOLDITALIC] = boldItalic;
                    s_Fonts[FONT_H1] = H1; s_Fonts[FONT_H2] = H2; s_Fonts[FONT_H3]= H3; 
                    s_Fonts[FONT_H4] = H4; s_Fonts[FONT_H5] = H5; s_Fonts[FONT_H6]= H6; }

    static ImFont* GetFont(unsigned header);

    static inline MarkdownConfig s_config {};

private:
    void renderText(const char* text, const std::size_t size);
    void renderCode(const char* text, const std::size_t size);

    static int EnterBlockCallback(MD_BLOCKTYPE type, void* d, void* userdata) 
    {
        auto* renderer = static_cast<ImGuiMarkdown*>(userdata);
        return renderer->Block(type, d, true); 
    }
    static int LeaveBlockCallback(MD_BLOCKTYPE type, void* d, void* userdata) 
    {
        auto* renderer = static_cast<ImGuiMarkdown*>(userdata);
        return renderer->Block(type, d, false); 
    }
    int Block(MD_BLOCKTYPE type, void* detail, bool enter);


    static int EnterSpanCallback(MD_SPANTYPE type, void* d, void* userdata) 
    {
        auto* renderer = static_cast<ImGuiMarkdown*>(userdata);
        return renderer->Span(type, d, true); 
    }
    static int LeaveSpanCallback(MD_SPANTYPE type, void* d, void* userdata) 
    {
        auto* renderer = static_cast<ImGuiMarkdown*>(userdata);
        return renderer->Span(type, d, false); 
    }
    int Span(MD_SPANTYPE type, void* detail, bool enter);


    static int TextCallback(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata) 
    {
        auto* renderer = static_cast<ImGuiMarkdown*>(userdata);
        return renderer->Text(type, text, size); 
    }
    int Text(MD_TEXTTYPE t, const MD_CHAR* text, MD_SIZE size);


    MD_PARSER m_Parser {};
    MD4CCallbacks m_md4cCallbacks {};

    static inline ImFont* s_Fonts[10];

#ifdef DEBUG
    static inline std::vector<std::string> block_debug = {
    "MD_BLOCK_DOC",
    "MD_BLOCK_QUOTE",
    "MD_BLOCK_UL",
    "MD_BLOCK_OL",
    "MD_BLOCK_LI",
    "MD_BLOCK_HR",
    "MD_BLOCK_H",
    "MD_BLOCK_CODE",
    "MD_BLOCK_HTML",
    "MD_BLOCK_P",
    "MD_BLOCK_TABLE",
    "MD_BLOCK_THEAD",
    "MD_BLOCK_TBODY",
    "MD_BLOCK_TR",
    "MD_BLOCK_TH",
    "MD_BLOCK_TD",
    "MD_BLOCK_FOOTNOTE_DEF_SECTION",
    "MD_BLOCK_FOOTNOTE_DEF",
    "MD_BLOCK_ADMONITION"
    };

    static inline std::vector<std::string> span_debug = {
    "MD_SPAN_EM",
    "MD_SPAN_STRONG",
    "MD_SPAN_A",
    "MD_SPAN_IMG",
    "MD_SPAN_CODE",
    "MD_SPAN_DEL",
    "MD_SPAN_LATEXMATH",
    "MD_SPAN_LATEXMATH_DISPLAY",
    "MD_SPAN_WIKILINK",
    "MD_SPAN_U",
    "MD_SPAN_SPOILER",
    "MD_SPAN_SUPERSCRIPT",
    "MD_SPAN_SUBSCRIPT",
    "MD_SPAN_FOOTNOTE_REF",
    "MD_SPAN_MARK"
    };

    static inline std::vector<std::string> text_debug = {
    "MD_TEXT_NORMAL",
    "MD_TEXT_NULLCHAR",
    "MD_TEXT_BR",
    "MD_TEXT_SOFTBR",
    "MD_TEXT_ENTITY",
    "MD_TEXT_CODE",
    "MD_TEXT_HTML",
    "MD_TEXT_LATEXMATH"
    };
#endif
};
