
#include "structs/content.h"
#include "structs/DisplayText.h"
#include "utils/utils.h"

#include <QString>
#include <QFontMetrics>
#include <vector>
#include <unordered_map>

class Layout {

    public:
        std::vector<DisplayText> layout(std::vector<Content> tokens, int page_width);

    protected:
     

    private:
  
    std::vector<DisplayText> display_list;

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
        {"/h1", 13}
    };

};
