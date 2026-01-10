#include <string>
#include <vector>

#pragma once
struct Content {

    bool isTag = false;
    std::string text;
    std::vector<std::string> attributes;
    Content* parent = nullptr;
    std::vector<Content*> children;

    public:
        void reset() {
            isTag = false;
            text = "";
            parent = nullptr;
            children.clear();
        }
};