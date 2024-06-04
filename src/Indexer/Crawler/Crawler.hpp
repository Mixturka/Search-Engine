#pragma once
#include <iostream>
#include <filesystem>
#include <set>
#include "../DataBase/DataBase.hpp"

namespace fs = std::filesystem;

class Crawler {
 public:
  Crawler(DataBase base) : index_(base) {}

  void Crawl(const std::string&& path_to_dir);


  void Check() {

  }

 private:
  void TokenizeFile(const std::string&& path_to_file);

  uint64_t CountWordsInFile(const std::string& path_to_file);

  uint64_t CountSpecificWordInFile(const std::string& path_to_file, const std::string& word);

  DataBase index_;
  uint64_t cur_doc_id_ = -1;
};