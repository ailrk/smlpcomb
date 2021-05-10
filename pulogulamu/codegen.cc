#include "codegen.h"
#include "error.h"

static std::unique_ptr<llvm::LLVMContext> context;
static std::unique_ptr<llvm::Module> module;
static std::unique_ptr<llvm::IRBuilder<>> builder;
static std::unordered_map<std::string, llvm::Value *> environment;

llvm::Value *
log_error_v(const char *str) {
    log_error<Expr>(str);
    return nullptr;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(NumberExpr &expr) {
    result = llvm::ConstantFP::get(*context, llvm::APFloat(expr.value));
    return *this;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(VariableExpr &expr) {
    llvm::Value *v = environment[expr.name];
    if (!v) {
        result = log_error_v("unknown variable name");
    } else {
        result = v;
    }
    return *this;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(BinaryExpr &expr) {
    // ASTCodegenVisitor &ref =
    //     dynamic_cast<ASTCodegenVisitor &>(expr.lhs->accept(*this));

    llvm::Value *lhs = expr.lhs->accept(*this).get_result<type>();
    llvm::Value *rhs = expr.rhs->accept(*this).get_result<type>();

    if (!lhs || !rhs) {
        result = nullptr;
    } else if (expr.op == "+") {
        result = builder->CreateFAdd(lhs, rhs, "add");
    } else if (expr.op == "-") {
        result = builder->CreateFSub(lhs, rhs, "sub");
    } else if (expr.op == "*") {
        result = builder->CreateFMul(lhs, rhs, "mul");
    } else if (expr.op == "/") {
        result = builder->CreateFDiv(lhs, rhs, "div");
    } else if (expr.op == "<") {
        lhs = builder->CreateFCmpULT(lhs, rhs, "lt");
        result = builder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(*context),
                                       "bool");
    }

    result = log_error_v("unknown binary operator");
    return *this;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(CallExpr &expr) {
    llvm::Function *callee = module->getFunction(expr.func);
    if (!callee) {
        result = log_error_v("referencing unknown function");
        return *this;
    } else if (callee->size() != expr.args.size()) {
        result = log_error_v("wrong arity");
        return *this;
    }

    std::vector<llvm::Value *> args_v;
    for (size_t i = 0; i != expr.args.size(); ++i) {
        args_v.push_back(expr.args[i]->accept(*this).get_result<type>());
        if (!args_v.back()) {
            result = nullptr;
            return *this;
        }
    }

    result = builder->CreateCall(callee, args_v, "call");
    return *this;
}


ASTCodegenVisitor &

// llvm::Value *ASTCodegenVisitor::codegen() { return ;}
