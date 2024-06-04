#include "ArgParser.hpp"

void ArgParser::Parse(int& argc, char** argv) {
  if (argc != 3) {
    throw std::runtime_error("You should provide 2 args:\n1) Path to data base directory(Should be empty)\n2)Path to directory where you would like to make search");
  }

  for (int i = 0; i < argc; ++i) {
    this->path_to_base_ = argv[1];
    this->path_to_source_ = argv[2];
  }
}

std::string& ArgParser::GetPathToBase() {
  return this->path_to_base_;
}

std::string ArgParser::GetPathToSource() {
  return this->path_to_source_;
}