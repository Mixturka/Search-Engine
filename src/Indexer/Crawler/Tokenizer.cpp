#include "Tokenizer.hpp"

std::list<std::string> Tokenizer::TokenizeFile(const std::string&& path_to_file) {
  std::list<std::string> tokens;
  std::ifstream file(path_to_file, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + path_to_file);
  }

  std::string cur_token;
  char ch;

  while (file.get(ch)) {
    if (std::isalpha(static_cast<unsigned char>(ch))) {
      cur_token += std::tolower(static_cast<unsigned char>(ch));
    } else if (!cur_token.empty()) {
      tokens.push_back(cur_token);
      cur_token.clear();
    }
  }

  if (!cur_token.empty()) {
    tokens.push_back(cur_token);
  }

  file.close();

  return tokens;
}