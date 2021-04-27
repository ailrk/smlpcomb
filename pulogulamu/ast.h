#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

template <typename T> class ASTVisitor;

class AST {
public:
  virtual ~AST() = default;
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &);
};

class Expr : public AST {
public:
  virtual ~Expr() = default;
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &) override = 0;
};

class NumberExpr : public Expr {

public:
  double value;
  NumberExpr(double value) : value(value) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

class VariableExpr : public Expr {

public:
  std::string name;
  VariableExpr(const std::string &name) : name(name) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

class BinaryExpr : public Expr {
public:
  std::string op;
  std::unique_ptr<Expr> lhs, rhs;

  BinaryExpr(std::string op, std::unique_ptr<Expr> lhs,
             std::unique_ptr<Expr> rhs)
      : op(std::move(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

class CallExpr : public Expr {
public:
  std::string func;
  std::vector<std::unique_ptr<Expr>> args;

  CallExpr(const std::string &func, std::vector<std::unique_ptr<Expr>> args)
      : func(func), args(std::move(args)) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

// function prototype
class Prototype : public AST {
public:
  std::string name;
  std::vector<std::string> args;

  Prototype(const std::string &name, std::vector<std::string> args)
      : name(name), args(args) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;

  const std::string &get_name() const { return name; }
};

class Function : AST {
public:
  std::unique_ptr<Prototype> prototype;
  std::unique_ptr<Expr> body;

  Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
      : prototype(std::move(proto)), body(std::move(body)) {}
  virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

template <typename T> class ASTVisitor {
public:
  virtual T visit(NumberExpr &expr) = 0;
  virtual T visit(VariableExpr &expr) = 0;
  virtual T visit(BinaryExpr &expr) = 0;
  virtual T visit(CallExpr &expr) = 0;
  virtual T visit(Prototype &expr) = 0;
  virtual T visit(Function &expr) = 0;
};

llvm::Value *NumberExpr::codegen(ASTVisitor<llvm::Value *> &visitor) {
  return visitor.visit(*this);
}

llvm::Value *BinaryExpr::codegen(ASTVisitor<llvm::Value *> &visitor) {
  return visitor.visit(*this);
}

llvm::Value *CallExpr::codegen(ASTVisitor<llvm::Value *> &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Prototype::codegen(ASTVisitor<llvm::Value *> &visitor) {
  return visitor.visit(*this);
}

llvm::Value *Function::codegen(ASTVisitor<llvm::Value *> &visitor) {
  return visitor.visit(*this);
}
