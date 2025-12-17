#include "layout.h"

Layout::Layout()
        //initialize font metrics
        : font_metrics(font)
{}


void Layout::initialLayout(std::vector<Content> tokens, int page_width)
{

    this->tokens = tokens;
    this->page_width = page_width;
    
    layoutHelper();
}

std::vector<DisplayText> Layout::layout(int page_width)
{
    this->page_width = page_width;

    cursor_x = HSTEP;
    cursor_y = VSTEP;
    size = 16;

    font.setBold(false);
    font.setItalic(false);
    font.setPixelSize(size);

    content_height = 0;
    content_width = 0;

    return layoutHelper();
}

std::vector<DisplayText> Layout::layoutHelper() {

    display_list.clear();

    inBody = false;

    content_height = 0;
    content_width = 0;


    for (Content tok : tokens) {

        if (tok.isTag) {

            tagHandler(tok);
        }
        if (!tok.isTag && inBody) {
            
            textHandler(tok);
        }

    }

    return this->display_list;
}

void Layout::tagHandler(Content tok) {

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

void Layout::textHandler(Content tok) {

            font_metrics = QFontMetrics(font);

            

            //std::cout << "tok.text: " + tok.text + "\n";
            for (std::string word : Utils::split(tok.text, ' ')) {

                if (word.empty()) {
                    continue;
                }

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
                    cursor_y += h * 1.25;
                    cursor_x = HSTEP;
                }

                if (!(word.find('\n') == std::string::npos)) {
                    cursor_y += h * 1.25;
                    cursor_x = HSTEP;
                }

                if (qword != "" && qword != " ") {

                    addToList(cursor_x, cursor_y, qword);
                    cursor_x +=  w + font_metrics.horizontalAdvance(" "); 
                }
                
            }
}

void Layout::addToList(int x, int y, QString word) {

    DisplayText text;


    text.x = x;
    text.y = y;
    text.text = word;
    text.font = font;

    //std::cout << "adding \"" + word.toStdString() + "\" to display list!\n";

    content_width = std::max(content_width, x + w);
    content_height = std::max(content_height, y + h);

    display_list.push_back(std::move(text));
}

void Layout::addFontMetricsToCache(QString word) {

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