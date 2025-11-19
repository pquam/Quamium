#include "lexer.h"


std::vector<Content> Lexer::lex(std::string body) {

    auto emit = [&](bool isTag) {
        if (buffer.empty()) return;
        Content chunk;
        chunk.isTag = isTag;
        chunk.text = buffer;
        out.push_back(std::move(chunk));
        buffer.clear();
    };

    for (char c : body) {
        if (c == '<') {
            emit(false);
        } else if (c == '>') {
            emit(true);
        } else {
            buffer += c;
        }
    }

    emit(false);

    return out;

}