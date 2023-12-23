#O=main.o toks.o engine.o prog.o
#GCC="C:/msys64/ucrt64/bin/g++.exe"
GCC="C:/mingw/bin/g++.exe"
O=main.o engine.o prog.o toks.o
#FLAGS=-fpermissive -g
#FLAGS=-g -O2
#FLAGS=-g -DNO_INDEXING
FLAGS=-g

run: $O
	${GCC} ${FLAGS} $O

$O: hhprolog.h defs.h makefile

main.o: main.cpp
	${GCC} -c ${FLAGS} main.cpp

engine.o: engine.cpp
	${GCC} -c ${FLAGS} engine.cpp

prog.o: prog.cpp
	${GCC} -c ${FLAGS} prog.cpp

toks.o: toks.cpp
	${GCC} -c ${FLAGS} toks.cpp


