#include "parser.h"
#include "lexer.h"
#include "error.h"

Token Parser::next_tok() { return tok = lexer.get_token(); }

int Parser::token_precedence() {
  if (tok.istok()) {
    return -1;
  }

  char t = tok.value();
  int precedence = binary_op_precedence_table[t];
  return precedence <= 0 ? -1 : precedence;
}

// number         ::= ..
std::unique_ptr<Expr> Parser::numberexpr() {
  auto result = std::make_unique<NumberExpr>(lexer.n_val);
  next_tok();
  return std::move(result);
}

// parenexpr      ::= '(' expression ')'
std::unique_ptr<Expr> Parser::parenexpr() {
  if (tok.value() != '(') {
    return log_error<Expr>("expected (");
  }
  next_tok();

  auto result = expression();
  if (!result) {
    return nullptr;
  }

  if (tok.value() != ')')
    return log_error<Expr>("expected )");

  next_tok();
  return result;
}

// identifierexpr ::= identifier | identifier '(' expression * ')'
std::unique_ptr<Expr> Parser::identifierexpr() {
  const std::string identifer = lexer.identifier_str;
  next_tok();

  if (tok.value() != '(') { // is variable name.
    return std::make_unique<VariableExpr>(identifer);
  }

  // is function call
  next_tok();
  std::vector<std::unique_ptr<Expr>> args;
  if (tok.value() != ')') {
    while (true) {
      if (auto e = expression()) {
        args.push_back(std::move(e));
      } else {
        return nullptr;
      }
      if (tok.value() == ')') {
        break;
      }
      if (tok.value() != ',') {
        return log_error<Expr>("Expected ')' or ',' in argument list");
      }
      next_tok(); // eat ','
    }
  }

  next_tok();
  return std::make_unique<CallExpr>(identifer, std::move(args));
}

// primary        ::= identifierexpr | numberexpr | parenexpr
std::unique_ptr<Expr> Parser::primary() {
  switch (tok.value()) {
  case TokenType::TokenDef:
    return identifierexpr();
  case TokenType::TokenNumber:
    return numberexpr();
  case '(':
    return parenexpr();
  default:
    return log_error<Expr>("unknown token for primary expression");
  }
}

// binoprhs       ::= (biop primary)*
std::unique_ptr<Expr> Parser::binoprhs(int exprprec,
                                       std::unique_ptr<Expr> lhs) {
  while (true) {
    int binopprec = token_precedence();

    // if next tok != binop, the precedence is the min value -1.
    if (binopprec < exprprec) {
      return lhs;
    }

    std::string binop(1, tok.value());
    next_tok();

    auto rhs = primary();

    // in case if next is a binop with higher precedence,
    int nextprec = token_precedence();
    if (binopprec < nextprec) {
      rhs = binoprhs(binopprec + 1, std::move(rhs));
      if (!rhs) {
        return nullptr;
      }
    }
    lhs = std::make_unique<BinaryExpr>(binop, std::move(lhs), std::move(rhs));
  }
}

// expression     ::= primary binoprhs
std::unique_ptr<Expr> Parser::expression() {
  auto lhs = primary();
  if (!lhs) {
    return nullptr;
  }
  return binoprhs(0, std::move(lhs));
}

// prototype      ::= id '(' id* ')'
std::unique_ptr<Prototype> Parser::prototype() {
  if (tok.value() != TokenType::TokenIdentifier) {
    return log_error<Prototype>("Expected function name");
  }

  const std::string name = lexer.identifier_str;
  next_tok();

  if (tok.value() != '(') {
    return log_error<Prototype>("Expected ( in prototype");
  }

  std::vector<std::string> arg_names;
  while (TokenType::TokenIdentifier == next_tok().value()) {
    arg_names.push_back(lexer.identifier_str);
  }
  if (tok.value() != ')') {
    return log_error<Prototype>("Expected ) in prototype");
  }

  next_tok();
  return std::make_unique<Prototype>(name, std::move(arg_names));
}

// definition ::= 'def' prototype expression
std::unique_ptr<Function> Parser::definition() {
  next_tok();
  auto proto = prototype();
  if (!proto) {
    return nullptr;
  }
  if (auto e = expression()) {
    return std::make_unique<Function>(std::move(proto), std::move(e));
  }
  return nullptr;
}

// external       ::= 'extern' prototype
std::unique_ptr<Prototype> Parser::external() {
  next_tok();
  return prototype();
}

// toplevelexpr   ::= expression
std::unique_ptr<Function> Parser::toplevelexpr() {
  if (auto e = expression()) {
    auto proto =
        std::make_unique<Prototype>("__anon_expr", std::vector<std::string>());
    return std::make_unique<Function>(std::move(proto), std::move(e));
  }

  return nullptr;
}

void Parser::handle_definition() {
  if (definition()) {
    fprintf(stderr, "parsed a function definiton\n");
  } else {
    next_tok();
  }
}

void Parser::handle_extern() {
  if (external()) {
    fprintf(stderr, "parsed a function definiton\n");
  } else {
    next_tok();
  }
}

void Parser::handle_toplevel_expr() {
  if (toplevelexpr()) {
    fprintf(stderr, "parsed a function definiton\n");
  } else {
    next_tok();
  }
}

void Parser::run() {
  while (true) {
    fprintf(stderr, "ready>");
    switch (tok.value()) {
    case TokenEOF:
      return;
    case ';':
      next_tok();
      break;
    case TokenDef:
      handle_definition();
      break;
    case TokenExtern:
      handle_extern();
      break;
    default:
      handle_toplevel_expr();
      break;
    }
  }
}

#ifdef SIMPOU_TEST_PARSER
// TODO test
#include <iostream>
int main(void) {
  std::cout << "hi" << std::endl;
  Parser p;
  p.run();

  return 0;
}
#endif
