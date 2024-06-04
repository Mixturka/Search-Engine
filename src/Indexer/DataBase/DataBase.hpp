#pragma once
#include <iostream>
#include <memory>
#include "BTree/BTree.hpp"
#include <filesystem>
#include <unordered_map>

class DataBase {
 public:
  enum class BlockType {
    POSTING_LIST,
    DID_LIST,
  };

  DataBase() {
    this->posting_lists_ = std::make_unique<BTree>(3);
    this->did_list_ = std::make_unique<BTree>(3);
  }

  DataBase(const DataBase& other);

  void Create(std::string path);

  void Insert(std::string key, uint64_t value, uint64_t line, uint64_t freq, uint64_t size);

  void Insert(std::string key, std::string value);

  void Insert(std::string key, std::vector<Data>& value);

  void Load(std::string path);

  

 public:
  std::unique_ptr<BTree> posting_lists_;
  std::unique_ptr<BTree> did_list_;
  uint64_t doc_num_ = 0;
  uint64_t dl_sum_ = 0;
  uint64_t dl_avg_;
  std::string posting_lists_path_;
  std::string did_list_path_;
};