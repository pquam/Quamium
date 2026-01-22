#include "htmlparser.h"
#include "structs/content.h"
#include "utils/utils.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <memory>
#include<ranges>
#include <ostream>
#include <string>
#include <vector>


Content* HtmlParser::parse(std::string &body, std::vector<Content*> &htmlTreeHolder) {

    buffer.clear();
    root_node.reset();
    unfinished.clear();
    
    if (!htmlTreeHolder.empty()) {
        for (Content* node : htmlTreeHolder) {
            delete node;
        }
        //htmlTreeHolder.clear();
    }
    

    bool inTag = false;
    for (char &c : body) {
        if (c == '<') {
            htmlTreeHolder.push_back(addText());
            inTag = true;
        } else if (c == '>') {
            inTag = false;
            if (buffer[0] != '!') {
                htmlTreeHolder.push_back(addTag());
            }
            else {
            buffer.clear();
            }
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

    node->parent = unfinished.empty() ? nullptr : unfinished.back();
    node->isTag = false;
    node->text = buffer;
    if (node->parent != nullptr) {
        node->parent->children.push_back(node);
    }
    

    buffer.clear();

    return node;
}

Content* HtmlParser::addTag() {

    Content* node = new Content;
    node->isTag = true;

    Content* closenode = new Content;

    std::vector<std::string> splitTag = Utils::split(buffer, ' ');
    std::string tag = splitTag[0];
    splitTag.erase(splitTag.begin());
    std::vector<std::string> attributes = splitTag;

    

    bool foundMatch = false;

    if (tag == "head") {
        inHeader = true;
    }
    if (tag == "/head") {
        inHeader = false;
    }

    if (tag == "body") {
        inBody = true;
    }
    if (tag == "/body") {
        inBody = false;
    }
    

    if (!tag.empty() && tag[0] == '/' && unfinished.size() > 1) {

        closenode->isTag = true;
        closenode->text = tag;

        return finishSection(tag, node, closenode);
    }
    else {
        for (Content* match : unfinished | std::ranges::views::reverse) {
            if (match->text == tag) {
                foundMatch = true;
                std::cout << "found match! \n" + tag;
            }
        }
    }

    if (foundMatch) {

        closenode->isTag = true;
        closenode->text = "/" + tag;
        finishSection(tag, node, closenode);
    }

    if (tag == "html") {

        root_node.isTag = true;
        root_node.text = tag;
        unfinished.push_back(&root_node);
    }
    else if (std::find(selfClosingTags, selfClosingTags + 14, tag)
    !=
    (selfClosingTags + 14)
    ||
    tag.back() == '/'
    ) {

        node->text = tag;
        node->parent = unfinished.empty() ? nullptr : unfinished.back();
        if (node->parent != nullptr) {
            node->parent->children.push_back(node);
        }
    }
    else if (tag == "body" && inHeader) {

        closenode->isTag = true;
        closenode->text = "/head";
        finishSection("/head", node, closenode);
        inHeader = false;

        node->text = tag;
        node->attributes = attributes;
        node->parent = unfinished.empty() ? nullptr : unfinished.back();
        unfinished.push_back(node);
    }
    else if (tag == "/html" && inBody) {
        
        closenode->isTag = true;
        closenode->text = "/body";
        finishSection("/body", node, closenode);
        inBody = false;

        node->text = tag;
        node->attributes = attributes;
        node->parent = unfinished.empty() ? nullptr : unfinished.back();
        unfinished.push_back(node);
    }
    else {
        node->text = tag;
        node->attributes = attributes;
        node->parent = unfinished.empty() ? nullptr : unfinished.back();
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

Content* HtmlParser::finishSection(std::string tag, Content* node, Content* closenode) {

    while (unfinished.size() > 1) {

        node = unfinished.back();
        unfinished.pop_back();

        node->parent = unfinished.back();

        node->parent->children.push_back(node);

        if ("/" + tag == closenode->text) {
                closenode->parent = node;
                break;
            }
    }

    node = unfinished.back();
    unfinished.pop_back();

    

    buffer.clear();
    return node;
}

void HtmlParser::printTree(Content* node, int indent) {

    for (int i = 0; i < indent; i++) {
        std::cout << " ";
    }

    if (node->isTag) {
        std::cout << "<" + node->text + ">";
    }

    
    if (!node->isTag) {
        std::cout << node->text;
    }

    std::cout<<std::endl;
    

    for (int i = 0; i < node->children.size(); i++) {
        printTree(node->children[i], indent + 2);
    }

    if (node->isTag &&
    std::find(selfClosingTags, selfClosingTags + 14, Utils::split(node->text, ' ')[0])
    ==
    (selfClosingTags + 14)
    ) {
        for (int i = 0; i < indent; i++) {
            std::cout << " ";
        }
        std::cout << "</" + Utils::split(node->text, ' ')[0] + ">" << std::endl;
    }
}
