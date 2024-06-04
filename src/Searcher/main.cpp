#include <iostream>
#include <sstream>
#include "RequestParser/Parser.hpp"
#include "RequestParser/Scanner.hpp"
#include "ArgParser/ArgParser.hpp"

int main(int argc, char** argv) {
  ArgParser arg_parser;
  // argc = 2;
  // argv = new char*("3");
  arg_parser.Parse(argc, argv);
  std::string expr;

  while (expr != "EXIT") {
    std::getline(std::cin, expr);
    Scanner scanner(expr);
    Parser parser(scanner.ScanTokens());  
    Interpreter iter(arg_parser.GetK());

    iter.Init("/Users/deniv/Documents/labwork11-Mixturka/Base/");

    iter.CalculateScores(scanner.ScanTokens());

    iter.Interpret(parser.ParseExpression());
    iter.OutputList();
  }
}