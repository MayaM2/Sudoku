#include <stdlib.h>
#include <stdio.h>
#include "Enums.h"
#include "Structs.h"
#include "Game.h"
#include "Parser.h"
/*
#include "Solver.h"
#include "IOmod.h"
*/

int main()
{
	Command* currCommand = commandCreator(-1,-1,-1,-1,-1);/*external command struct which will represent the current command*/

	srand(5);
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stderr,NULL,_IONBF,0);
	setvbuf(stdin,NULL,_IONBF,0);

	while(currCommand->commands!=EXIT_COMMAND)
	{
		ParseCommand(currCommand);
		doCommand(currCommand);
	}

	commandDestructor(currCommand); /*free memory of current (and only) command*/
	return 1;
}


