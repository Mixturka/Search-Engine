#pragma once
#include <iostream>
#include "../../Indexer/DataBase/DataBase.hpp"
#include <queue>
#include <map>

class Expression;
class LogicExpr;
class LiteralExpr;
class GroupingExpr;
class Token;


class Interpreter {
 public:
  Interpreter(uint64_t& k) : k_(k) {}
  ~Interpreter() = default;

  void Init(const std::string& path_to_bd);

  std::string VisitLiteralExpr(LiteralExpr* expr);

  std::string VisitGroupExpr(GroupingExpr* expr);

  std::string Evaluate(Expression* expr);

  std::string VisitLogicExpr(LogicExpr* expr);

  void Interpret(Expression* expr);

  void GetORScore(const std::string& term1, const std::string& term2);

  void GetANDScore(const std::string& term1, const std::string& term2);

  void CalculateScores(std::vector<Token> tokens);

  void OutputList();
 private:
  DataBase db_;
  uint64_t request_id_ = -1;

  uint64_t k_;
  std::vector<double> scores_;
  std::vector<Token> tokens_;
  std::vector<std::pair<Data, double>> result_;
  std::map<std::string, std::map<uint64_t, std::vector<uint64_t>>> lines_;
};

enum class TokenType {
  LEFT_PAREN,
  RIGHT_PAREN,
  AND,
  OR,
  LITERAL,
  EO_F,
};

class Token {
 public:
  Token() = default;

  Token(TokenType type, std::string lexeme, std::string literal) : type_(type), lexeme_(lexeme),
                                                                   literal_(literal) {}

  friend std::ostream& operator<<(std::ostream& stream, const Token& token) {
    switch (token.type_) {
        case TokenType::OR:
            stream << "OR";
            break;
        case TokenType::AND:
          stream << "AND";
          break;
        case TokenType::LEFT_PAREN:
            stream << "(";
            break;
        case TokenType::RIGHT_PAREN:
          stream << ")";
          break;
        case TokenType::LITERAL:
          stream << token.lexeme_;
          break;
        case TokenType::EO_F:
          stream << "EOF";
          break;
        default:
            stream << "Unknown Token";
            break;
    }
    return stream;
}
 public:
  TokenType type_;
  std::string lexeme_;
  std::string literal_;
};

class Expression {
 public:
  virtual ~Expression() = default;

  virtual std::string Accept(Interpreter* interpreter) = 0;
  virtual std::string GetType() = 0;
};

class LogicExpr : public Expression {
 public:
  LogicExpr(Expression* left, Token& oper, Expression* right) : left_(left), operator_(oper), right_(right) {}

  std::string Accept(Interpreter* interpreter) override;
  std::string GetType() override {
    return this->type_;
  }

  Expression* GetLeft();

  Expression* GetRight();

  Token GetOperator();
 private:
  Expression* left_;
  Token operator_;
  Expression* right_;
  std::string type_ = "Logic";
};

class LiteralExpr : public Expression {
 public:
   LiteralExpr(std::string& literal) : literal_(literal) {}

  std::string Accept(Interpreter* interpreter) override {
    return interpreter->VisitLiteralExpr(this);
  }
  std::string GetType() override {
    return this->type_;
  }

  std::string GetValue() {
    return literal_;
  }
 private:
  std::string literal_;
  std::string type_ = "Literal";
};

class GroupingExpr: public Expression {
 public:
  GroupingExpr(Expression* expression) : expression_(expression) {}
  std::string GetType() override {
    return this->type_;
  }
  std::string Accept(Interpreter* interpreter) override {
    return interpreter->VisitGroupExpr(this);
  }

  Expression* GetExpr() {
    return this->expression_;
  }
 private:
  Expression* expression_;
  std::string type_ = "Grouping";
};
