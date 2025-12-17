#pragma once
#include "structs/content.h"
#include "structs/DisplayText.h"
#include "structs/FontCache.h"
#include "utils/utils.h"

#include <QString>
#include <QFontMetrics>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cctype>

class Layout {

public:
        Layout();
        void initialLayout(std::vector<Content> tokens, int page_width);
        std::vector<DisplayText> layout(int page_width);
        
        int getContentWidth() const { return content_width; }
        int getContentHeight() const { return content_height; }

        void setContentWidth(int content_width) {  this->content_width = content_width; }
        void setContentHeight(int content_height) {  this->content_height = content_height; }

        std::vector<DisplayText> getDisplayList() { return display_list; };

protected:     

private:

    bool inBody;

    int w;
    int h;

    int content_width = 0;
    int content_height = 0;

    static constexpr int HSTEP = 13;
    static constexpr int VSTEP = 24;

    int cursor_x = HSTEP;
    int cursor_y = VSTEP;
    int size = 16;

    QString qword;
    QFont font;
    QFontMetrics font_metrics;
    std::hash<std::string> hasher;
    std::size_t key;
    FontCache checkWord;
    std::unordered_map<std::size_t, FontCache> metricsCache;

    std::vector<DisplayText> layoutHelper();

    void addFontMetricsToCache(QString word);
    void tagHandler(Content tok);
    void textHandler(Content tok);

    std::vector<Content> tokens;
    int page_width;
    std::vector<DisplayText> display_list;


    void addToList(int x, int y, QString text);

    std::unordered_map<std::string, int> tags = {
        {"i", 0},
        {"/i", 1},
        {"b", 2},
        {"/b", 3},
        {"small", 4},
        {"/small", 5},
        {"big", 6},
        {"/big", 7},
        {"body", 8},
        {"/body", 9},
        {"p", 10},
        {"/p", 11},
        {"h1", 12},
        {"/h1", 13},
        {"h2", 14},
        {"/h2", 15},
        {"h3", 16},
        {"/h3", 17},
        {"h4", 18},
        {"/h4", 19},
        {"li", 20},
        {"/li", 21}
    };

};
