#include "structs/content.h"

#include <string>
#include <vector>
#include <iostream>

#pragma once
class Lexer {

    public:
        std::vector<Content> lex(std::string &body, std::vector<Content> &out);

    protected:

    private:
        std::string buffer = "";

};