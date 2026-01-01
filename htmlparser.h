#include "structs/content.h"

#include <string>
#include <vector>
#include <iostream>

#pragma once
class HtmlParser {

    public:
        Content parse(std::string &body);

    protected:

    private:
        std::string buffer = "";
        Content startingNode;
        std::vector<Content> unfinished;

        void addText();
        void addTag();

        Content finish();

        void printTree(Content &node, int indent);

};