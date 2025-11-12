#include <string>
#include <vector>
#include "structs/content.h"

class Lexer {

    public:
        void lex(std::string body);

    protected:

    private:
        bool inTag = false;
        std::string buffer = "";
        Content content;
        std::vector<Content> out;

};