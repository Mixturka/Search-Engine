#include "Crawler.hpp"

void Crawler::Crawl(const std::string&& path_to_dir) {
  for (const auto& entry : fs::recursive_directory_iterator(path_to_dir)) {
    if (entry.is_regular_file()) {
      fs::path file_path = entry.path();
      TokenizeFile(file_path);
    }
  }
}

void Crawler::TokenizeFile(const std::string&& path_to_file) {
  std::ifstream file(path_to_file, std::ios::binary | std::ios::in);
  
  std::set<std::string> test;

  ++cur_doc_id_;
  index_.Insert(std::to_string(cur_doc_id_), path_to_file);
  uint64_t line = 1;
  uint64_t prev_line = line;
  uint64_t doc_size;


  file.seekg(0, std::ios::end);
  doc_size = this->CountWordsInFile(path_to_file);
  file.seekg(0);
  this->index_.doc_num_++;
  this->index_.dl_sum_ += doc_size;
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + path_to_file);
  }

  std::string cur_token;
  char ch;

  while (file.get(ch)) {
    if (ch == '\n') {
      if (!cur_token.empty()) {
        if (cur_token == "for") std::cout << line << ' ';
        uint64_t freq = this->CountSpecificWordInFile(path_to_file, cur_token);
        index_.Insert(cur_token, cur_doc_id_, line, freq, doc_size);
        test.insert(cur_token);
        cur_token.clear();
      }
      prev_line = line;
      ++line;
      continue;
    }

    if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-') {
      cur_token += std::tolower(static_cast<unsigned char>(ch));
    } else if (!cur_token.empty()) {
              if (cur_token == "for") std::cout << line << ' ';
      uint64_t freq = this->CountSpecificWordInFile(path_to_file, cur_token);
      test.insert(cur_token);
      index_.Insert(cur_token, cur_doc_id_, line, freq, doc_size);
      cur_token.clear();
    }
  }

  if (!cur_token.empty()) {
            if (cur_token == "for") std::cout << line << ' ';
    test.insert(cur_token);
    uint64_t freq = this->CountSpecificWordInFile(path_to_file, cur_token);
    index_.Insert(cur_token, cur_doc_id_, freq, line, doc_size);
  }

  std::vector<Data> res;

  for (const std::string& str : test) {
    if (str == "for") {
      res = index_.posting_lists_->Search(str, BTree::DATA{});

      std::cout << str + ": " << '\n';
      for (auto v : res) {
        std::cout << "DID " << v.DID << '\n';
        std::cout << "LINES ";
        for (auto r : v.lines) {
          std::cout << r << ' ';
        }
        std::cout << '\n';
        std::cout << v.size << '\n'; 
      }
    }
  }
  
  file.close();

  std::ofstream meta("/Users/deniv/Documents/labwork11-Mixturka/Base/meta.dat", std::ios::out | std::ios::binary);
  meta.write(reinterpret_cast<char*>(&this->index_.dl_sum_), sizeof(uint64_t));
  meta.write(reinterpret_cast<char*>(&this->index_.doc_num_), sizeof(uint64_t));
  meta.close();
}

uint64_t Crawler::CountWordsInFile(const std::string& path_to_file) {
  std::ifstream file(path_to_file, std::ios::binary | std::ios::in);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + path_to_file);
  }

  int word_count = 0;
  std::string token;
  char ch;

  while (file.get(ch)) {
    if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-') {
      token += std::tolower(static_cast<unsigned char>(ch));
    } else if (!token.empty()) {
      ++word_count;
      token.clear();
    }
  }

  if (!token.empty()) {
    ++word_count;
  }

  file.close();

  return word_count;
}

uint64_t Crawler::CountSpecificWordInFile(const std::string& path_to_file, const std::string& word) {
  std::ifstream file(path_to_file, std::ios::binary | std::ios::in);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + path_to_file);
  }

  int specific_word_count = 0;
  std::string token;
  char ch;
  std::string lower_word = word;
  std::transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);

  while (file.get(ch)) {
    if (std::isalpha(static_cast<unsigned char>(ch)) || ch == '_' || ch == '-') {
      token += std::tolower(static_cast<unsigned char>(ch));
    } else if (!token.empty()) {
      if (token == lower_word) {
        ++specific_word_count;
      }
      token.clear();
    }
  }

  if (!token.empty() && token == lower_word) {
    ++specific_word_count;
  }

  file.close();

  return specific_word_count;
}