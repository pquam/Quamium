#include "htmlparser.h"
#include "structs/content.h"
#include "utils/utils.h"
#include <iostream>
#include <memory>
#include <vector>


Content* HtmlParser::parse(std::string &body, std::vector<Content*> &htmlTreeHolder) {

    buffer.clear();
    /*
    if (!htmlTreeHolder.empty()) {
        for (Content* node : htmlTreeHolder) {
            delete node;
        }
        htmlTreeHolder.clear();
    }
    root_node = nullptr;
    */
    

    bool inTag = false;
    for (char &c : body) {
        if (c == '<') {
            htmlTreeHolder.push_back(addText());
            inTag = true;
        } else if (c == '>') {
            inTag = false;
            htmlTreeHolder.push_back(addTag());
        } else {
            buffer += c;
        }
    }

    if (!inTag && !buffer.empty()) {
        htmlTreeHolder.push_back(addText());
    }

    return finish();

}

Content* HtmlParser::addText() {

    Content* node = new Content;

    node->parent = unfinished.back();
    node->isTag = false;
    node->text = buffer;
    node->parent->children.push_back(node);

    buffer.clear();

    return node;
}

Content* HtmlParser::addTag() {

    Content* node = new Content;

    if (buffer[0] == '/') {
        node = unfinished.back();
        unfinished.pop_back();

        node->parent = unfinished.back();
        node->parent->children.push_back(node);
    }
    else if (unfinished.size() == 0) {

        root_node.isTag = true;
        root_node.text = buffer;
        unfinished.push_back(&root_node);
    }/*
    else if (!unfinished.empty() && selfClosingTags->find(Utils::split(buffer, ' ')[0])) {
        node->parent = unfinished.back();
        node->isTag = true;
        node->parent->children.push_back(node);
    }*/
    else {
        node->parent = unfinished.empty() ? nullptr : unfinished.back();
        node->isTag = true;
        node->text = buffer;
        unfinished.push_back(node);
    }

    buffer.clear();

    return node;
}

Content* HtmlParser::finish() {

    std::cout << "finished! \n";

    Content* node = new Content;

    while (unfinished.size() > 1) {

        node = unfinished.back();
        unfinished.pop_back();

        node->parent = unfinished.back();

        node->parent->children.push_back(node);
    }

    node = unfinished.back();
    unfinished.pop_back();

    return node;
}

void HtmlParser::printTree(Content* node, int indent) {

    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }

    if (node->isTag) {
        std::cout << "<";
    }

    std::cout << node->text;
    
    if (node->isTag) {
        std::cout << ">";
    }

    std::cout<<std::endl;
    

    for (int i = 0; i < node->children.size(); i++) {
        printTree(node->children[i], indent + 2);
    }
}
