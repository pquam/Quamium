#include "layout.h"
#include <iostream>

std::vector<DisplayText> Layout::layout(std::vector<Content> tokens, int page_width)
{

    bool inBody = false;

    int HSTEP, VSTEP;
    HSTEP = 13;
    VSTEP = 18;

    int w;

    int cursor_x = 30;
    int cursor_y = 30;
    int size = 16;

    QString qword;
    QFont font;

    for (Content tok : tokens) {

        if (tok.isTag) {
            std::cout << "tag: " + tok.text + "\n";
            
            auto tagIt = tags.find(tok.text);
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
                        std::cout << "Entering body! \n";
                        break;
                    case 10:
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                    case 12:
                        size += 4;
                        break;
                    case 13:
                        size -= 4;
                        cursor_y += VSTEP;
                        cursor_x = 30;
                        break;
                }
        
        font.setPixelSize(size);
            }

        }
        if (!tok.isTag && inBody) {
            std::cout << "text: " + tok.text + "\n";
            for (std::string word : Utils::split(tok.text, ' ')) {
                
                qword = QString::fromStdString(word);

                QFontMetrics metrics(font);
                w = metrics.horizontalAdvance(qword);

                if (cursor_x + HSTEP > page_width - HSTEP) {
                    cursor_y += metrics.height() * 1.25;
                    cursor_x = 30;
                }

                addToList(cursor_x, cursor_y, qword, font);
                cursor_x +=  w + metrics.horizontalAdvance(" "); 

            }
        }

    }

    return display_list;
}

void Layout::addToList(int x, int y, QString word, QFont font) {

    DisplayText text;


    text.x = x;
    text.y = y;
    text.text = word;
    text.font = font;

    display_list.push_back(std::move(text));
}
