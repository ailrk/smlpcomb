#pragma once
#include "ast.h"
#include "lexer.h"
#include <algorithm>
#include <string_view>
#include <unordered_map>

static std::unordered_map<char, int> binary_op_precedence_table = {
    {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}, {'/', 40},
};

// bnf
//
// external       ::= 'extern' prototype
// definition     ::= 'def' prototype expression
// prototype      ::= id '(' id* ')'
//
// toplevelexpr   ::= expression
// expression     ::= primary binoprhs
//
// binoprhs       ::= (biop primary)*
// primary        ::= identifierexpr | numberexpr | parenexpr
//
// identifierexpr ::= identifier | identifier '(' expression * ')'
// numberexpr     ::= number
// parenexpr      ::= '(' expression ')'
//
// biop           ::= '+' | '-' | '*' | '/' | '<'
// number         ::= ..
// identifier     ::= ..

class Parser {
  Lexer lexer;
  Token tok;

  int token_precedence();
  Token next_tok();

  std::unique_ptr<Expr> numberexpr();
  std::unique_ptr<Expr> parenexpr();
  std::unique_ptr<Expr> identifierexpr();
  std::unique_ptr<Expr> primary();
  std::unique_ptr<Expr> binoprhs(int exprprec, std::unique_ptr<Expr> lhs);
  std::unique_ptr<Expr> expression();
  std::unique_ptr<Prototype> prototype();
  std::unique_ptr<Function> definition();
  std::unique_ptr<Prototype> external();
  std::unique_ptr<Function> toplevelexpr();

  void handle_definition();
  void handle_extern();
  void handle_toplevel_expr();

public:
  Parser() : tok(' '), lexer(Lexer()) {}
  void run();
};


