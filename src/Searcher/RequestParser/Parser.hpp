#pragma once
#include <iostream>
#include <vector>
#include <ostream>
#include <list>
#include "Expression.hpp"

class Parser {
 public:
 Parser(std::vector<Token> tokens) : tokens_(tokens) {
  int count = 0;
  for (Token& token : tokens) {
    if (token.type_ == TokenType::AND || token.type_ == TokenType::OR) {
      ++count;
    }
  }

  if ((tokens.size() != 4 && tokens.size() != 2) && count == 0) {
    throw std::runtime_error("Wrong request syntax");
  }

 }

  Expression* ParseExpression() {
    if (tokens_.size() == 2 && tokens_[0].type_ == TokenType::LITERAL) {
      return new LiteralExpr(tokens_[0].lexeme_);
    } else if (tokens_.size() == 4 && tokens_[0].type_ == TokenType::LEFT_PAREN
               && tokens_[1].type_ == TokenType::LITERAL && tokens_[2].type_ == TokenType::RIGHT_PAREN) {
      return new LiteralExpr(tokens_[1].lexeme_);
    }

    return ParseTerm();
  }

 private:
  std::vector<Token> tokens_;
  int current_ = 0;

  bool IsAtEnd() {
    return tokens_[current_].type_ == TokenType::EO_F;
  }

  Token Advance() {
    if (!IsAtEnd()) current_++;

    return tokens_[current_ - 1];
  }

  bool Check(TokenType type) {
    if (IsAtEnd()) return false;

    return tokens_[current_].type_ == type;
  }

  bool Match(std::vector<TokenType>&& types) {
    for (auto type : types) {
      if (Check(type)) {
        Advance();
        return true;
      }
    }

    return false;
  }

  Token Consume(TokenType type, std::string&& message) {
    if (Check(type)) return Advance();

    throw std::runtime_error("No closure bracket");
  }

  Expression* ParseTerm() {
    Expression* expr = ParseFactor();

    while (Match({TokenType::OR})) {
      Token operator_ = tokens_[current_ - 1];
      Expression* right = ParseFactor();

      if (right == nullptr || expr == nullptr) {
        throw std::runtime_error("OR operator requires two operands");
      }
      expr = new LogicExpr(expr, operator_, right);
    }
    return expr;
  }

  Expression* ParseFactor() {

    Expression* expr = Primary();

    while (Match({TokenType::AND})) {
      Token operator_ = tokens_[current_ - 1];

      Expression* right = Primary();

      if (right == nullptr || expr == nullptr) {
        throw std::runtime_error("AND operator requires two operands");
      }
      expr = new LogicExpr(expr, operator_, right);
    }

    return expr;
  }

  Expression* Primary() {
    if (Match({TokenType::LITERAL})) {
      return new LiteralExpr(tokens_[current_ - 1].lexeme_);
    }

    if (Match({TokenType::LEFT_PAREN})) {
      Expression* expr = ParseExpression();
      Consume(TokenType::RIGHT_PAREN, "damn");

      return new GroupingExpr(expr);
    }

    return nullptr;
  }
};
