#include "DataBase.hpp"
namespace fs = std::filesystem;

DataBase::DataBase(const DataBase& other) {
  this->posting_lists_ = std::make_unique<BTree>(std::move(*other.posting_lists_));
  this->did_list_ = std::make_unique<BTree>(std::move(*other.did_list_));
}

void DataBase::Create(std::string path) {

  if (!fs::is_directory(path)) throw std::runtime_error("Path to database should be a folder.");
  if (!fs::is_empty(path)) throw std::runtime_error("Folder where you want to store database should be empty.");

  std::ofstream file(path + '/' + "posting_list.dat", std::ios::out);
  file.close();
  file.open(path + '/' + "DIDs.dat", std::ios::out);
  file.close();

  this->posting_lists_path_ = fs::absolute(path + '/' + "posting_list.dat");
  this->did_list_path_ = fs::absolute(path + '/' + "DIDs.dat");
  this->posting_lists_->Create(posting_lists_path_);
  this->did_list_->Create(did_list_path_);
}

void DataBase::Load(std::string path) {
  if (!fs::is_directory(path)) throw std::runtime_error("Provide correct FOLDER path to load database");
  if (fs::is_empty(path)) throw std::runtime_error("Can't load database from empty folder.");
  this->posting_lists_->Load(path + "posting_list.dat");
  this->did_list_->Load(path + "DIDs.dat");
  std::ifstream meta("/Users/deniv/Documents/labwork11-Mixturka/Base/meta.dat", std::ios::in | std::ios::binary);
  meta.read(reinterpret_cast<char*>(&this->dl_sum_), sizeof(uint64_t));
  meta.read(reinterpret_cast<char*>(&this->doc_num_), sizeof(uint64_t));
}

void DataBase::Insert(std::string key, uint64_t value, uint64_t line, uint64_t freq, uint64_t size) {
  this->posting_lists_->Insert(key, value, line, freq, size);
}

void DataBase::Insert(std::string key, std::string value) {
  this->did_list_->Insert(key, value);
}


void DataBase::Insert(std::string key, std::vector<Data>& value) {
  for (Data& data : value) {
    for (auto& line : data.lines) {
      this->posting_lists_->Insert(key, data.DID, line, data.freq, data.size);
    }
  }
}
