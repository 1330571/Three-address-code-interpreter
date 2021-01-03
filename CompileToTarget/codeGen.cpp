//
// Created by xys on 2021/1/1.
//

#include "codeGen.hpp"

std::unique_ptr<LLVMContext> TheContext;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, Value *> NamedValues;

void initializeTarget() {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
}

void initializeLLVM() {
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("XYS Compiler", *TheContext);
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

void initializeIOFunc() {
    FunctionCallee CalleeF = TheModule->getOrInsertFunction("printf",
                                                            FunctionType::get(IntegerType::getInt32Ty(*TheContext),
                                                                              PointerType::get(
                                                                                      Type::getInt8Ty(*TheContext),
                                                                                      0),
                                                                              true));
    FunctionType *readFnType = llvm::FunctionType::get(Builder->getInt32Ty(), true);
    Function *readFn = Function::Create(readFnType, GlobalValue::ExternalLinkage, "scanf", *TheModule);

}

Value *createBinaryExpr(Value *lhs, Value *rhs, char op, bool includeEQ = false) {
    switch (op) {
        case '+':
            return Builder->CreateAdd(lhs, rhs, "add");
        case '-':
            return Builder->CreateSub(lhs, rhs, "sub");
        case '*':
            return Builder->CreateMul(lhs, rhs, "mul");
        case '/':
            return Builder->CreateSDiv(lhs, rhs, "div");
        case '<': {
            if (!includeEQ) {
                Value *v = Builder->CreateCmp(CmpInst::ICMP_SLT, lhs, rhs, "cmp_le");
                v = Builder->CreateIntCast(v, Type::getInt32Ty(*TheContext), true);
                return v;
            }
        }
//            else return Builder->CreateCmp(CmpInst::FCMP_OLT, lhs, rhs, "cmp_le_eq");
//        case '>':
//            if (includeEQ) return Builder->CreateCmp(CmpInst::FCMP_OGE, lhs, rhs, "cmp_ge");
//            else return Builder->CreateCmp(CmpInst::FCMP_OGT, lhs, rhs, "cmp_ge_eq");
        case '=': {
            Value *v = Builder->CreateCmp(CmpInst::ICMP_EQ, lhs, rhs, "cmp_eq");
//            v = Builder->CreateIntCast(v, Type::getInt32Ty(*TheContext), true);
            return v;
        }
//        case '!':
//            return Builder->CreateCmp(CmpInst::FCMP_ONE, lhs, rhs, "cmp_ne");
        default:
            cout << "Error Op " << op << endl;
            assert(0);
    }
}

//Value *callFunction() {
//}