#include "layout.h"


std::vector<DisplayText> Layout::layout(std::vector<Content> tokens, int page_width)
{

    this->tokens = tokens;
    this->page_width = page_width;    

    return layoutHelper();
}

std::vector<DisplayText> Layout::layout(int page_width)
{
    this->page_width = page_width;

    return layoutHelper();
}

std::vector<DisplayText> Layout::layoutHelper() {

    display_list.clear();

    bool inBody = false;

    int w;

    int cursor_x = HSTEP;
    int cursor_y = VSTEP;
    int size = 16;

    content_height = 0;
    content_width = 0;

    QString qword;
    QFont font;

    for (Content tok : tokens) {

        if (tok.isTag) {
            //std::cout << "tag: " + tok.text + "\n";

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
                        //size -= 2;
                        break;
                    case 5:
                        //size += 2;
                        break;
                    case 6:
                        //size += 2;
                        break;
                    case 7:
                        //size -= 2;
                        break;
                    case 8:
                        inBody = true;
                        break;
                    case 9:
                        cursor_x = 30;
                        inBody = false;
                        break;
                    case 10:
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                    case 11:
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                    //h1
                    case 12:
                        size = size *1.5;
                        cursor_y += VSTEP*1.2;
                        font.setBold(true);
                        break;
                    case 13:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP*1.2;
                        cursor_x = 30;
                        break;
                    case 14:
                        size = size *1.4;
                        cursor_y += VSTEP*1.2;
                        font.setBold(true);
                        break;
                    case 15:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP*1.2;
                        cursor_x = 30;
                        break;
                    case 16:
                        size = size *1.3;
                        cursor_y += VSTEP*1.2;
                        font.setBold(true);
                        break;
                    case 17:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP*1.2;
                        cursor_x = 30;
                        break;
                    case 18:
                        size = size *1.2;
                        cursor_y += VSTEP*1.2;
                        font.setBold(true);
                        break;
                    case 19:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP*1.2;
                        cursor_x = 30;
                        break;
                    case 20:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                    case 21:
                        size = 16;
                        font.setBold(false);
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                }
        
        font.setPixelSize(size);
            }

        }
        if (!tok.isTag && inBody) {
            //std::cout << "text: " + tok.text + "\n";
            for (std::string word : Utils::split(tok.text, ' ')) {

                qword = QString::fromStdString(word);

                QFontMetrics metrics(font);
                w = metrics.horizontalAdvance(qword);

                if (cursor_x + w > page_width - HSTEP) {
                    cursor_y += metrics.height() * 1.25;
                    cursor_x = HSTEP;
                }

                if (!(word.find('\n') == std::string::npos)) {
                    cursor_y += metrics.height() * 1.25;
                    cursor_x = HSTEP;
                }

                if (qword != "" && qword != " ") {

                    addToList(cursor_x, cursor_y, qword, font);
                    cursor_x +=  w + metrics.horizontalAdvance(" "); 
                }
                
            }
        }

    }

    return this->display_list;
}

void Layout::addToList(int x, int y, QString word, QFont font) {

    DisplayText text;


    text.x = x;
    text.y = y;
    text.text = word;
    text.font = font;

    QFontMetrics metrics(font);
    int width = metrics.horizontalAdvance(word);
    int height = metrics.height();

    content_width = std::max(content_width, x + width);
    content_height = std::max(content_height, y + height);

    display_list.push_back(std::move(text));
}
