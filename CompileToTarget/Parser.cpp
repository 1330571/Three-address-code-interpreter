//
// Created by xys on 2021/1/2.
//

#include "Parser.hpp"
#include "codeGen.hpp"
#include "codeHelper.hpp"

#include <utility>

Parser::Parser(Lexer lexer) : lexer(std::move(lexer)) {}


const Lexer &Parser::getLexer() const {
    return lexer;
}

TOKEN Parser::getCurToken() const {
    return curToken;
}

TOKEN Parser::getNextToken() {
    return this->curToken = this->lexer.getToken();
}

ostream &operator<<(ostream &os, const Parser &parser) {
    os << "lexer: " << parser.lexer;
    return os;
}

struct valStrWrapper {
    bool isVar{};
    int val{};
    string var;
};

void Parser::parseToken() {

    auto getOpd = [&]() -> valStrWrapper {
        valStrWrapper v;
        if (this->curToken == TK_NUM) {
            v.isVar = true;
            v.val = this->lexer.getNumLiteral();
        } else {
            v.var = this->lexer.getIdentifier();
        }
        return v;
    };

    auto fetchValue = [](valStrWrapper &v) -> Value * {
        if (v.isVar) return Builder->getInt32(v.val);
        else return NamedValues[v.var];
    };

    TOKEN token = getCurToken();
    if (token == TK_DEF) {
        getNextToken();
        //get Identity
        string funcName = this->lexer.getIdentifier();
        //get Prototype
        auto &&vec = generateEmptyVecStr(funcName);
        //make function definition
        vector<Type *> args(vec.size(), Type::getInt32Ty(*TheContext));
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), args, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, funcName, TheModule.get());

        unsigned idx = 0;
        NamedValues.clear();
        for (auto &Arg : F->args()) {
            Arg.setName(vec[idx]);
            NamedValues[vec[idx++]] = &Arg;
        }

        BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", F);
        Builder->SetInsertPoint(BB);

//        for (auto &Arg: F->args()) {
//            AllocaInst *Alloca = CreateEntryBlockAlloca(F, Arg.getName());
//            Builder->CreateStore(&Arg, Alloca);
//            NamedValues[string(Arg.getName())] = Alloca;
//        }
//        Builder->CreateRet(Builder->getInt32(100));
        bool retFlag = false;
        this->getNextToken(); //peek
        while (!retFlag) {
            TOKEN token1 = this->getCurToken();
            if (token1 == TK_PRM) {
                this->getNextToken();
                if (this->getCurToken() == TK_IDT)this->paramStack.push_back(this->lexer.getIdentifier());
                else this->paramStack.push_back(to_string(this->lexer.getNumLiteral()));
                this->getNextToken();
                continue;
            }
            if (token1 == TK_RTN) {
                this->getNextToken();
                auto opd = getOpd();
                Builder->CreateRet(fetchValue(opd));
                this->getNextToken();
                continue;
            }
            if (token1 == TK_IDT) {
                string leftVar = this->lexer.getIdentifier();
                this->getNextToken(); //:=
                if (this->lexer.getIdentifier() != ":=") cerr << "Error, missing :=" << endl;
                this->getNextToken();
                auto opd1 = getOpd();
                //Make Function Call
                if (opd1.var == "call") {
                    //Function Call Parse
                    this->getNextToken();
                    auto callee = getOpd();
                    Function *functionCallee = TheModule->getFunction(callee.var);
                    vector<Value *> paramValueStack;
                    for (const auto &str: this->paramStack) {
                        if (isDigit(str[0]))
                            paramValueStack.push_back(Builder->getInt32(atoi(str.c_str())));
                        else
                            paramValueStack.push_back(NamedValues[str]);
                    }
                    NamedValues[leftVar] = Builder->CreateCall(functionCallee, paramValueStack, callee.var);
                    this->paramStack.clear();
                    this->getNextToken();
                    continue;
                }

                this->getNextToken();
                if (this->getCurToken() == TK_OP) {
                    //还有下一步需要parse
                    string op = this->lexer.getIdentifier();
                    bool included = false;
                    if (op.size() > 1 && op[1] == '=')included = true;
                    this->getNextToken();
                    auto opd2 = getOpd();
                    NamedValues[leftVar] = createBinaryExpr(fetchValue(opd1), fetchValue(opd2), op[0], included);
                    this->getNextToken(); //将游标指到下一个变量
                } else {
                    NamedValues[leftVar] = fetchValue(opd1);
                }
                continue;
            }
            if (token1 == TK_IF) {
                //IF - ELSE - THEN
                Value *CondV;
                this->getNextToken();
                auto opd1 = getOpd();
                this->getNextToken();
                if (this->getCurToken() == TK_OP) {
                    //还有下一步需要parse
                    string op = this->lexer.getIdentifier();
                    bool included = false;
                    if (op.size() > 1 && op[1] == '=')included = true;
                    this->getNextToken();
                    auto opd2 = getOpd();
                    CondV = createBinaryExpr(fetchValue(opd1), fetchValue(opd2), op[0], included);
                    this->getNextToken(); //将游标指到下一个变量
                } else {
                    CondV = fetchValue(opd1);
                }
//                CondV = Builder->CreateICmpEQ(CondV, ConstantInt::get(*TheContext, APInt(32, 1, true)), "if_cond");

                Function *TheFunction = Builder->GetInsertBlock()->getParent();

// Create blocks for the then and else cases.  Insert the 'then' block at the end of the function.
                BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
                BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
                BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "merge");

                Builder->CreateCondBr(CondV, ThenBB, ElseBB);
                Builder->SetInsertPoint(ThenBB);
                //Then 代码生成
                bool ifEndFlag = true;
                this->getNextToken();
                bool ifElseFlag = false;
                while (ifEndFlag && !ifElseFlag) {
                    TOKEN token2 = this->getCurToken();
                    if (token2 == TK_IF) {
                        cerr << "If In If Does Not support yet" << endl;
                    }
                    if (token2 == TK_RTN) {
                        this->getNextToken();
                        auto opValue = getOpd();
                        Builder->CreateRet(fetchValue(opValue));
                    }
                    if (token2 == TK_END)ifEndFlag = false;
                    if (token2 == TK_ELSE) ifElseFlag = true;
                    this->getNextToken();
                }

                Builder->CreateBr(MergeBB);
                ThenBB = Builder->GetInsertBlock();

                TheFunction->getBasicBlockList().push_back(ElseBB);
                Builder->SetInsertPoint(ElseBB);

                if (ifElseFlag) {

                }

                Builder->CreateBr(MergeBB);
                ElseBB = Builder->GetInsertBlock();

                TheFunction->getBasicBlockList().push_back(MergeBB);
                Builder->SetInsertPoint(MergeBB);

                PHINode *PN = Builder->CreatePHI(Type::getInt32Ty(*TheContext), 2, "iftmp");
                PN->addIncoming(Builder->getInt32(0), ThenBB);
                PN->addIncoming(Builder->getInt32(0), ElseBB);
                continue;
            }
            cerr << "Fatal Error " << endl;
            retFlag = true;
        }
        verifyFunction(*F);
    } else {
        cerr << "无法解析函数名" << endl;
    }
}


