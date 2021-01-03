#include <iostream>
#include <fstream>
#include <cstdio>

#include "codeGen.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "type.hpp"
#include "FuncPrototype.hpp"

#define G_OBJ

int buildObj();

int main() {
    std::cout << "XYS Coder, Compile three-address-code to LLVM IR and To Target Machine" << std::endl;
    std::cout << "Initializing LLVM Components" << std::endl;
    initializeLLVM();
    std::cout << "Initializing LLVM Target Machine Components" << std::endl;
    initializeTarget();
//    initializeIOFunc();
    ifstream file("fib");
    vector<string> codeLine;
    string temp;
    while (getline(file, temp))
        codeLine.push_back(temp);
    buildFunc("S2");
    Lexer lexer(codeLine);
    Parser parser(lexer);

//    while (parser.getNextToken() != TK_EOF) {
//        cout << print_helper(parser.getCurToken()) << " " << parser << endl;
//    }

#ifdef G_OBJ
    parser.getNextToken();
    parser.parseToken();

    TheModule->print(errs(), nullptr);
    buildObj();
#endif

    return 0;
}

int buildObj() {
    cout << "生成obj文件中 ... " << endl;
    auto TargetTriple = sys::getDefaultTargetTriple();
    string err;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, err);
    if (!Target) {
        errs() << err;
        return 1;
    }
    auto CPU = "generic";
    auto Features = "";
    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
    TheModule->setDataLayout(TargetMachine->createDataLayout());
    TheModule->setTargetTriple(TargetTriple);
    //Emit Object Code
    auto Filename = "fib.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);
    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }
    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;


    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*TheModule);
    dest.flush();
    return 0;
}