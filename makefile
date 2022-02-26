CXX = g++
RM = rm -f
CPPFLAGS = -std=c++11 -Wall -g

SCANNAR_FILES = ./lib/Scanner/Token.cpp \
				./lib/Scanner/Scanner.cpp \

PARSER_FILES = ./lib/Parser/ParseError.cpp \
				./lib/Parser/Parser.cpp \
				./lib/Parser/Expression/Expr.cpp \
				./lib/Parser/Expression/Assign.cpp \
				./lib/Parser/Expression/Binary.cpp \
				./lib/Parser/Expression/Grouping.cpp \
				./lib/Parser/Expression/Literal.cpp \
				./lib/Parser/Expression/Unary.cpp \
				./lib/Parser/Expression/Variable.cpp \
				./lib/Parser/Expression/Logical.cpp \
				./lib/Parser/Stmt/Stmt.cpp \
				./lib/Parser/Stmt/Expression.cpp \
				./lib/Parser/Stmt/Print.cpp \
				./lib/Parser/Stmt/Var.cpp \
				./lib/Parser/Stmt/Block.cpp \
				./lib/Parser/Stmt/If.cpp \
				./lib/Parser/Stmt/While.cpp \

TOOLS_FILES = 	./lib/Parser/AstPrinter.cpp \
				./lib/Interpreter/Environment.cpp \
				./lib/Interpreter/Interpreter.cpp \

INTERPRETER_FILES = ./lib/Interpreter/RuntimeError.cpp \
					./lib/Lox.cpp \

SRCS_CPP = \
				./src/main.cpp \

run:
	$(CXX) $(SCANNAR_FILES) $(PARSER_FILES) $(INTERPRETER_FILES) $(TOOLS_FILES) $(SRCS_CPP) -o application $(CPPFLAGS) 

