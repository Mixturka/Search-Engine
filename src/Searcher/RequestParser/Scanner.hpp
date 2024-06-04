#pragma once
#include <iostream>
#include <vector>
#include "Expression.hpp"

class Scanner {
 public:
  Scanner(std::string& source) : source_(source) {}

  std::vector<Token> ScanTokens() {
    while (!IsAtEnd()) {
      start_ = current_;
      ScanToken();
    }

    tokens_.emplace_back(Token(TokenType::EO_F, "", ""));

    return tokens_;
  }

  void PrintTokens() {
    for (auto it = tokens_.begin(); it != tokens_.end(); ++it) {
      std::cout << *it << ' ';
    }
  }

 private:
  bool IsAtEnd() {
    return current_ >= source_.length();
  }

  void AddToken(TokenType type, std::string literal) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.emplace_back(Token(type, text, literal));
  }

  bool Match(const char& ch) {
    if (IsAtEnd()) return false;

    if (source_[current_] != ch) return false;

    current_++;

    return true; 
  }

  bool IsChar(const char& ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z');
  }

  char Peek() {
    if (IsAtEnd()) return '\0';

    return source_[current_];
  }

  void ScanToken() {
    if (IsAtEnd()) return;
    char c = source_[current_++];

    switch (c) {
      case '(':
        AddToken(TokenType::LEFT_PAREN, "");
        break;
      case ')':
        AddToken(TokenType::RIGHT_PAREN, "");
        break;
      case 'O':
        if (current_ < source_.length() && Match('R')) {
          AddToken(TokenType::OR, "");
          return;
        }
        break;
      case 'A':
        if (current_ < source_.length() && Match('N')) {
          if (current_ < source_.length() && Match('D')) {
            AddToken(TokenType::AND, "");
            return;
          }
        }
        break;
      default:
        break;
    }

    if (IsChar(c)) {
      while (IsChar(source_[current_])) current_++;

      AddToken(TokenType::LITERAL, "");
    }
  }

  std::string source_;
  std::vector<Token> tokens_;
  int start_ = 0;
  int current_ = 0;
  int line_ = 0; 
};