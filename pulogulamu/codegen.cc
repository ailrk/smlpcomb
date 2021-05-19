#include "codegen.h"
#include "error.h"

// llvm context.
static std::unique_ptr<llvm::LLVMContext> context;

// top level IR construct.
// module owns memory for all IR generated.
static std::unique_ptr<llvm::Module> module;

static std::unique_ptr<llvm::IRBuilder<>> builder;

// symbol table for current scope.
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

    llvm::Value *lhs = expr.lhs->accept(*this).get<ASTCodegenVisitor>();
    llvm::Value *rhs = expr.rhs->accept(*this).get<ASTCodegenVisitor>();

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
        // converet bool to float.
        result = builder->CreateUIToFP(lhs, llvm::Type::getDoubleTy(*context),
                                       "bool");
    } else {
        result = log_error_v("unknown binary operator");
    }

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

    std::vector<llvm::Value *> args;

    for (auto &arg : expr.args) {
        args.push_back(arg->accept(*this).get<ASTCodegenVisitor>());
        if (!args.back()) {
            result = nullptr;
            return *this;
        }
    }

    result = builder->CreateCall(callee, args, "call");
    return *this;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(Prototype &func) {
    std::vector<llvm::Type *> doubles{ func.args.size(),
                                       llvm::Type::getDoubleTy(*context) };

    llvm::FunctionType *ft = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*context), doubles, false);

    llvm::Function *f = llvm::Function::Create(
        ft, llvm::Function::ExternalLinkage, func.name, module.get());

    unsigned idx = 0;
    for (auto &arg : f->args()) {
        arg.setName(func.args[idx++]);
    }

    result = f;
    return *this;
}

ASTCodegenVisitor &
ASTCodegenVisitor::visit(Function &func) {
    llvm::Function *f = module->getFunction(func.prototype->get_name());

    if (!f) {
        f = static_cast<llvm::Function *>(
            func.prototype->accept(*this).get<ASTCodegenVisitor>());
    }

    if (!f) {
        result = nullptr;
        return *this;
    }

    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*context, "entry", f);
    builder->SetInsertPoint(bb);

    // add arguments into the environment.
    environment.clear();
    for (auto &arg : f->args()) {
        environment[std::string{ arg.getName() }] = &arg;
    }

    if (llvm::Value *ret = func.body->accept(*this).get<ASTCodegenVisitor>()) {
        builder->CreateRet(ret);
        llvm::verifyFunction(*f);
        result = f;
        return *this;
    }

    f->eraseFromParent();
    result = nullptr;
    return *this;
}

void
init_module() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("pulogulamu", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}
