#include "Interpreter.hpp"
#include "../RequestParser/Expression.hpp"
#include <unordered_set>
#include <set>

void Interpreter::Init(const std::string& path_to_bd) {
  this->db_.Load(path_to_bd);
  this->scores_.resize(this->db_.doc_num_);

  std::fill(scores_.begin(), scores_.end(), 0);
}

std::string Interpreter::VisitLiteralExpr(LiteralExpr* expr) {
  std::vector<Data> posting_list = this->db_.posting_lists_->Search(expr->GetValue(), BTree::DATA{});
  this->result_.resize(posting_list.size());

  for (const Data& data : posting_list) {
    this->result_[data.DID] = std::make_pair(data, this->scores_[data.DID]);
  }

  for (const Data& data : posting_list) {
    for (int i = 0; i < data.lines.size(); ++i)
      this->lines_[expr->GetValue()][data.DID].push_back(data.lines[i]);
  }

  return expr->GetValue();
}

std::string Interpreter::VisitGroupExpr(GroupingExpr* expr) {
  return Evaluate(expr->GetExpr());
}

std::string Interpreter::Evaluate(Expression* expr) {
  return expr->Accept(this);
}

void Interpreter::GetORScore(const std::string& term1, const std::string& term2) {
  std::vector<Data> posting_list1 = this->db_.posting_lists_->Search(term1, BTree::DATA{});
  std::vector<Data> posting_list2 = this->db_.posting_lists_->Search(term2, BTree::DATA{});
  std::unordered_set<uint64_t> added_docs;
  std::vector<Data> united_v;

  for (const Data& data : posting_list1) {
    united_v.push_back(data);
    added_docs.insert(data.DID);
    for (int i = 0; i < data.lines.size(); ++i) {
      this->lines_[term1][data.DID].push_back(data.lines[i]);
    }
  }

  for (const Data& data : posting_list2) {
    if (added_docs.find(data.DID) == added_docs.end()) {
      united_v.push_back(data);
      added_docs.insert(data.DID);
    }
    for (int i = 0; i < data.lines.size(); ++i) {
      this->lines_[term2][data.DID].push_back(data.lines[i]);
    }
  }

  uint64_t max = 0;
  for (const auto& data: united_v) {
    if (data.DID > max) max = data.DID;
  }

  this->result_.resize(max + 1);
  for (const Data& data : united_v) {
    this->result_[data.DID] = std::make_pair(data, this->scores_[data.DID]);
  }

  this->db_.Insert(term1 + "OR" + term2, united_v);

  std::sort(this->result_.begin(), this->result_.end(),
            [](const std::pair<Data, double>& first, const std::pair<Data, double>& second) {
              return first.second > second.second;
  });
}

void Interpreter::GetANDScore(const std::string& term1, const std::string& term2) {
  std::vector<Data> posting_list1 = this->db_.posting_lists_->Search(term1, BTree::DATA{});
  std::vector<Data> posting_list2 = this->db_.posting_lists_->Search(term2, BTree::DATA{});

  std::vector<Data> united;

  this->lines_ = std::map<std::string, std::map<uint64_t, std::vector<uint64_t>>>();
  for (const Data& data : posting_list1) {
    for (const Data& data2 : posting_list2) {
      if (data.DID == data2.DID) {
        united.push_back(data);

        for (int i = 0; i < data.lines.size(); ++i) {
          this->lines_[term1][data.DID].push_back(data.lines[i]);
        }
        for (int i = 0; i < data2.lines.size(); ++i) {
          this->lines_[term2][data.DID].push_back(data2.lines[i]);
        }
      }
    }
  }

  uint64_t max = 0;

  for (const auto& data: united) {
    if (data.DID > max) max = data.DID;
  }

  if (max >= result_.size()) {
    this->result_.resize(max + 1);
  }

  for (const Data& data : united) {
    this->result_[data.DID] = std::make_pair(data, this->scores_[data.DID]);
  }

  this->db_.Insert(term1 + "AND" + term2, united);

  std::sort(this->result_.begin(), this->result_.end(),
            [](std::pair<Data, double> first, std::pair<Data, double> second) {
              return first.second > second.second;
  });
}

std::string Interpreter::VisitLogicExpr(LogicExpr* expr) {
  std::string left = Evaluate(expr->GetLeft());
  std::string right = Evaluate(expr->GetRight());

  switch (expr->GetOperator().type_) {
   case TokenType::AND:

    this->GetANDScore(left, right);

    return left + "AND" + right;
   case TokenType::OR:

    this->GetORScore(left, right);
    return left + "OR" + right;
   default:
    break;
  }

  return "";
}

void Interpreter::Interpret(Expression* expr) {
  std::string value = Evaluate(expr);
}

void Interpreter::CalculateScores(std::vector<Token> tokens) {
  this->tokens_ = tokens;
  for (Token& token : tokens) {
    if (token.type_ == TokenType::LITERAL) {
      std::vector<Data> list = this->db_.posting_lists_->Search(token.lexeme_, BTree::DATA{});

      for (Data& data : list) {
        double avg_doc_length = double(this->db_.dl_sum_) / double(this->db_.doc_num_);
        double term_freq_norm = double(data.freq) / (data.freq + 1 * (1 - 1 + 1 * (double(data.size) / avg_doc_length)));
        double idf = log(double(this->db_.doc_num_ - list.size() + 0.5) / double(list.size() + 0.5) + 1);
        double score = term_freq_norm * idf;
        this->scores_[data.DID] += score;
      }
    }
  }
}

void Interpreter::OutputList() {
  for (int i = 0; i < std::min(this->k_, uint64_t(this->result_.size())); ++i) {
    std::cout << "Document ID: " << this->result_[i].first.DID << '\n';
    std::cout << "File: " << this->db_.did_list_->Search(std::to_string(this->result_[i].first.DID), BTree::STRING{}) << '\n';
    std::cout << "###########LINES:########### \n";

    for (Token& token : this->tokens_) {
      if (token.type_ == TokenType::LITERAL) {
        auto post_list = this->db_.posting_lists_->Search(token.lexeme_, BTree::DATA{});
        if (post_list.size() > 0) {
          std::cout << token.lexeme_ << ": ";
          for (Data& data : post_list) {
            if (data.DID == this->result_[i].first.DID) {
              for (int i = 0; i < data.lines.size(); ++i) {
                std::cout << data.lines[i] << ' ';
              }
            }
          }
          std::cout << '\n';
        }
      }
    }
  }


  this->result_ = std::vector<std::pair<Data, double>>();
  this->lines_ = std::map<std::string, std::map<uint64_t, std::vector<uint64_t>>>();
  this->scores_ = std::vector<double>();
}
