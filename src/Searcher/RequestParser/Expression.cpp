#include "Expression.hpp"

std::string LogicExpr::Accept(Interpreter* interpreter) {
  return interpreter->VisitLogicExpr(this);
}

Expression* LogicExpr::GetLeft() {
  if ((this->right_->GetType() != "Grouping" && this->right_->GetType() != "Literal") || this->right_ == nullptr) throw std::runtime_error("Wrong request syntax");
  return this->left_;
}

Expression* LogicExpr::GetRight() {
  if ((this->right_->GetType() != "Grouping" && this->right_->GetType() != "Literal") || this->right_ == nullptr) throw std::runtime_error("Wrong request syntax");
  return this->right_;
}

Token LogicExpr::GetOperator() {
  return this->operator_;
}