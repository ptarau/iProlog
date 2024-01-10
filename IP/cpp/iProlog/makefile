O= \
	CellList.o \
	CellStack.o \
	clause.o \
	engine.o \
	IntMap.o \
	main.o \
	Object.o \
	prog.o \
	spine.o \
	test.o \
	toks.o


OPT=-Ofast
#DBG=-g
DBG=
CPPFLAGS= $(DBG) $(OPT) --std=c++17

$O: makefile \
	cell.h \
	CellList.h \
	CellStack.h \
	clause.h \
	defs.h \
	Engine.h \
	file2string.h \
	IMap.h \
	Integer.h \
	IntMap.h \
	Object.h \
	prog.h \
	spine.h \
	toks.h

a.out: $O
	g++ $O

