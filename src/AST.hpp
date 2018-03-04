//
//  AST.hpp
//  zwwC
//
//  Created by Frank  on 2/12/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//

#ifndef AST_hpp
#define AST_hpp

#include "llvm/ADT/APFloat.h"
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
#include <stdio.h>
#include <string>
#include <vector>
class ExprAST{
public:
    virtual ~ExprAST(){}
    virtual Value* codegen() = 0;
};

class ProgramAST: public ExprAST{
    std::vector<ExprAST*> programStmts;
public:
    ProgramAST(std::vector<ExprAST*> stmts):programStmts(stmts){}
};

class BlockAST: public ExprAST{
    std::vector<ExprAST*> statements;
public:
    BlockAST(std::vector<ExprAST*> stmts):statements(stmts){};
    void addStmt(ExprAST* newast){
        statements.push_back(newast);
    }
};

class NumberAST: public ExprAST{
    int value;
public:
    NumberAST(const int v):value(v){}
    Value* codegen() override;
};

class VariableAST: public ExprAST{
    std::string name;
public:
    VariableAST(const std::string & n):name(n){}
    Value* codegen() override;
};

//AST to parse the operation between two variables
class BinopAST: public ExprAST{
    std::string op;
    ExprAST* lva,* rva; //store two number
public:
    BinopAST(std::string o,ExprAST* l,ExprAST* r):op(o),lva(l),rva(r){}
    Value* codegen() override;
};

//the prototype of function prototype:  funcname(args)
class ProtoAST: public ExprAST{
    std::string name;
    std::vector<ExprAST*> args; //store parameters of function
public:
    ProtoAST(std::string n,std::vector<ExprAST*> a):name(n),args(a){}
    std::string getName(){
        return name;
    }
    Function* codegen();
};

class FunctionAST: public ExprAST{
    ProtoAST * proto;
    BlockAST * block;
public:
    FunctionAST(ProtoAST* p, BlockAST* b):proto(p),block(b){}
    Function* codegen();
};

//AST to parse the function call
class CallfuncAST: public ExprAST{
    std::string callname; //the funciton name
    std::vector<ExprAST*> args; //define the parameters of function
public:
    CallfuncAST(const std::string cal, std::vector<ExprAST*> &ar):callname(cal),args(ar){}
    Value* codegen() override;
};

//AST to store while statement

class WhileAST: public ExprAST{
    ExprAST * cond;
    ExprAST * body;
public:
    WhileAST(ExprAST* c, ExprAST*b):cond(c),body(b){}
};

//AST to store if statement

class IfAST: public ExprAST{
    ExprAST * cond;
    ExprAST * body;
    ExprAST * elsebody;
public:
    IfAST(ExprAST* c,ExprAST* b,ExprAST* e):cond(c),body(b),elsebody(e){}
};

//AST to store for statement
class ForcallAST: public ExprAST{
    ExprAST * initVar;
    ExprAST * cond;
    ExprAST * Iterator;
    ExprAST * body;
public:
    ForcallAST(ExprAST*init, ExprAST*c, ExprAST* it,ExprAST*bo):initVar(init),cond(c),Iterator(it),body(bo){}
};

//AST to store assignment

class AssignAST: public ExprAST{
    ExprAST* lst;
    ExprAST* rst;
public:
    AssignAST(ExprAST *l,ExprAST* r):lst(l),rst(r){}
};

class DeclareAST: public ExprAST{
    VariableAST * var;
    ExprAST * value;
public:
    DeclareAST(VariableAST*v,ExprAST* vl):var(v),value(vl){}
    DeclareAST(VariableAST*v):var(v){}
};


#endif /* AST_hpp */
