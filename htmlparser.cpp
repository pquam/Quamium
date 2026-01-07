#include "htmlparser.h"
#include "structs/content.h"
#include <iostream>
#include <vector>


Content* HtmlParser::parse(std::string &body) {

    buffer.clear();

    bool inTag = false;
    for (char &c : body) {
        if (c == '<') {
            addText();
            inTag = true;
        } else if (c == '>') {
            inTag = false;
            addTag();
        } else {
            buffer += c;
        }
    }

    if (!inTag && !buffer.empty()) {
        addText();
    }


    return finish();

}

void HtmlParser::addText() {

    Content node;

    node.parent = &unfinished.back();
    node.isTag = false;
    node.text = buffer;
    node.parent->children.push_back(&node);
}

void HtmlParser::addTag() {

    if (buffer.empty()) return;

    Content node;

    if (buffer[0] == '/') {
        node = unfinished.back();
        unfinished.pop_back();

        node.parent = &unfinished.back();
        node.parent->children.push_back(&node);
    }
    else if (unfinished.size() == 0) {

        startingNode.isTag = true;
        startingNode.text = buffer;
        unfinished.push_back(startingNode);
    }
    else {
        node.parent = &unfinished.back();
        node.isTag = true;
        node.text = buffer;
        unfinished.push_back(node);
    }

    buffer.clear();
}

Content* HtmlParser::finish() {

    Content node;

    while (unfinished.size() > 1) {

        node = unfinished.back();
        unfinished.pop_back();

        node.parent = &unfinished.back();

        node.parent->children.push_back(&node);
    }

    node = unfinished.back();
    unfinished.pop_back();
    return &node;
}

void printTree(Content &node, int indent) {

    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }

    if (node.isTag) {
        std::cout << "<";
    }

    std::cout << node.text;
    
    if (node.isTag) {
        std::cout << ">";
    }

    std::cout<<std::endl;
    

    for (int i = 0; i < node.children.size(); i++) {
        printTree(*node.children[i], indent + 2);
    }
}