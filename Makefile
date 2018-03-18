#=========================================================================
#FILE:         Makefile
#
#AUTHOR:       Zhongwei Wang
#
#DESCRIPTION:  This file constains commands that comile the ZwwC compiler
#=========================================================================


all:
	@-rm -rf bin
	@-mkdir bin
	@cd bin
	@clang++ -g -O3 src/main.cpp src/AST.cpp src/Parser.cpp src/Scanner.cpp `llvm-config --cxxflags --ldflags --system-libs --libs all` -o bin/zwwC
