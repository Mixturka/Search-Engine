#include <iostream>
#include "Crawler/Crawler.hpp"
#include "ArgParser/ArgParser.hpp"

int main(int argc, char** argv) {
  ArgParser parser;
  parser.Parse(argc, argv);

  DataBase base;
  std::string path = parser.GetPathToBase();
  base.Create(path);
  Crawler crawler(base);
  crawler.Crawl(parser.GetPathToSource());
}