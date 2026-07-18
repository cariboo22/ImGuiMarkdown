#pragma once

#include <md4c.h>
#include <imgui.h>

#include <cstddef>


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
};

class ImGuiMarkdown
{
public:
    ImGuiMarkdown();
    ~ImGuiMarkdown() = default;

    void Parse(const char* text, const size_t size);
    void SetFonts(ImFont* H1, ImFont* H2, ImFont* H3) { m_H[0] = H1; m_H[1] = H2; m_H[2]= H3; }
    static ImFont* GetFont(unsigned header);

    static inline MarkdownConfig s_config {};

private:

    static int EnterBlock(MD_BLOCKTYPE type, void* d, void*) { return Block(type, d, true); }
    static int LeaveBlock(MD_BLOCKTYPE type, void* d, void*) { return Block(type, d, false); }
    static int Block(MD_BLOCKTYPE type, void* detail, bool enter);

    static int EnterSpan(MD_SPANTYPE type, void* d, void*) { return Span(type, d, true); }
    static int LeaveSpan(MD_SPANTYPE type, void* d, void*) { return Span(type, d, false); }
    static int Span(MD_SPANTYPE type, void* detail, bool enter);

    static int Text(MD_TEXTTYPE t, const MD_CHAR* text, MD_SIZE size, void* u);

    MD_PARSER m_Parser {};

    static inline ImFont* m_H[3];

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
