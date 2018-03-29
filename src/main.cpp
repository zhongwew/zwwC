//
//  main.cpp
//  zwwC
//
//  Created by Frank  on 1/25/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "Scanner.hpp"
#include "Parser.hpp"
#include "AST.hpp"
//using namespace llvm;

#ifdef LLVM_ON_WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
  fputc((char)X, stderr);
  return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
  fprintf(stderr, "%f\n", X);
  return 0;
}

void testFunction(){
    if(auto* testfunc = TheModule->getFunction("testFunc"))
        fprintf(stderr,"has function-------------------------\n");
    else 
        fprintf(stderr,"no function--------------------------\n");
}


int main(int argc, const char * argv[]) {
    // insert code here...
    std::ifstream infile(argv[1]);
    TheModule = new llvm::Module("my cool jit", TheContext);
    Scanner * scn = new Scanner();
    scn->scan(infile);
    scn->printToken();
    Parser * parser = new Parser(scn->getList());
    auto asTree = parser->parseProgram();
    // Make the module, which holds all the code.
    //TheModule = llvm::make_unique<Module>("my cool jit", TheContext);
    //TheModule = new llvm::Module("my cool jit", TheContext);
    asTree->codegen();
    // Print out all of the generated code
    TheModule->print(errs(), nullptr);
    testFunction();
    if(strcmp(argv[2],"-o") == 0){
        //output .o file
        //initialize the target repo
        fprintf(stderr,"start output\n");
        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        auto targetTriple = sys::getDefaultTargetTriple();
        TheModule->setTargetTriple(targetTriple);

        std::string error;
        auto Target = TargetRegistry::lookupTarget(targetTriple,error);
        //if the target didn't init sucessfully
        if(!Target){
            errs()<<error;
            return 1;
        }
        auto CPU = "generic";
        auto Feature = "";

        TargetOptions opt;
        auto RM = Optional<Reloc::Model>();
        auto TheTargetMachine = Target->createTargetMachine(targetTriple,CPU,Feature,opt,RM);
        TheModule->setDataLayout(TheTargetMachine->createDataLayout());

        auto Filename = argv[3];
        std::error_code ec;
        raw_fd_ostream dest(Filename,ec,sys::fs::F_None);
        //if output error
        if(ec){
            errs()<<"couldn't open file: "<<ec.message();
            return 1;
        }
        legacy::PassManager pass;
        auto file_type = TargetMachine::CGFT_ObjectFile;
        if(TheTargetMachine->addPassesToEmitFile(pass,dest,file_type)){
            errs() << "TheTargetMachine can't emit a file of this type";
            return 1;
        }
        pass.run(*::TheModule);
        dest.flush();
        outs()<<"Wrote: "<<Filename<<"\n";
    }
    return 0;
}
