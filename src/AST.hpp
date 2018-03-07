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
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include <stdio.h>
#include <string>
#include <vector>
using namespace llvm;


static LLVMContext TheContext; 
static IRBuilder<> Builder(TheContext); //help to generate instructions
static llvm::Module* TheModule = new llvm::Module("my cool jit", TheContext); //contains funcitons and global values
static std::map<std::string, AllocaInst *> NamedValues; //a symbol table to track
//a helper function that ensure the alloca is created in block of function
static AllocaInst* CreateEntryBlockAlloca(Function* thefunc,const std::string & varName){
    IRBuilder<> TmpB(&thefunc->getEntryBlock(),thefunc->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type::getDoubleTy(TheContext),0,varName.c_str());
}


class ExprAST{
public:
    virtual ~ExprAST(){}
    virtual llvm::Value* codegen() = 0;
};

class ProgramAST: public ExprAST{
    std::vector<ExprAST*> programStmts;
public:
    ProgramAST(std::vector<ExprAST*> stmts):programStmts(stmts){}
    llvm::Function* codegen() override;
};

class BlockAST: public ExprAST{
    std::vector<ExprAST*> statements;
public:
    BlockAST(std::vector<ExprAST*> stmts):statements(stmts){};
    void addStmt(ExprAST* newast){
        statements.push_back(newast);
    }
    llvm::Value* codegen() override;
};

class NumberAST: public ExprAST{
    double value;
public:
    NumberAST(const double v):value(v){}
    double getValue(){
        return value;
    }
    llvm::Value* codegen() override;
};

class VariableAST: public ExprAST{
    std::string name;
public:
    VariableAST(const std::string & n):name(n){}
    llvm::Value* codegen() override;
    std::string getName(){
        return name;
    }
};

//AST to parse the operation between two variables
class BinopAST: public ExprAST{
    std::string op;
    ExprAST* lva,* rva; //store two number
public:
    BinopAST(std::string o,ExprAST* l,ExprAST* r):op(o),lva(l),rva(r){}
    llvm::Value* codegen() override;
};

//the prototype of function prototype:  funcname(args)
class ProtoAST: public ExprAST{
    std::string name;
    std::vector<std::string> args; //store parameters of function
public:
    ProtoAST(std::string n,std::vector<std::string> a):name(n),args(a){}
    std::string getName(){
        return name;
    }
    llvm::Function* codegen();
};

class FunctionAST: public ExprAST{
    ProtoAST * proto;
    ExprAST * block;
public:
    FunctionAST(ProtoAST* p, ExprAST* b):proto(p),block(b){}
    std::string getFuncname(){
        return proto->getName();
    }
    llvm::Function* codegen();
};

//AST to parse the function call
class CallfuncAST: public ExprAST{
    std::string callname; //the funciton name
    std::vector<ExprAST*> args; //define the parameters of function
public:
    CallfuncAST(const std::string cal, std::vector<ExprAST*> &ar):callname(cal),args(ar){}
    llvm::Value* codegen() override;
};

//AST to store while statement

class WhileAST: public ExprAST{
    ExprAST * cond;
    ExprAST * body;
public:
    WhileAST(ExprAST* c, ExprAST*b):cond(c),body(b){}
    llvm::Value* codegen() override;
};

//AST to store if statement

class IfAST: public ExprAST{
    ExprAST * cond;
    ExprAST * body;
    ExprAST * elsebody;
public:
    IfAST(ExprAST* c,ExprAST* b,ExprAST* e):cond(c),body(b),elsebody(e){}
    llvm::Value* codegen() override;
};

//AST to store for statement
class ForcallAST: public ExprAST{
    ExprAST * initVar;
    ExprAST * cond;
    ExprAST * Iterator;
    ExprAST * body;
public:
    ForcallAST(ExprAST*init, ExprAST*c, ExprAST* it,ExprAST*bo):initVar(init),cond(c),Iterator(it),body(bo){}
    llvm::Value* codegen() override;
};

//AST to store assignment

class AssignAST: public ExprAST{
    VariableAST* lst;
    ExprAST* rst;
public:
    AssignAST(VariableAST *l,ExprAST* r):lst(l),rst(r){}
    std::string getVarname(){
        return lst->getName();
    }
    llvm::Value* codegen();
};

class DeclareAST: public ExprAST{
    VariableAST * var;
    ExprAST * value;
public:
    DeclareAST(VariableAST*v,ExprAST* vl):var(v),value(vl){}
    DeclareAST(VariableAST*v):var(v){}
    std::string getVarname(){
        return var->getName();
    }
    llvm::Value* codegen();
};


#endif /* AST_hpp */
