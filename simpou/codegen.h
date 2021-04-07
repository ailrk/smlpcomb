#pragma once
#include "ast.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

// can make other visitors too.
class ASTCodegenVisitor : public ASTVisitor<llvm::Value *> {
public:
  virtual llvm::Value *visit(NumberExpr &expr) override;
  virtual llvm::Value *visit(VariableExpr &expr) override;
  virtual llvm::Value *visit(BinaryExpr &expr) override;
  virtual llvm::Value *visit(CallExpr &expr) override;
  virtual llvm::Value *visit(Prototype &expr) override;
  virtual llvm::Value *visit(Function &expr) override;
};
