#pragma once

#include <imgui.h>
#include <md4c.h>

#include <string>
#include <vector>

class MD4CCallbacks
{
private:
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

    struct Span
    {
        ImU32 color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Text]);
        ImFont* font = ImGui::GetFont();
        float fontSize = ImGui::GetFontSize();
        // ?? Is std::string good for string buffer ??
        std::string buffer = "";
    };

public:
    MD4CCallbacks() = default;
    ~MD4CCallbacks() = default;

    TableState m_tableState {};
    bool m_isInCodeBlock = false;

    Counter m_counter {};
    std::string m_codeTextBuffer = "";

    unsigned int m_quoteDepth = 0;
    std::vector<Quote> m_quoteStack {};

    int m_listDepth = 0;
    std::vector<List> m_listStack {};

    bool m_italic = false;
    bool m_bold = false;

    std::vector<Span> m_spanStack {};

    void RenderRichText();
    void DrawQuote(ImVec2 startPos, ImVec2 endPos);

    void Reset() 
    {
        m_tableState = {};
        m_counter = {};
    }

    /* === Block definitions === */
    void BLOCK_DEFAULT(bool enter);
    void BLOCK_QUOTE(bool enter);
    void BLOCK_UL(MD_BLOCK_UL_DETAIL detail, bool enter);
    void BLOCK_OL(MD_BLOCK_OL_DETAIL detail, bool enter);
    void BLOCK_LI(MD_BLOCK_LI_DETAIL, bool enter);
    void BLOCK_HR(bool enter);
    void BLOCK_H(MD_BLOCK_H_DETAIL detail, bool enter);
    void BLOCK_CODE(const MD_BLOCK_CODE_DETAIL, bool enter);
    void BLOCK_P(bool enter);
    void BLOCK_TABLE(const MD_BLOCK_TABLE_DETAIL detail, bool enter);
    void BLOCK_THEAD(bool enter);
    void BLOCK_TR(bool enter);
    void BLOCK_TD(const MD_BLOCK_TD_DETAIL, bool enter);

    /* === Span definitions === */

    void SPAN_EM(bool enter);
    void SPAN_STRONG(bool enter);
    void SPAN_CODE(bool enter);
};
