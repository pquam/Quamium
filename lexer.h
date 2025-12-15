#include "structs/content.h"

#include <string>
#include <vector>
#include <iostream>

class Lexer {

    public:
        std::vector<Content> lex(std::string body);

    protected:

    private:
        std::string buffer = "";
        std::vector<Content> out;

};