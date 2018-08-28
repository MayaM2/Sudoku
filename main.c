#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Enums.h"
#include "Structs.h"
#include "Game.h"
#include "Parser.h"

int main()
{
	Command* currCommand = commandCreator(-1,-1,-1,-1,-1);/*external command struct which will represent the current command*/

	srand(time(NULL));
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stderr,NULL,_IONBF,0);
	setvbuf(stdin,NULL,_IONBF,0);
	printf("Sudoku\n------\n");
	while(currCommand->commands!=EXIT_COMMAND)
	{
		ParseCommand(currCommand);
		if(doCommand(currCommand)==FATAL_ERROR)
			return 0;
	}

	commandDestructor(currCommand); /*free memory of current (and only) command*/
	return 1;
}


