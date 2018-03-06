//
//  Parser.cpp
//  zwwC
//
//  Created by Frank  on 2/12/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//

#include "AST.hpp"

static LLVMContext TheContext; 
static IRBuilder<> Builder(TheContext); //help to generate instructions
static llvm::Module* TheModule; //contains funcitons and global values
static std::map<std::string, llvm::Value *> NamedValues; //a symbol table to track

std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s\n", Str);
  return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

llvm::Value* VariableAST::codegen(){
    Value * v = NamedValues[name]; //check if variable has been define in the table
    if(!v) LogErrorV("unknown variable name");
    return v;
}

llvm::Value* NumberAST::codegen(){
    return ConstantFP::get(TheContext,APFloat(this->getValue())); //default to be float type
}

llvm::Value* BinopAST::codegen(){
   llvm::Value* lv = lva->codegen();
   llvm::Value* rv = rva->codegen();
   if(!lv || !rv) return nullptr;
   //different operators
   if(op == "+") 
        return Builder.CreateFAdd(lv,rv,"addtmp");
    else if(op == "-")
        return Builder.CreateFSub(lv,rv,"subtmp");
    else if(op == "*")
        return Builder.CreateFMul(lv,rv,"multmp");
    else if(op == "/")
        return Builder.CreateFDiv(lv,rv,"divtmp");
    else if(op == "%")
        return Builder.CreateFRem(lv,rv,"remtmp");
    else if(op == "<"){
        lv = Builder.CreateFCmpULT(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else if(op == "<="){
        lv = Builder.CreateFCmpULE(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else if(op == ">"){
        lv = Builder.CreateFCmpUGT(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else if(op == ">="){
        lv = Builder.CreateFCmpUGE(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else if(op == "=="){
        lv = Builder.CreateFCmpUEQ(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else if(op == "!="){
        lv = Builder.CreateFCmpUNE(lv,rv,"cmptmp");
        return Builder.CreateUIToFP(lv,Type::getDoubleTy(TheContext),"booltmp");
    }
    else 
        return nullptr;
}

llvm::Value* CallfuncAST::codegen(){
    //look up function name in module first
    llvm::Function * callfunc = TheModule->getFunction(callname);
    if(!callfunc)
        return LogErrorV("Unknown fucntion called");
    //check if arg size is valid
    if(callfunc->arg_size() != args.size())
        return LogErrorV("invalid parameters");
    std::vector<llvm::Value*> argV;
    for(unsigned int i = 0, e = args.size();i<e;i++){
        argV.push_back(args[i]->codegen());
        if(!argV.back())
            return nullptr; //confused...
    }
    return Builder.CreateCall(callfunc,argV,"calltmp");
}

llvm::Function* ProtoAST::codegen(){
    //declare a function
    std::vector<Type*> Doubles(args.size(),Type::getDoubleTy(TheContext));
    //Define the funciton type:double take Doubles as args and return double
    FunctionType * FT = FunctionType::get(Type::getDoubleTy(TheContext),Doubles,false);
    //create function
    Function * f = Function::Create(FT,Function::ExternalLinkage,name,TheModule);
    //set name for args
    unsigned int index = 0;
    for(auto & arg: f->args())
    //this is not correct
        arg.setName(args[index++]);
    return f;
}

llvm::Function* FunctionAST::codegen(){
    llvm::Function * thefunc = TheModule->getFunction(proto->getName());
    //check if function has been declared somewhere
    if(!thefunc)
        thefunc = proto->codegen();
    if(!thefunc)
        return nullptr;
    if(!thefunc->empty())
        return (llvm::Function*)LogErrorV("function can't be redefined");
    //create a basic block to start insertion into
    BasicBlock * bb = BasicBlock::Create(TheContext,"entry",thefunc);
    Builder.SetInsertPoint(bb);
    //record the function args
    NamedValues.clear();
    for(auto & arg : thefunc->args())
        NamedValues[arg.getName()] = &arg;
    if(llvm::Value* retval = block->codegen()){
        //the return value is generated successfully
        Builder.CreateRet(retval);
        //check the consistency
        verifyFunction(*thefunc);
        return thefunc;
    }
    //for the error case
    thefunc->eraseFromParent();
    return nullptr;
}

llvm::Function* ProgramAST::codegen(){
    // for(int i = 0;i<programStmts.size();i++)
    //     programStmts[i]->codegen();
    //make an anonymous function
    std::vector<std::string> tempStr;
    auto proto = new ProtoAST("anonymous",tempStr);
    BlockAST * tempb = new BlockAST(programStmts);
    auto anoy_func = new FunctionAST(proto,tempb);
    return anoy_func->codegen();
}

llvm::Value* BlockAST::codegen(){
    for(int i=0;i<statements.size();i++)
        statements[i]->codegen();
}

llvm::Value* IfAST::codegen(){
    llvm::Value* condv = cond->codegen();
    if(!condv)
        return nullptr;
    //convert condv into boolean by compare it with 0.0
    condv = Builder.CreateFCmpONE(condv,ConstantFP::get(TheContext,APFloat(0.0)),"ifcond");
    //get the current fucntion that is being built
    Function * thefunc = Builder.GetInsertBlock()->getParent();
    //create blocks for the other cases
    BasicBlock * thenb = BasicBlock::Create(TheContext,"then",thefunc);
    BasicBlock * elsebb = BasicBlock::Create(TheContext,"else");
    BasicBlock * Mergeb = BasicBlock::Create(TheContext,"ifcont");

    Builder.CreateCondBr(condv,thenb,elsebb);
    //emit then value
    Builder.SetInsertPoint(thenb);
    llvm::Value* thenv = body->codegen();
    if(!thenv)
        return nullptr;
    Builder.CreateBr(Mergeb);
    //todo what is phi? phi is a function choose the specific block for the control flow
    thenb = Builder.GetInsertBlock();
    //emit else block, similar with above
    thefunc->getBasicBlockList().push_back(elsebb);
    Builder.SetInsertPoint(elsebb);
    llvm::Value* elsev = elsebody->codegen();
    if(!elsev)
        return nullptr;
    Builder.CreateBr(Mergeb);
    elsebb = Builder.GetInsertBlock();
    //emit the merge block
    thefunc->getBasicBlockList().push_back(Mergeb);
    Builder.SetInsertPoint(Mergeb);
    PHINode * pn = Builder.CreatePHI(Type::getDoubleTy(TheContext),2,"iftmp");
    pn->addIncoming(thenv,thenb);
    pn->addIncoming(elsev,elsebb);
    return pn;
}

llvm::Value* ForcallAST::codegen(){
    //initialize variable code generation
    llvm::Value * initv = initVar->codegen();
    DeclareAST* varPtr = dynamic_cast<DeclareAST*>(initVar);
    AssignAST* itePtr = dynamic_cast<AssignAST*>(Iterator);
    std::string varName = varPtr->getVarname();
    std::string iteName = itePtr->getVarname();
    if(!initv)
        return nullptr;
    //make new basic block for loop header
    Function* thefunc = Builder.GetInsertBlock()->getParent();
    BasicBlock *preheaderbb = Builder.GetInsertBlock();
    BasicBlock * loopbb = BasicBlock::Create(TheContext,"loop",thefunc);
    //insert an explicit fall 
    Builder.CreateBr(loopbb);
    //start insertion in loopbb
    Builder.SetInsertPoint(loopbb);
    //start the phi node with an entry for start
    //todo incorrect
    PHINode * var = Builder.CreatePHI(Type::getDoubleTy(TheContext),2,varName.c_str());
    var->addIncoming(initv,preheaderbb);
    //within the loop save the old value and use new value to cover it 
    llvm::Value * oldv = NamedValues[varName];
    NamedValues[varName] = var;
    if(!body->codegen())
        return nullptr;
    //emit the iterator value
    llvm::Value * iterValue = nullptr;
    if(iterValue){
        iterValue = Iterator->codegen();
        if(!iterValue)
            return nullptr;
    }else{
        //if the iterator is not specified
        iterValue = ConstantFP::get(TheContext,APFloat(1.0));
    }
    llvm::Value * nextVar = Builder.CreateFAdd(var,iterValue,"nextvar");
    //compute the condition value
    llvm::Value * condv = cond->codegen();
    if(!condv)
        return nullptr;
    //convert it to a boolean
    condv = Builder.CreateFCmpONE(condv,ConstantFP::get(TheContext,APFloat(0.0)),"loopcond");
    //create the after loop block
    BasicBlock * loopendbb = Builder.GetInsertBlock();
    BasicBlock * afterbb = BasicBlock::Create(TheContext,"afterloop",thefunc);
    //insert the conditional branch into the end of loopendbb
    Builder.CreateCondBr(condv,loopbb,afterbb);
    //set insert point to afterbb
    Builder.SetInsertPoint(afterbb);
    //add a new entry to the phi node for the backedge
    //todo can't understand it...
    var->addIncoming(nextVar,loopendbb);
    //restore the unshadowed variable
    if(oldv)
        NamedValues[iteName] = oldv;
    else
        NamedValues.erase(iteName);
    //return 0.0
    return Constant::getNullValue(Type::getDoubleTy(TheContext));
}

llvm::Value* WhileAST::codegen(){
}

llvm::Value* DeclareAST::codegen(){
    Value * tempv;
    std::string varName = var->getName();
    if(NamedValues[varName])
        return LogErrorV("duplicate variable");
    NamedValues[varName] = tempv;
    //if has right value:emit the right value first
    if(value){
        Value* val = value->codegen();
        if(!val)
            return nullptr;
        Builder.CreateStore(val,NamedValues[varName]);
        return val;
    }
    return tempv;

}

llvm::Value* AssignAST::codegen(){
    std::string varName = lst->getName();
    if(NamedValues[varName])
        return LogErrorV("duplicate variable");
    Value* var = NamedValues[varName];
    Value * rvalue = rst->codegen();
    if(rvalue)
        return nullptr;
    Builder.CreateStore(rvalue,var);
    return rvalue;
}