# zwwC
This project constructs a simple compiler. 

## Update Schedule
* Week 3

	Learning the implementation of Compiler through an example
		

		Kaleidoscope: Tutorial Introduction and the Lexer
		
		Ref:https://llvm.org/docs/tutorial/OCamlLangImpl1.html
	
		https://www.tutorialspoint.com/compiler_design/compiler_design_lexical_analysis.htm

*  Week 4

	finish lexical scanner, generate all the tokens.
	
	Input: Character stream 
	
	Output: Tokens stream 

*  Week 6

	implement parser and AST
	
	Input: Tokens stream
	
	Output: AST

*  Week 7

	Generate IR using LLVM IR API
	
	Input: AST
	
	Output: IR

*  Week 8

	Using LLVM to generate .o file

## Lexical Analysis

To develop a new language, the first thing we should do is to define keywords and all the operators in the form of regular expression.

	Letter = [a-zA-Z]

	Num = [0-9]

To make the language simple, we can only store two types of data sturcture currently.  Number will be stored as double.

	Variable = [Letter|_(Letter|Num)*]

	String = ["(Letter|Num)*"]

	Number = [0-9(Num)*]

And I define a lot of keywords.  These keywords are reserved and cannot be used as name of variables.  More keywords may be defined in the future's work to expand the compiler's function.

	Keywords = {for, if, else, let, put, get, Begin, End}

	Operator = {+, -, *, /, =, ==, !=, >, <, >=, <=, ||, &&,}

## Grammar definition

I define some basic grammar of this language, where . represents an empty:

		Program -> Begin State-List End

		State-List -> State; State-List | .

		State_List -> If State-List EExp

		EExp -> Else State-List | .

		State-List -> For {State-List}

		State -> Assign | Func

		Assign -> let ID = Exp

		Exp -> Term Exp'

		Exp' -> Add Term Exp' | .

		Term -> F Term'

		Term' -> Mul F Term' | .

		F -> (Exp) | ID | Number

		Add -> + | -

		Mul -> * | /

		Func -> put(Put)
	
		Put -> ID | Number | String

		Func -> get(ID) 
