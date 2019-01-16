# zwwC
This project constructs a simple compiler. It's a self-motivate project finished in winter 2017. The whole project is based on C++. I implemented lexical analysis, grammar parsing and code generation. The code generation used Low Level Virtual Machine(LLVM).


## Lexical Analysis

To develop a new language, the first thing we should do is to define keywords and all the operators in the form of regular expression.

	Letter = [a-zA-Z]

	Num = [0-9]

To make the language simple, we can only store two types of data sturcture currently.  Number will be stored as double.

	ID = [Letter|_(Letter|Num)*]

	String = ["(Letter|Num)*"]

	Number = [0-9(Num)*]


And I define a lot of keywords.  These keywords are reserved and cannot be used as name of variables. 

	Keywords = {set, main, for, while, if, else, put, get, return}

	boolean = {true, false}

Operators are pre-divided into groups by its priority:(name pattern: priority_type_op)

	self_op = {++, --}

	H_logic_op = {&&}

	L_logic_op = {||}

	H_math_op = {* | / | %}

	L_math_op = {+ , -}

	judge_op = {== , >=, <=, =, !=, <,>}

## Grammar definition

I define some basic grammar of this language, where . represents an empty. 

	The entrance of program is 

		Program -> State-List

	The grammer of statement:

		State-List -> State State-List | .

		State -> Declare | Assign | Func 

		State -> If(Bool) { State-List} EExp

		EExp -> else {State-List} | .

		State -> for(Assign; Bool; Assign) {State-List}

		State -> while(Bool){State-List}

		State -> def ID (IDList) {State-List}
		
		State-> {State State-List}

		State -> return ID;

	Assignment and Declare grammer:

		Assign -> Array Assign' | self_op ID

		Assign' -> = Bool | self_op

		Declare -> set Array Assign'| .

		Array -> ID | ID[Number] | ID[*ID] 
	
	Logic operation and math calculation, ordered by priority:

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

##