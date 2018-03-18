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


int main(int argc, const char * argv[]) {
    // insert code here...
    std::ifstream infile(argv[1]);
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
    return 0;
}
