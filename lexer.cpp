#include "lexer.h"
#include <iostream>

void Lexer::lex(std::string body) {

    for (char c : body) {
        if (c == '<') {
            content.isTag = true;
            if (!buffer.empty()) {
                content.text = buffer;
                out.push_back(content);
                buffer = "";
            }
        }
        else if (c == '>') {
            content.isTag = false;
            if (!buffer.empty()) {
                content.text = buffer;
                out.push_back(content);
                buffer = "";
            }
        }
        else {
            buffer += c;
        }
    }

    if (!buffer.empty() && !content.isTag) {
        content.text = buffer;
        out.push_back(content);
        buffer = "";
    }

    for (Content i : out) {
        std::cout << "text: " + i.text + "\n";
    }
    
}