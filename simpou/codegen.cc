#include "codegen.h"

static std::unique_ptr<llvm::LLVMContext> context;
static std::unique_ptr<llvm::Module> module;
static std::unique_ptr<llvm::IRBuilder<>> builder;
static std::unordered_map<std::string, llvm::Value *> environment;

// TODO visibility.
llvm::Value *ASTCodegenVisitor::visit(NumberExpr &expr) {
  return llvm::ConstantFP::get(*context, llvm::APFloat());
}
