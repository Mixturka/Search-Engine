#include "ArgParser.hpp"

void ArgParser::Parse(int& argc, char** argv) {
  if (argc != 2) {
    throw std::runtime_error("You should provide K as an argument - number of most relevant files you want to see");
  }

  this->k_ = std::stoi(argv[1]);
}

uint64_t& ArgParser::GetK() {
  return this->k_;
}