#pragma once
#include <iostream>

class ArgParser {
 public:
  ArgParser() = default;

  void Parse(int& argc, char** argv);

  uint64_t& GetK();
 private:
  uint64_t k_;
};