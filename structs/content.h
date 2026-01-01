#include <string>
#include <vector>

#pragma once
struct Content {

    bool isTag = false;
    std::string text;
    Content* parent;
    std::vector<Content*> children;

};