#include "nodelayout.h"
#include "structs/DisplayText.h"
#include "structs/content.h"
#include <iostream>

NodeLayout::NodeLayout()
        //initialize font metrics
        : font_metrics(font), line_metrics(font)
{}


void NodeLayout::initialLayout(Content* root_node, int page_width)
{
    this->root_node = root_node;

    this->page_width = page_width;

    //std::cout << "Initial Layout! \n";
    
    layoutHelper(root_node);
}

std::vector<DisplayText> NodeLayout::layout(int page_width)
{
    this->page_width = page_width;

    layoutReset();

    return layoutHelper(root_node);
}

void NodeLayout::layoutReset() {
    
    cursor_x = HSTEP;
    cursor_y = VSTEP;
    size = 16;

    font.setBold(false);
    font.setItalic(false);
    font.setPixelSize(size);

    content_height = 0;
    content_width = 0;
}

std::vector<DisplayText> NodeLayout::layoutHelper(Content* root_node) {

    display_list.clear();

    inBody = false;

    content_height = 0;
    content_width = 0;

    //std::cout << "Layout helper! \n";

    recurse(root_node);

    return this->display_list;
}

void NodeLayout::recurse(Content* node) {

    //std::cout << "node text: " + node->text + "\n";

    //if the node is a tag
    if (node->isTag) {
        tagHandler(*node);
    }

    //if the node is text
    if (!node->isTag) {
        textHandler(*node);
    }

    if (!node->children.empty()) {
        for (Content* child : node->children) {
            recurse(child);
        }
    }
    
}

void NodeLayout::tagHandler(Content tok) {

    std::string tagName = tok.text;
    auto spacePos = tagName.find(' ');
    if (spacePos != std::string::npos) {
        tagName = tagName.substr(0, spacePos);
    }
    std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);

    auto tagIt = tags.find(tagName);
    if (tagIt != tags.end()) {

        switch (tagIt->second) {

            case 0:
                font.setItalic(true);
                break;
            case 1:
                font.setItalic(false);
                break;
            case 2:
                font.setBold(true);
                break;
            case 3:
                font.setBold(false);
                break;
            case 4:
                size -= 2;
                break;
            case 5:
                size += 2;
                break;
            case 6:
                size += 2;
                break;
            case 7:
                size -= 2;
                break;
            case 8:
                inBody = true;
                break;
            case 9:
                inBody = false;
                break;
            case 10:
                cursor_y += VSTEP;
                cursor_x = 30;
                break;
            case 11:
                addLineToList();
                break;
            //h1
            case 12:
                size += 6;
                cursor_y += VSTEP*1.2;
                font.setBold(true);
                break;
            case 13:
                size = 16;
                font.setBold(false);
                addLineToList();
                break;
            case 14:
                size += 4;
                cursor_y += VSTEP*1.2;
                font.setBold(true);
                break;
            case 15:
                size = 16;
                font.setBold(false);
                addLineToList();
                break;
            case 16:
                size += 3;
                cursor_y += VSTEP*1.2;
                font.setBold(true);
                break;
            case 17:
                size = 16;
                font.setBold(false);
                addLineToList();
                break;
            case 18:
                size += 2;
                cursor_y += VSTEP*1.2;
                font.setBold(true);
                break;
            case 19:
                size = 16;
                font.setBold(false);
                addLineToList();
                break;
            case 20:
                size = 16;
                font.setBold(false);
                break;
            case 21:
                size = 16;
                font.setBold(false);
                addLineToList();
                break;
            case 22:
                HSTEP += 13;
                break;
            case 23:
                HSTEP -= 13;
                addLineToList();
                break;
            case 24:
                addLineToList();
                break;
            case 25:
                addLineToList();
                break;
        }

        font.setPixelSize(size);
    }
}

void NodeLayout::textHandler(Content tok) {

    font_metrics = QFontMetrics(font);

    //std::cout << "tok.text: " + tok.text + "\n";
    for (std::string word : Utils::split(tok.text, ' ')) {

        if (word.empty()) {
            continue;
        }
        
        wordHandler(word);
    }
}

void NodeLayout::wordHandler(std::string word) {

    //std::cout << " word: " + word;

    qword = QString::fromStdString(word);

    key = hasher(word + "/&/" + font.toString().toStdString());

    bool wordfound = false;

    //check if word is in cache array with matching font
    auto i = metricsCache.find(key);

    if (i != metricsCache.end()) {

        checkWord = i->second;

        //if yes, get word advance and height, get the data required
        if (qword == checkWord.word && font == checkWord.font) {

            w = checkWord.advance;
            h = checkWord.height;
            wordfound = true;
        }
    }

    if (wordfound == false) {
        //if no, add it to the list, then continue.

        addFontMetricsToCache(qword);
    }



    if (cursor_x + w > page_width - HSTEP) {
        addLineToList();
    }


    addToLine(cursor_x, qword);
    cursor_x +=  w + font_metrics.horizontalAdvance(" "); 

}

void NodeLayout::addToLine(int x, QString word) {

    DisplayText text;

    text.x = x;
    text.text = word;
    text.font = font;

    content_width = std::max(content_width, x + w);
    
    line.push_back(text);
}

void NodeLayout::addLineToList() {

    //std::cout << "adding line to display list\n";
    int max_ascent = 0;
    int max_desc = 0;

    for (DisplayText &text : line) {
        line_metrics = QFontMetrics(text.font);
        max_ascent = std::max(max_ascent, line_metrics.ascent());
        max_desc = std::max(max_desc, line_metrics.descent());
    }

    double  baseline = cursor_y - 1.25 * max_ascent;

    for (DisplayText &text : line) {
        line_metrics = QFontMetrics(text.font);
        text.y = int(baseline + line_metrics.ascent());
    }

    content_height = std::max(content_height, int(baseline) + h);

    cursor_y = int(baseline - 1.25 * max_desc);

    display_list.insert(display_list.end(), line.begin(), line.end());
    line.clear();
    newLine(1.25);
}

void NodeLayout::addFontMetricsToCache(QString word) {

    //std::cout << "Font Cache miss! adding: \"" + word.toStdString() + "\"\n";

    FontCache cache;
    key = hasher(word.toStdString() + "/&/" + font.toString().toStdString());

    cache.word = word;
    cache.font = font;

    w = font_metrics.horizontalAdvance(word);
    h = font_metrics.height();

    cache.advance = w;
    cache.height = h;

    metricsCache[key] = cache;

}

void NodeLayout::newLine(double lineSpacing) {
    cursor_y += VSTEP*(lineSpacing*lineSpacing);
    cursor_x = HSTEP;
}