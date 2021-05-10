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
class ASTCodegenVisitor : public ASTVisitor {
  public:
    using type = llvm::Value *;

  public:
    type result;
    virtual ASTCodegenVisitor &visit(NumberExpr &expr) override;
    virtual ASTCodegenVisitor &visit(VariableExpr &expr) override;
    virtual ASTCodegenVisitor &visit(BinaryExpr &expr) override;
    virtual ASTCodegenVisitor &visit(CallExpr &expr) override;
    virtual ASTCodegenVisitor &visit(Prototype &expr) override;
    virtual ASTCodegenVisitor &visit(Function &expr) override;
};

// pull value out of a vistor.
template <>
ASTCodegenVisitor::type
ASTVisitor::get_result<ASTCodegenVisitor::type>() {
    return dynamic_cast<ASTCodegenVisitor &>(*this).result;
}
