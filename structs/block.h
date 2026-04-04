#include <string>
#include <vector>

#pragma once
struct Block {

    bool isInterior = false;
    std::string text;
    std::vector<std::string> attributes;
    Content* parent = nullptr;
    std::vector<Content*> children;

    public:
        void reset() {
            isInterior = false;
            text = "";
            parent = nullptr;
            children.clear();
        }
};