#include "structs/content.h"

#include <string>
#include <vector>

#pragma once
class HtmlParser {

    public:
        Content* parse(std::string &body, std::vector<Content*> &htmlTreeHolder);
        Content root_node;

        void printTree(Content* node, int indent);

    protected:

    private:
        std::string buffer = "";
        std::vector<Content*> unfinished = {&root_node};

        std::string selfClosingTags[14] = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };

        Content* addText();
        Content* addTag();

        Content* finish();

};