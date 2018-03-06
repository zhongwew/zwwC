//
//  scanner.cpp
//  zwwC
//
//  Created by Frank  on 1/29/18.
//  Copyright © 2018 Zhongwei Wang. All rights reserved.
//

#include <stdio.h>
#include "Scanner.hpp"

bool Scanner::readNext(std::ifstream & infile, char c){
    infile>>ptr;
    if (c == ptr) {
        infile >> ptr;
        return true;
    }
    else{
        return false;
    }
}

void Scanner::scan(std::ifstream & infile){
    //std::vector<Token*> TokenList;
    infile.get(ptr);
    while(!infile.eof()){
        //handle the space and special char
        if (ptr == ' ' || ptr == '\t' || ptr == '\n') {
            infile.get(ptr);
            continue;
        }
        //handle the number
        if (isnumber(ptr)) {
            int number = 0;
            while (isnumber(ptr)) {
                number = number * 10 + ptr-'0';
                infile.get(ptr);
            }
            TokenList.push_back(new Token(T_NUM,number));
            if (isalpha(ptr)) {
                std::cout<<"number define error"<<std::endl;
            }
            continue;
        }
        
        //handle the operators
        switch (ptr) {
            case '!':
                if (readNext(infile, '=')) {
                    TokenList.push_back(new Token(T_JUDGEOP,"!="));
                }
                else{
                    TokenList.push_back(new Token(T_SELFOP,"!"));
                }
                break;
            case '+':
                if (readNext(infile, '+')) {
                    TokenList.push_back(new Token(T_SELFOP,"++"));
                }
                else{
                    TokenList.push_back(new Token(T_LMATHOP,"+"));
                }
                break;

            case '-':
                if (readNext(infile, '-')) {
                    TokenList.push_back(new Token(T_SELFOP,"--"));
                }
                else{
                    TokenList.push_back(new Token(T_LMATHOP,"-"));
                }
                break;

            case '*':
                TokenList.push_back(new Token(T_HMATHOP,"*"));
                break;

            case '/':
                TokenList.push_back(new Token(T_HMATHOP,"/"));
                infile.get(ptr);
                break;

            case '%':
                TokenList.push_back(new Token(T_HMATHOP,"%"));
                infile.get(ptr);
                break;

            case '&':
                if (readNext(infile, '&')) {
                    TokenList.push_back(new Token(T_HLOGICOP, "&&"));
                }
                else{
                    std::cout << "error" <<std::endl;
                }
                break;

            case '|':
                if (readNext(infile, '|')) {
                    TokenList.push_back(new Token(T_LLOGICOP, "||"));
                }
                else{
                    std::cout << "error" <<std::endl;
                }
                break;

            case '<':
                if (readNext(infile, '=')) {
                    TokenList.push_back(new Token(T_JUDGEOP,"<="));
                }
                else{
                    TokenList.push_back(new Token(T_JUDGEOP,"<"));
                }
                break;

            case '>':
                if (readNext(infile, '=')) {
                    TokenList.push_back(new Token(T_JUDGEOP,">="));
                }
                else{
                    TokenList.push_back(new Token(T_JUDGEOP,">"));
                }
                break;
            case '=':
                if (readNext(infile, '=')) {
                    TokenList.push_back(new Token(T_JUDGEOP,"=="));
                }
                else{
                    TokenList.push_back(new Token(T_EQUAL,"="));
                }
                break;
            case '(':
                TokenList.push_back(new Token(T_L_RBRAC,"("));
                infile.get(ptr);
                break;
            case ')':
                TokenList.push_back(new Token(T_R_RBRAC,")"));
                infile.get(ptr);
                break;
            case '{':
                TokenList.push_back(new Token(T_L_BBRAC,"{"));
                infile.get(ptr);
                break;
            case '}':
                TokenList.push_back(new Token(T_R_BBRAC,"}"));
                infile.get(ptr);
                break;
            case '[':
                TokenList.push_back(new Token(T_L_SBRAC,"["));
                infile.get(ptr);
                break;
            case ']':
                TokenList.push_back(new Token(T_R_SBRAC,"]"));
                infile.get(ptr);
                break;
            case ';':
                TokenList.push_back(new Token(T_TERMINATE,";"));
                infile.get(ptr);
                break;

            default:
                break;
        }
        
        //==========handle the string=================================
        if (isalpha(ptr)) {
            //using tempstr to get the whole string
            std::ostringstream tempstr;
            while (isalnum(ptr)) {
                tempstr << ptr;
                infile.get(ptr);
            }
            std::string str = tempstr.str();
            bool flag = 1;
            for (int i = 0; i<wordsTable.size(); i++) {
                if (wordsTable[i]->getvalue() == str) {
                    //the string equals to reserved word
                    TokenList.push_back(new Token(wordsTable[i]));
                    flag = 0;
                    break;
                }
            }
            if (flag) {//no tokens in table match string
                TokenList.push_back(new Token(T_ID,str));
            }
        }
        //===================================================
    }
    
    return;
}
