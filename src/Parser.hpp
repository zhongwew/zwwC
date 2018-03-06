//
//  Parser.hpp
//  zwwC
//
//  Created by Frank  on 2/12/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include "Scanner.hpp"
#include "AST.hpp"
class Parser{
    std::vector<Token*> tokens;
    int pos;  //to indicate the position of current Token
    int lineNum; //used to record line number
public:
    Parser(std::vector<Token*> t){
        this->tokens = t;
        pos = 0;
    }
    Token* readNext(){
        return tokens[pos+1];
    }
    void toNext(){
        pos++;
    }
    void match(std::string value){
        if(value != getToken()->getvalue()){
            std::cout << "match error at: "<<pos+1<<" value: "<<getToken()->getvalue()<<std::endl;
            exit(0);
        }
        toNext();
    }
    Token* getToken(){
        return tokens[pos];
    }
public:
    //parser function
    ExprAST* parseProgram();
    ExprAST* parseStateList();
    ExprAST* parseState();
    ExprAST* parseAssign();
    ExprAST* parseDeclare();
    ExprAST* parseBlock();
    ExprAST* parseArray();
    CallfuncAST* parseFuncCall();
    //parse different kinds of statements
    ExprAST* parseIF();
    ExprAST* parseWhile();
    ExprAST* parseFor();
    ExprAST* parseDef();
    ExprAST* parseReturn();
    //parse variable and numbers
    VariableAST* parseVariable();
    NumberAST* parseNumber();
    /*
     Bool -> HRel Bool'
     Bool' -> L_logic_op HRel Bool'| .
     HRel -> Rel HRel'
     HRel -> H_logic_op Rel HRel' | .
     Rel -> Exp Rel'
     Rel' -> judge_op Exp Rel' | .
     Exp -> Term Exp'
     Exp' -> Add Term Exp' | .
     Term -> F Term'
     Term' -> Mul F Term' | .
     F -> (Bool) | !(Bool) | ID | Number | boolean
     */
    ExprAST* parseBool();
    ExprAST* parseLBool(ExprAST* lvalue);
    ExprAST* parseHRel();
    ExprAST* parseLHRel(ExprAST* lvalue);
    ExprAST* parseRel();
    ExprAST* parseLRel(ExprAST* lvalue);
    ExprAST* parseExp();
    ExprAST* parseLExp(ExprAST* lvalue);
    ExprAST* parseTerm();
    ExprAST* parseLTerm(ExprAST* lvalue);
    ExprAST* parseF();
};


#endif /* Parser_hpp */
