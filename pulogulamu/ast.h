#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

template <typename T>
class ASTVisitor;

template <typename T, typename Visitor = ASTVisitor<T>>
class AST {
  public:
    virtual ~AST() = default;
    virtual typename Visitor::type visit(Visitor &);
};

template <typename T, typename Visitor = ASTVisitor<T>>
class Expr : public AST<T> {
  public:
    virtual ~Expr() = default;
    virtual typename Visitor::type visit(Visitor &) override;
};

template <typename T, typename Visitor = ASTVisitor<T>>
class NumberExpr : public Expr<T> {

  public:
    double value;
    NumberExpr(double value)
        : value(value) {}
    virtual typename Visitor::type visit(Visitor &) override;
};

template <typename T, typename Visitor = ASTVisitor<T>>
class VariableExpr : public Expr<T> {

  public:
    std::string name;
    VariableExpr(const std::string &name)
        : name(name) {}
    virtual typename Visitor::type visit(Visitor &) override;
};

template <typename T, typename Visitor = ASTVisitor<T>>
class BinaryExpr : public Expr<T> {
  public:
    std::string op;
    std::unique_ptr<Expr<T>> lhs, rhs;

    BinaryExpr(std::string op, std::unique_ptr<Expr<T>> lhs,
               std::unique_ptr<Expr<T>> rhs)
        : op(std::move(op))
        , lhs(std::move(lhs))
        , rhs(std::move(rhs)) {}
    virtual typename Visitor::type visit(Visitor &) override;
};

template <typename T, typename Visitor = ASTVisitor<T>>
class CallExpr : public Expr<T> {
  public:
    std::string func;
    std::vector<std::unique_ptr<Expr<T>>> args;

    CallExpr(const std::string &func,
             std::vector<std::unique_ptr<Expr<T>>> args)
        : func(func)
        , args(std::move(args)) {}
    virtual typename Visitor::type visit(Visitor &) override;
};

// function prototype
template <typename T, typename Visitor = ASTVisitor<T>>
class Prototype : public AST<T> {
  public:
    std::string name;
    std::vector<std::string> args;

    Prototype(const std::string &name, std::vector<std::string> args)
        : name(name)
        , args(args) {}

    virtual typename Visitor::type visit(Visitor &) override;
    const std::string &get_name() const { return name; }
};

template <typename T, typename Visitor = ASTVisitor<T>>
class Function : AST<T> {
  public:
    std::unique_ptr<Prototype<T>> prototype;
    std::unique_ptr<Expr<T>> body;

    Function(std::unique_ptr<Prototype<T>> proto, std::unique_ptr<Expr<T>> body)
        : prototype(std::move(proto))
        , body(std::move(body)) {}
    virtual llvm::Value *codegen(ASTVisitor<llvm::Value *> &visitor) override;
};

template <typename T>
class ASTVisitor {
  public:
    using type = std::decay_t<T>;
    virtual T visit(NumberExpr<T> &expr) = 0;
    virtual T visit(VariableExpr<T> &expr) = 0;
    virtual T visit(BinaryExpr<T> &expr) = 0;
    virtual T visit(CallExpr<T> &expr) = 0;
    virtual T visit(Prototype<T> &expr) = 0;
    virtual T visit(Function<T> &expr) = 0;
};
