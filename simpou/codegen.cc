#include "codegen.h"
#include "error.h"

static std::unique_ptr<llvm::LLVMContext> context;
static std::unique_ptr<llvm::Module> module;
static std::unique_ptr<llvm::IRBuilder<>> builder;
static std::unordered_map<std::string, llvm::Value *> environment;

llvm::Value *log_error_v(const char *str) {
  log_error<Expr>(str);
  return nullptr;
}

// TODO visibility.
llvm::Value *ASTCodegenVisitor::visit(NumberExpr &expr) {
  return llvm::ConstantFP::get(*context, llvm::APFloat(expr.value));
}

llvm::Value *ASTCodegenVisitor::visit(VariableExpr &expr) {
  llvm::Value *v = environment[expr.name];
  if (!v) {
    return log_error_v("unknown variable name");
  }
  return v;
}

llvm::Value *ASTCodegenVisitor::visit(BinaryExpr &expr) {
  llvm::Value *lhs = expr.lhs->codegen(*this);
  llvm::Value *rhs = expr.rhs->codegen(*this);

  if (!lhs || !rhs) {
    return nullptr;
  }

  if (expr.op == "+") {
    return builder->CreateFAdd(lhs, rhs, "add");
  } else if (expr.op == "-") {
    return builder->CreateFSub(lhs, rhs, "sub");
  } else if (expr.op == "*") {
    return builder->CreateFMul(lhs, rhs, "mul");
  } else if (expr.op == "/") {
    return builder->CreateFDiv(lhs, rhs, "div");
  } else if (expr.op == "<") {
    lhs = builder->CreateFCmpULT(lhs, rhs, "lt");
    return builder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(*context),
                                 "bool");
  }

  return log_error_v("unknown binary operator");
}

llvm::Value *ASTCodegenVisitor::visit(CallExpr &expr) {
  llvm::Function *callee = module->getFunction(expr.func);
  if (!callee) {
    return log_error_v("referencing unknown function");
  }

  if (callee->size() != expr.args.size()) {
    return log_error_v("wrong arity");
  }

  std::vector<llvm::Value *> args_v;
  for (size_t i = 0; i != expr.args.size(); ++i) {
    args_v.push_back(expr.args[i]->codegen(*this));
    if (!args_v.back()) {
      return nullptr;
    }
  }

  return builder->CreateCall(callee, args_v, "call");
}


// llvm::Value *ASTCodegenVisitor::visit() { return ;}
