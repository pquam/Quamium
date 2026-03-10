#include "htmlparser.h"
#include "structs/content.h"
#include "utils/utils.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <stack>
#include<ranges>
#include <ostream>
#include <string>
#include <vector>


Content* HtmlParser::parse(std::string &body, std::vector<Content*> &htmlTreeHolder) {

    buffer.clear();
    root_node.reset();
    unfinished.clear();
    inHeader = false;
    
    if (!htmlTreeHolder.empty()) {

        for (Content* node : htmlTreeHolder) {

            delete node;
        }
        htmlTreeHolder.clear();
    }
    

    bool inTag = false;
    bool inText = false;
    std::stack<bool> tagStack;

    //for (char &c : body) {
    for (int i = 0; i < body.size(); i++) {
        char &c = body[i];

        if (c == '\n') {
            c = ' ';
        }

        if (c == '<') {

            if (!buffer.empty()) {

                Content* text = addText();
                if (text != nullptr) {

                    htmlTreeHolder.push_back(text);
                }

                inTag = true;
            }
            else if (inText) {

            }

        } else if (c == '>') {

            inTag = false;
            inText = true;

            if (!buffer.empty() && buffer[0] != '!') {

                Content* tagNode = addTag();

                if (tagNode != nullptr) {

                    htmlTreeHolder.push_back(tagNode);
                }
            }
            else {

            buffer.clear();
            }
        } else {

            if (c == '&') {
                if (body.substr(i+1,3) == "lt;") {
                    c = '<';
                    i+=3;
                }
                if (body.substr(i+1,3) == "gt;") {
                    c = '>';
                    i+=3;
                }
                if (body.substr(i+1,4) == "amp;") {
                    c = '&';
                    i+=4;
                }
                if (body.substr(i+1,4) == "#39;") {
                    c = '\'';
                    i+=4;
                }
                if (body.substr(i+1,5) == "quot;") {
                    c = '"';
                    i+=5;
                }
            }

            buffer += c;
        }
    }

    if (!inTag && !buffer.empty()) {

        Content* text = addText();

        if (text != nullptr) {

            htmlTreeHolder.push_back(text);
        }
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

    std::vector<std::string> splitTag = Utils::split(buffer, ' ');

    if (splitTag.empty() || splitTag[0].empty()) {

        buffer.clear();
        return nullptr;
    }

    std::string tag = splitTag[0];
    splitTag.erase(splitTag.begin());
    std::vector<std::string> attributes = splitTag;

    bool foundMatch = false;
    auto hasOpenTag = [this](const std::string& name) {
        return std::any_of(unfinished.begin(), unfinished.end(),
                           [&name](const Content* node) { return node->text == name; });
    };

    if (tag == "head") {
        inHeader = true;
    }
    if (tag == "/head") {
        inHeader = false;
    }

    if (!tag.empty() && tag[0] == '/') {

        const std::string closingTag = tag.substr(1);
        if (unfinished.size() > 1 && hasOpenTag(closingTag)) {

            finishSection(closingTag);
        }

        buffer.clear();

        return nullptr;
    }
    else {

        for (Content* match : unfinished | std::ranges::views::reverse) {

            if (match->text == tag) {

                foundMatch = true;
            }
        }
    }

    if (foundMatch) {
        finishSection(tag);
    }

    if (tag == "html") {

        root_node.isTag = true;
        root_node.text = tag;
        unfinished.push_back(&root_node);

        buffer.clear();

        return nullptr;
    }

    Content* node = new Content;
    node->isTag = true;

    if (std::find(selfClosingTags, selfClosingTags + 14, tag)
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

        finishSection("head");
        inHeader = false;

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

    Content* node = nullptr;

    while (unfinished.size() > 1) {

        node = unfinished.back();
        unfinished.pop_back();

        node->parent = unfinished.back();

        node->parent->children.push_back(node);
    }

    if (unfinished.empty()) {
        return nullptr;
    }

    node = unfinished.back();
    unfinished.pop_back();

    return node;
}

void HtmlParser::finishSection(const std::string& tag) {

    while (unfinished.size() > 1) {

        Content* node = unfinished.back();
        unfinished.pop_back();

        node->parent = unfinished.back();

        node->parent->children.push_back(node);

        if (node->text == tag) {
            break;
        }
    }

    buffer.clear();
}

void HtmlParser::printTree(Content* node, int indent) {

    if (node == nullptr) {
        return;
    }

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
