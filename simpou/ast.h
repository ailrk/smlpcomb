#pragma once
#include <memory>
#include <string>
#include <vector>

class AST {
  public:
    virtual ~AST() = default;
};

class Expr : public AST {
  public:
    virtual ~Expr() = default;
};

class NumerExpr : public Expr {
    double value;

  public:
    NumerExpr(double value)
        : value(value) {}
};

class VariableExpr : public Expr {
    std::string name;

  public:
    VariableExpr(const std::string &name)
        : name(name) {}
};

class BinaryExpr : public Expr {
    std::string op;
    std::unique_ptr<Expr> lhs, rhs;

  public:
    BinaryExpr(std::string op, std::unique_ptr<Expr> lhs,
               std::unique_ptr<Expr> rhs)
        : op(std::move(op))
        , lhs(std::move(lhs))
        , rhs(std::move(rhs)) {}
};

class CallExpr : public Expr {
    std::string func;
    std::vector<std::unique_ptr<Expr>> args;

  public:
    CallExpr(const std::string &func, std::vector<std::unique_ptr<Expr>> args)
        : func(func)
        , args(std::move(args)) {}
};

// function prototype
class Prototype : public AST {
    std::string name;
    std::vector<std::string> args;

  public:
    Prototype(const std::string &name, std::vector<std::string> args)
        : name(name)
        , args(args) {}

    const std::string &get_name() const { return name; }
};

class Function : AST {
    std::unique_ptr<Prototype> prototype;
    std::unique_ptr<Expr> body;

  public:
    Function(std::unique_ptr<Prototype> proto, std::unique_ptr<Expr> body)
        : prototype(std::move(proto))
        , body(std::move(body)) {}
};
