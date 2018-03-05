//
//  scanner.h
//  zwwC
//
//  Created by Frank  on 1/25/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//
#include <vector>
#include <iostream>
#include <cctype>
#include <sstream>
#include <fstream>
#ifndef scanner_h
#define scanner_h

//tag of reserved keywords
const int T_SET = 1, T_MAIN = 3, T_FOR = 4, T_IF = 5, T_ELSE = 6, T_RETURN = 7,T_TERMINATE = 8,T_WHILE = 9,T_DEF=10;

//tag of operators
const int T_SELFOP = 30, T_HLOGICOP = 31, T_LLOGICOP = 32, T_HMATHOP = 33, T_LMATHOP = 34, T_JUDGEOP = 35, T_EQUAL = 36, T_L_SBRAC = 37, T_R_SBRAC = 38, T_L_BBRAC = 39,T_R_BBRAC = 40,T_L_RBRAC = 41,T_R_RBRAC = 42;

//tag of user defined data
const int T_NUM = 50, T_ID = 60, T_STRING = 70, T_BOOLEAN = 80;

class Token{
    std::string value;
    int type;
public:
    Token(int t, const std::string v):value(v),type(t){}
    Token(Token * t){
        value = t->getvalue();
        type = t->getType();
    }
    Token(int t, int numV){
        type = t;
        value = std::to_string(numV);
    }
    int getNumvalue(){
        int number = 0;
        if (this->type == T_NUM) {
            for (int i = 0; i<value.length(); i++) {
                number = number*10 + value[i]-'0';
            }
            return number;
        }
        else{
            std::cout<<"error"<<std::endl;
            return false;
        }
    }
    const std::string getvalue(){
        return value;
    }
    int getType(){
        return type;
    }
};
class Scanner{
    char ptr; //a pointer to scan through file
    std::vector<Token*> TokenList;
    std::vector<Token*> wordsTable; //store the reserved Tokens and defined Tokens
public:
    Scanner(){//push reserved words in table
        wordsTable.push_back(new Token(T_IF, "if"));
        wordsTable.push_back(new Token(T_BOOL, "bool"));
        wordsTable.push_back(new Token(T_FOR, "for"));
        wordsTable.push_back(new Token(T_INT, "int"));
        wordsTable.push_back(new Token(T_MAIN, "main"));
        wordsTable.push_back(new Token(T_RETURN, "return"));
        wordsTable.push_back(new Token(T_ELSE, "else"));
        wordsTable.push_back(new Token(T_DEF, "def"));
    }
    void scan(std::ifstream & infile);
    //function to scan through infile
    bool readNext(std::ifstream & infile, char c);
    //to determine if next char is c
    void printToken(){
        for (int i = 0; i<TokenList.size(); i++) {
            std::cout<<"<"<<TokenList[i]->getType()<<",";
            if (TokenList[i]->getType() == T_NUM) {
                std::cout<<TokenList[i]->getNumvalue()<<"> ";
            }
            else{
                std::cout<<TokenList[i]->getvalue()<<"> ";
            }
        }
    }
    std::vector<Token*> getList(){
        return TokenList;
    }
};


#endif /* scanner_h */
