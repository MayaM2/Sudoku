CC = gcc
OBJS = main.o Game.o IOmod.o Solver.o Parser.o Structs.o
EXEC = sudoku
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Game.h Enums.h Parser.h Structs.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c  Solver.h Enums.h Structs.h IOmod.h
	$(CC) $(COMP_FLAG) -c $*.c
Solver.o: Solver.c Enums.h Structs.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Parser.o: Parser.c Enums.h Structs.h
	$(CC) $(COMP_FLAG) -c $*.c
IOmod.o: IOmod.c Structs.h Enums.h
	$(CC) $(COMP_FLAG) -c $*.c
Structs.o: Structs.c Structs.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
