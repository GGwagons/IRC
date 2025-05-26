#include <vector>
#include "utils.hpp"

std::vector<std::string> split(const std::string &str, const char delimiter)
{
  std::vector<std::string> tokens;

  size_t token_start = 0;
  size_t token_end = str.find(delimiter);
  while (token_end != std::string::npos)
  {
    tokens.push_back(str.substr(token_start, token_end - token_start));
    token_start = token_end + 1;
    token_end = str.find(delimiter, token_start);
  }

  tokens.push_back(str.substr(token_start));

  return tokens;
}
