#pragma once
#include <iostream>

class ArgParser {
 public:
  ArgParser() = default;

  void Parse(int& argc, char** argv);

  std::string& GetPathToBase();

  std::string GetPathToSource();
 private:
  std::string path_to_base_;
  std::string path_to_source_;
};