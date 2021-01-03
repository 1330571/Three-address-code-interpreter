//
// Created by xys on 2021/1/1.
//

#ifndef COMPILE_TO_TARGET_CODEGEN_HPP
#define COMPILE_TO_TARGET_CODEGEN_HPP

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Alignment.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <string>

using namespace llvm;
using namespace std;

extern std::unique_ptr<LLVMContext> TheContext; //LLVM Context变量
extern std::unique_ptr<Module> TheModule; //LLVM Module变量
extern std::unique_ptr<IRBuilder<>> Builder; //LLVM Builder变量
extern std::map<std::string, Value *> NamedValues; //运行时对于Value值存储，为了契合LLVM的SSA形式

void initializeLLVM();

void initializeTarget();

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
                                          StringRef VarName) {
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                     TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type::getInt32Ty(*TheContext), nullptr, VarName);
}

Value *createBinaryExpr(Value *lhs, Value *rhs, char op, bool includeEQ);

void initializeIOFunc();

#endif //COMPILE_TO_TARGET_CODEGEN_HPP
