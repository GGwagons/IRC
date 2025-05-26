#include <string>

#include "utils.hpp"

bool is_trailing(const std::string &str) {
    if (str.empty()) return false;

    if (str[0] != ':') return false;

    return true;
}

std::string get_trailing_value(const std::string &str) {
    if (!is_trailing(str)) return str;

    return str.substr(1);
}
