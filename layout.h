#pragma once
#include "structs/content.h"
#include "structs/DisplayText.h"
#include "utils/utils.h"

#include <QString>
#include <QFontMetrics>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

class Layout {

public:
        std::vector<DisplayText> layout(std::vector<Content> tokens, int page_width);
        std::vector<DisplayText> layout(int page_width);
        
        int getContentWidth() const { return content_width; }
        int getContentHeight() const { return content_height; }

    protected:     

    private:
    std::vector<DisplayText> layoutHelper();

    std::vector<Content> tokens;
    int page_width;
    std::vector<DisplayText> display_list;
    int content_width = 0;
    int content_height = 0;

    void addToList(int x, int y, QString text, QFont font);

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
