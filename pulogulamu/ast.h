#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

class ASTVisitor;

class AST {
  public:
    virtual ~AST() = default;
    virtual ASTVisitor &accept(ASTVisitor &) = 0;
};

class Expr : public AST {
  public:
    virtual ~Expr() = default;
    virtual ASTVisitor &accept(ASTVisitor &) = 0;
};

class NumberExpr : public Expr {

  public:
    double value;
    NumberExpr(double value)
        : value(value) {}
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

class VariableExpr : public Expr {

  public:
    std::string name;
    VariableExpr(const std::string &name)
        : name(name) {}
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

class BinaryExpr : public Expr {
  public:
    std::string op;
    std::unique_ptr<Expr> lhs, rhs;

    BinaryExpr(std::string op, std::unique_ptr<Expr> lhs,
               std::unique_ptr<Expr> rhs)
        : op(std::move(op))
        , lhs(std::move(lhs))
        , rhs(std::move(rhs)) {}
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

class CallExpr : public Expr {
  public:
    std::string func;
    std::vector<std::unique_ptr<Expr>> args;

    CallExpr(const std::string &func, std::vector<std::unique_ptr<Expr>> args)
        : func(func)
        , args(std::move(args)) {}
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

// function prototype
class Prototype : public AST {
  public:
    std::string name;
    std::vector<std::string> args;

    Prototype(const std::string &name, std::vector<std::string> args)
        : name(name)
        , args(args) {}

    const std::string &get_name() const { return name; }
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

class Function : AST {
  public:
    std::unique_ptr<Prototype> prototype;
    std::unique_ptr<Expr> body;

    Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
        : prototype(std::move(proto))
        , body(std::move(body)) {}
    virtual ASTVisitor &accept(ASTVisitor &) override;
};

class ASTVisitor {
  public:
    template <typename T>
    T get_result();

    virtual ASTVisitor &visit(NumberExpr &expr) = 0;
    virtual ASTVisitor &visit(VariableExpr &expr) = 0;
    virtual ASTVisitor &visit(BinaryExpr &expr) = 0;
    virtual ASTVisitor &visit(CallExpr &expr) = 0;
    virtual ASTVisitor &visit(Prototype &expr) = 0;
    virtual ASTVisitor &visit(Function &expr) = 0;
};

inline ASTVisitor &
NumberExpr::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}

inline ASTVisitor &
VariableExpr::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}

inline ASTVisitor &
BinaryExpr::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}

inline ASTVisitor &
CallExpr::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}

inline ASTVisitor &
Prototype::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}

inline ASTVisitor &
Function::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}
