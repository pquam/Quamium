#include "utils.h"

std::vector<std::string> Utils::split(const std::string &s, char delim) {
    std::vector<std::string> parts;
    std::string current;

    for (char ch : s) {
        if (ch == delim) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }

    parts.push_back(current);
    return parts;
}
