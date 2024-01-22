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
	toks.o \
	IMap.o \
	index.o


#OPT=-Os
#kOPT=-Ofast
OPT=

DBG=-g
#DBG=

#PRF=-pg
PRF=

CPPFLAGS= $(DBG) $(OPT) $(PRF) --std=c++17

a.out: $O
	g++ $(CPPFLAGS) $O


$O: makefile defs.h

index.o: cell.h

CellList.o: cell.h CellList.h

CellStack.o: cell.h CellStack.h

clause.o: cell.h index.h clause.h

Engine.o: Engine.h

Imap.o: Imap.h

index.o: index.h

IntMap.o: IntMap.h

main.o: toks.h prog.h

Object.o: Object.h

prog.o: Engine.h prog.h

spine.o: spine.h

toks.o: toks.h cell.h


clean:
	rm $O a.out errs gmon.out
