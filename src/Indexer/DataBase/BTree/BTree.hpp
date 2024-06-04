#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

struct Data {
  uint64_t DID;
  std::vector<uint64_t> lines;
  uint64_t freq;
  uint64_t size;

  friend bool operator<(const Data& first, const Data& second) {
    return first.freq < second.freq;
  }

  friend bool operator==(const Data& first, const Data& second) {
    return (first.DID == second.DID && first.freq == second.freq && first.size == second.size && first.lines == second.lines);
  }

  ~Data() = default;
};

class BTree {
 private:
  struct Node {
    bool is_leaf;
    uint64_t offset;
    std::vector<std::pair<std::string, uint64_t>> keys;
    std::vector<uint64_t> children;


  };

 public:
  struct DATA {};
  struct STRING {};

  BTree(uint64_t t) : t_(t) {}

  void Load(std::string name) {
    file_.open(name, std::ios::in | std::ios::out | std::ios::binary);
    file_name_ = name;
    root_ = ReadNode(0);
  }

  void Create(std::string name) {
    file_size_ = filesize(name);
    file_name_ = name;
    file_.open(name, std::ios::in | std::ios::out | std::ios::binary);
    root_ = std::make_unique<Node>();
    root_->is_leaf = true;
    root_->offset = 4096;
    root_->children = std::vector<uint64_t>();
    root_->keys = std::vector<std::pair<std::string, uint64_t>>();
    WriteNode(root_.get(), 0);
    WriteNode(root_.get(), 4096);
  }

  void Insert(std::string key, uint64_t value, uint64_t line, uint64_t freq, uint64_t size) {
    if (root_->keys.size() == 2 * t_ - 1) {
      SplitRoot();
    }
    InsertNotFull(root_.get(), key, value, line, freq, size);
  }

  void Insert(std::string key, std::string value) {
    if (root_->keys.size() == 2 * t_ - 1) {
        SplitRoot();
    }

    InsertNotFull(root_.get(), key, value);
  }

  std::vector<Data> Search(std::string key, DATA) {
    return Search(root_.get(), key, DATA{});
  }

  std::string Search(std::string key, STRING) {
    return Search(root_.get(), key, STRING{});
  }

 private:
  std::unique_ptr<Node> root_;
  std::string file_name_;
  uint64_t t_;
  std::fstream file_;
  uint64_t file_size_ = 0;
  char* buf = new char[4096]();
  char* buf_smally = new char[1024]();

  void ReadVector(uint64_t offset, std::vector<uint64_t>& res) {
    file_.seekg(offset);
    if (!file_.is_open()) throw std::runtime_error("BL");

    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
    uint64_t value;
    if (size != 126) {
      for (int i = 0; i < size; ++i) {
        file_.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
        res.push_back(value);
      }
      return;
    } else {
      for (int i = 0; i < size; ++i) {
        file_.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
        res.push_back(value);
      }
      uint64_t next;
      file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));
      if (next != 0)
        ReadVector(next, res);
    }
  }

  void ReadMainVector(uint64_t offset, std::vector<Data>& res) {

    file_.seekg(offset);
    if (!file_.is_open()) throw std::runtime_error("BL");

    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
    uint64_t DID;
    uint64_t lines_ptr;
    uint64_t freq;
    uint64_t file_size;
    if (size != 127) {
      for (int i = 0; i < size; ++i) {
        file_.seekg(offset + 8 + 32 * i);
        file_.read(reinterpret_cast<char*>(&DID), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&lines_ptr), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&freq), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&file_size), sizeof(uint64_t));
        std::vector<uint64_t> lines;
        ReadVector(lines_ptr, lines);
        res.push_back(Data{DID, lines, freq, file_size});
      }
    } else {
      for (int i = 0; i < size; ++i) {
        file_.seekg(offset + 8 + 32 * i);

        file_.read(reinterpret_cast<char*>(&DID), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&lines_ptr), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&freq), sizeof(uint64_t));
        file_.read(reinterpret_cast<char*>(&file_size), sizeof(uint64_t));

        std::vector<uint64_t> lines;
        ReadVector(lines_ptr, lines);
        res.push_back(Data{DID, lines, freq, file_size});
      }

      uint64_t next;
      file_.seekg(offset + 4088);
      file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));
      if (next != 0)
        ReadMainVector(next, res);
    }
  }

  std::vector<Data> Search(Node* u, std::string& key, DATA) {
    int i = 0;

    while (i < u->keys.size() && key > u->keys[i].first) {
      ++i;
    }
    if (i < u->keys.size() && key == u->keys[i].first) {
      std::vector<Data> res;
      ReadMainVector(u->keys[i].second, res);

      return res;
    }
    if (u->is_leaf) {
      return std::vector<Data>();
    }

    return Search(ReadNode(u->children[i]).get(), key, DATA{});
  }

  void ReadString(uint64_t offset, std::string& res) {
    file_.seekg(offset);
    if (!file_.is_open()) throw std::runtime_error("BL");

    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
    char ch;
    for (uint64_t i = 0; i < size; ++i) {
      file_.get(ch);
      res += ch;
    }
  }

  std::string Search(Node* u, std::string& key, STRING) {
    int i = 0;

    while (i < u->keys.size() && key > u->keys[i].first) {
      ++i;
    }
    if (i < u->keys.size() && key == u->keys[i].first) {
      std::string res;
      ReadString(u->keys[i].second, res);

      return res;
    }
    if (u->is_leaf) {
      return "";
    }

    return Search(ReadNode(u->children[i]).get(), key, STRING{});
  }

  void SplitRoot() {
    auto new_root = std::make_unique<Node>();
    file_.close();
    new_root->offset = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);

    new_root->children = {root_->offset};
    new_root->is_leaf = false;
    new_root->keys = {};

    WriteNode(new_root.get(), new_root->offset);
    SplitChild(new_root.get(), 0);

    root_ = std::move(new_root);
    WriteNode(root_.get(), 0);
  }

  void SplitChild(Node* u, uint64_t i) {
    auto full_node = ReadNode(u->children[i]);
    auto new_node = std::make_unique<Node>();

    new_node->is_leaf = full_node->is_leaf;
    new_node->keys = std::vector<std::pair<std::string, uint64_t>>(full_node->keys.begin() + t_, full_node->keys.end());
    file_.close();
    new_node->offset = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    if (!full_node->is_leaf) {
      new_node->children = std::vector<uint64_t>(full_node->children.begin() + t_, full_node->children.end());
    }

    u->children.insert(u->children.begin() + i + 1, new_node->offset);
    u->keys.insert(u->keys.begin() + i, full_node->keys[t_ - 1]);
    full_node->keys.resize(t_ - 1);
    full_node->children.resize(t_);

    WriteNode(full_node.get(), full_node->offset);
    WriteNode(new_node.get(), new_node->offset);
    WriteNode(u, u->offset);
    WriteNode(root_.get(), 0);
  }

  void WriteVector(uint64_t value, uint64_t offset, bool is_first) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");
    file_.close();
    uint64_t file_size = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    file_.seekg(offset);
    if (is_first) {
      file_.seekp(offset);
      file_.write(buf_smally, 1024);
      file_.seekg(offset);
    }
    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
    if (size != 126) {
      if (size > 0) {
         file_.seekg(offset + (size + 1) * 8 - 8);
        uint64_t prev_value;
        file_.read(reinterpret_cast<char*>(&prev_value), sizeof(uint64_t));

        if (prev_value == value) {
          return;
        }
      }

      file_.seekp(offset + (size + 1) * 8);
      file_.write(reinterpret_cast<char*>(&value), sizeof(uint64_t));
      ++size;
      file_.seekp(offset);
      file_.write(reinterpret_cast<char*>(&size), sizeof(uint64_t));
    } else {
      uint64_t prev;
      file_.seekg(offset + 1024 - 16);
      file_.read(reinterpret_cast<char*>(&prev), sizeof(uint64_t));
      if (prev == value) return;
      file_.seekg(offset + 1024 - 8);
      uint64_t next;
      file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));
      if (next == 0) {
        file_.seekp(file_size);
        file_.write(buf_smally, 1024);
        file_.seekp(offset + 1024 - 8);
        file_.close();
        uint64_t currentFileSize = filesize(file_name_);
        file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
        file_.seekp(offset + 1024 - 8);
        currentFileSize -= 1024;
        file_.write(reinterpret_cast<char*>(&currentFileSize), sizeof(uint64_t));
        WriteVector(value, currentFileSize, false);
      } else {
        WriteVector(value, next, false);
      }
    }
  }

  void WriteNewData(uint64_t value, uint64_t line, uint64_t freq, uint64_t size, uint64_t offset, bool is_first) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");
    file_.close();
    uint64_t file_size = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    file_.seekg(offset);
    file_.seekp(offset);
    if (is_first) {
      file_.write(buf, 4096);
      file_.seekg(offset);
    }
    uint64_t vec_size;
    file_.read(reinterpret_cast<char*>(&vec_size), sizeof(uint64_t));
    if (vec_size != 127) {
      file_.seekp(offset + (1 + vec_size * 4) * 8);
      file_.write(reinterpret_cast<char*>(&value), sizeof(uint64_t));
      file_.close();
      uint64_t file_size2 = filesize(file_name_);
      file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
      file_.seekp(offset + (1 + vec_size * 4) * 8 + 8);
      file_.write(reinterpret_cast<char*>(&file_size2), sizeof(uint64_t));
      WriteVector(line, file_size2, true);
      file_.seekp(offset + (1 + vec_size * 4) * 8 + 16);
      file_.write(reinterpret_cast<char*>(&freq), sizeof(uint64_t));
      file_.write(reinterpret_cast<char*>(&size), sizeof(uint64_t));
      ++vec_size;
      file_.seekp(offset);
      file_.write(reinterpret_cast<char*>(&vec_size), sizeof(uint64_t));

      return;
    }
    file_.seekg(offset + 4088);
    uint64_t next;


    file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));
    if (next == 0) {
      file_.close();
      uint64_t file_size = filesize(file_name_);
      file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
      file_.seekp(file_size);
      file_.write(buf, 4096);
      file_.close();
      uint64_t currentFileSize = filesize(file_name_);
      file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
      file_.seekp(offset + 4088);
      currentFileSize -= 4096;
      file_.write(reinterpret_cast<char*>(&currentFileSize), sizeof(uint64_t));
      WriteNewData(value, line, freq, size, currentFileSize, false);
    } else {
      WriteNewData(value, line, freq, size, next, false);
    }
  }

  uint64_t GetOffset(uint64_t main_offset) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");

    file_.seekg(main_offset);
    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));

    if (size != 127) {
      uint64_t offset;
      file_.seekg(main_offset + (1 + size * 4) * 8 - 24);
      file_.read(reinterpret_cast<char*>(&offset), sizeof(uint64_t));

      return offset;
    }
    file_.close();
    uint64_t file_size = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    file_.seekg(file_size - 8);
    uint64_t next;
    file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));

    return GetOffset(next);
  }

  bool CheckDID(uint64_t DID, uint64_t offset) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");

    file_.seekg(offset);
    uint64_t size;
    file_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));

    if (size != 127) {
      uint64_t DID2;
      file_.seekg(offset + (1 + size * 4) * 8 - 32);
      file_.read(reinterpret_cast<char*>(&DID2), sizeof(uint64_t));
      if (DID == DID2) {
        return true;
      } else {
        return false;
      }
    }
    file_.seekg(offset + 4088);
    uint64_t next;
    file_.read(reinterpret_cast<char*>(&next), sizeof(uint64_t));
    return CheckDID(DID, next);
  }

  void WriteString(uint64_t offset, std::string& value) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");
    file_.close();
    uint64_t file_size = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    file_.seekg(offset);
    file_.seekp(offset);
    char* buf = new char[value.length() + 8]();
    file_.write(buf, value.length() + 8);
    delete[] buf;
    file_.seekg(offset);

    uint64_t size = value.length();

    file_.write(reinterpret_cast<char*>(&size), sizeof(uint64_t));

    for (int i = 0; i < size; ++i) {
      file_.write(&value[i], 1);
    }

  }

  void InsertNotFull(Node* u, std::string key, uint64_t value, uint64_t line, uint64_t freq, uint64_t size) {
    int i = 0;
    while (i < u->keys.size() && key > u->keys[i].first) {
      ++i;
    }
    if (u->is_leaf) {
      if (u->keys.size() == 0) {
        file_.close();
        uint64_t file_size = filesize(file_name_);
        u->keys.insert(u->keys.begin() + i, std::make_pair(key, file_size));
        file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
        WriteNewData(value, line, freq, size, file_size, true);
      } else {
        if (key != u->keys[i].first) {
          file_.close();
          uint64_t file_size = filesize(file_name_);
          u->keys.insert(u->keys.begin() + i, std::make_pair(key, file_size));
          file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
          WriteNewData(value, line, freq, size, file_size, true);
        } else {
          if (CheckDID(value, u->keys[i].second)) {
            WriteVector(line, GetOffset(u->keys[i].second), false);
          } else {
            WriteNewData(value, line, freq, size, u->keys[i].second, false);
          }
        }
      }
      WriteNode(u, u->offset);
      if (root_.get() == u)
        WriteNode(root_.get(), 0);
    } else {
      auto child = ReadNode(u->children[i]);
      if (child->keys.size() == 2 * t_ - 1) {
        SplitChild(u, i);
        i = (key <= u->keys[i].first) ? i : i + 1;
      }
      InsertNotFull(ReadNode(u->children[i]).get(), key, value, line, freq, size);
    }
  }

  void InsertNotFull(Node* u, std::string key, std::string value) {
    int i = 0;
    while (i < u->keys.size() && key > u->keys[i].first) {
      ++i;
    }
    if (u->is_leaf) {
      if (u->keys.size() == 0) {
        file_.close();
        uint64_t file_size = filesize(file_name_);
        u->keys.insert(u->keys.begin() + i, std::make_pair(key, file_size));
        file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
        WriteString(file_size, value);
      } else {
        if (key != u->keys[i].first) {
          file_.close();
          uint64_t file_size = filesize(file_name_);
          u->keys.insert(u->keys.begin() + i, std::make_pair(key, file_size));
          file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
          WriteString(file_size, value);
        }
      }
      WriteNode(u, u->offset);
      if (root_.get() == u)
        WriteNode(root_.get(), 0);
    } else {
      auto child = ReadNode(u->children[i]);
      if (child->keys.size() == 2 * t_ - 1) {
        SplitChild(u, i);
        i = (key <= u->keys[i].first) ? i : i + 1;
      }
      InsertNotFull(ReadNode(u->children[i]).get(), key, value);
    }
  }

  std::unique_ptr<Node> ReadNode(uint64_t offset) {
    file_.seekg(offset);
    if (!file_.is_open()) throw std::runtime_error("BL");

    auto res = std::make_unique<Node>();
    uint64_t keys_size;
    uint64_t children_size;

    file_.read(reinterpret_cast<char*>(&res->is_leaf), sizeof(bool));
    file_.read(reinterpret_cast<char*>(&res->offset), sizeof(uint64_t));
    file_.read(reinterpret_cast<char*>(&keys_size), sizeof(uint64_t));
    file_.read(reinterpret_cast<char*>(&children_size), sizeof(uint64_t));

    std::string key;
    for (int i = 0; i < keys_size; ++i) {
      uint64_t key_length;
      file_.read(reinterpret_cast<char*>(&key_length), sizeof(key_length));

      std::string key;
      try {
        key.resize(key_length);
        file_.read(&key[0], key_length);
      } catch (const std::bad_alloc& e) {

        std::cerr << "Memory allocation failed when trying to read a string of size " << key_length << ": " << e.what() << std::endl;
        std::cerr << "\noffset : " << offset;
        throw;
      }
      uint64_t value;
      file_.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));

      res->keys.emplace_back(std::move(key), std::move(value));
    }

    for (int i = 0; i < children_size; ++i) {
     uint64_t child_offset;
      file_.read(reinterpret_cast<char*>(&child_offset), sizeof(child_offset));
      res->children.push_back(child_offset);
    }

    return res;
  }

  void WriteNode(Node* node, uint64_t offset) {
    if (!file_.is_open()) throw std::runtime_error("Unable to open file for writing.");
    file_.close();
    uint64_t file_size = filesize(file_name_);
    file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    file_.seekp(offset);
    file_.write(buf, 4096);
    file_.seekp(offset);

    file_.write(reinterpret_cast<char*>(&node->is_leaf), sizeof(bool));
    file_.write(reinterpret_cast<char*>(&node->offset), sizeof(uint64_t));
    uint64_t keys_size = node->keys.size();
    uint64_t children_size = node->children.size();
    file_.write(reinterpret_cast<char*>(&keys_size), sizeof(uint64_t));
    file_.write(reinterpret_cast<char*>(&children_size), sizeof(uint64_t));
    for (auto& key_value : node->keys) {
      uint64_t key_length = key_value.first.length();

      file_.write(reinterpret_cast<char*>(&key_length), sizeof(key_length));
      file_.write(key_value.first.c_str(), key_length);

      file_.write(reinterpret_cast<char*>(&key_value.second), sizeof(uint64_t));
    }
    for (int i = 0; i < node->children.size(); ++i) {
      file_.write(reinterpret_cast<char*>(&node->children[i]), sizeof(uint64_t));
    }
  }

  uint64_t filesize(std::string file_name) {
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
      return 0;
    }
    return file.tellg();
  }
};
