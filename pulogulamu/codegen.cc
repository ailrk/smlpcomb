#include "codegen.h"
#include "error.h"

static std::unique_ptr<llvm::LLVMContext> context;
static std::unique_ptr<llvm::Module> module;
static std::unique_ptr<llvm::IRBuilder<>> builder;
static std::unordered_map<std::string, llvm::Value *> environment;

llvm::Value *
log_error_v(const char *str) {
    log_error<Expr<llvm::Value *>>(str);
    return nullptr;
}

// TODO visibility.
ASTCodegenVisitor::type
ASTCodegenVisitor::visit(NumberExpr<type> &expr) {
    return llvm::ConstantFP::get(*context, llvm::APFloat(expr.value));
}

ASTCodegenVisitor::type
ASTCodegenVisitor::visit(VariableExpr<type> &expr) {
    llvm::Value *v = environment[expr.name];
    if (!v) {
        return log_error_v("unknown variable name");
    }
    return v;
}

ASTCodegenVisitor::type
ASTCodegenVisitor::visit(BinaryExpr<type> &expr) {
    llvm::Value *lhs = expr.lhs->visit(*this);
    llvm::Value *rhs = expr.rhs->visit(*this);

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

ASTCodegenVisitor::type
ASTCodegenVisitor::visit(CallExpr<type> &expr) {
    llvm::Function *callee = module->getFunction(expr.func);
    if (!callee) {
        return log_error_v("referencing unknown function");
    }

    if (callee->size() != expr.args.size()) {
        return log_error_v("wrong arity");
    }

    std::vector<llvm::Value *> args_v;
    for (size_t i = 0; i != expr.args.size(); ++i) {
        args_v.push_back(expr.args[i]->visit(*this));
        if (!args_v.back()) {
            return nullptr;
        }
    }

    return builder->CreateCall(callee, args_v, "call");
}

// llvm::Value *ASTCodegenVisitor::visit() { return ;}
