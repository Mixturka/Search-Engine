#pragma once
#include <iostream>
#include <list>
#include <fstream>

class Tokenizer {
 public:
  Tokenizer() = default;

  std::list<std::string> TokenizeFile(const std::string&& path_to_file);
};