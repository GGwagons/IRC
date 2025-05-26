#pragma once

#ifndef UTILS_HPP_
#    define UTILS_HPP_

#    include <string>
#    include <vector>

std::vector<std::string> split(const std::string &str, const char delimiter);

bool        is_trailing(const std::string &str);
std::string get_trailing_value(const std::string &str);

int parse_port(const std::string &raw);

#endif
