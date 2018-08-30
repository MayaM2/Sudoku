#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Enums.h"
#include "Structs.h"
#include "IOmod.h"
#include "Solver.h"

int **board=NULL;
int **fixed=NULL;
int**solvedBoard=NULL;
int blockHeight;
int blockWidth;
int dim;
int gameMode=INIT;
int markErrors=1;
UndoRedoList *undoRedo=NULL;

/*
 *  isErroneous- "hidden" func. checks if non-zero value at <col,row> exists in it's row, column, or block.
 *  returns 1 if so, 0 else.
 */
int isErroneous(int col, int row)
{
	int i=0, j=0;
	/* go over row */
	i=0;
	for(;i<dim;i++){
		if(i==col)
			continue;
		if(board[row][i]!=0 && board[row][i]==board[row][col])
			return 1;
	}

	/* go over col */
	i=0;
	for(;i<dim;i++){
		if(i==row)
			continue;
		if(board[i][col]!=0 && board[i][col]==board[row][col])
			return 1;
	}

	/*go over block*/
	i=(row/blockHeight)*blockHeight;
	for(;i<(row/blockHeight)*blockHeight+blockHeight;i++){
		j=(col/blockWidth)*blockWidth;
		for(;j<(col/blockWidth)*blockWidth+blockWidth;j++){
			if(i==row && j==col)
				continue;
			if(board[i][j]!=0 && board[i][j]==board[row][col])
				return 1;
		}
	}
	return 0;
}

/*
 *  isBoardErroneous- "hidden" func. checks if there are erroneous values in the board using isErroneous.
 *  returns 1 if so, 0 else.
 */
int isBoardErroneous()
{
	int i=0,j=0;
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			if(isErroneous(j,i)==1)
				return 1;
	return 0;
}

/*
 *  printBoard- "hidden" func that prints board per instructions.
 */
void printBoard()
{
	int i=0,j=0;
	for(;i<dim;i++){
		if(i%blockHeight==0){
			for(j=0;j<4*dim+blockHeight+1;j++)
				printf("-");
			printf("\n");
		}
		j=0;
		for(;j<dim;j++){
			if(j%blockWidth==0)
				printf("|");
			if(board[i][j]==0)
				printf("    ");
			else{
				printf(" %2d",board[i][j]);
				if(fixed[i][j]==1 && gameMode==SOLVE)
					printf(".");
				else{
					if( (gameMode==EDIT || markErrors==1) && isErroneous(j,i)==1)
						printf("*");
					else
						printf(" ");
				}
			}
			if(j==dim-1)
				printf("|\n");

		}
	}
	for(j=0;j<4*dim+blockHeight+1;j++)
		printf("-");
	printf("\n");
}

/*
 * numInRange - "hidden" function that returns 1 if a given num is between
 * lower and upper given numbers inclusively. 0 therwise.
 */
int numInRange(int num, int lower, int upper){
	if (num>=lower && num<=upper){
		return 1;
	}
	return 0;
}

/*
 * allFilled - "hidden" function that goes over all cells in board. returns 1 if all cells are filled,
 * 0 otherwise.
 */
int allFilled(){
	int i=0,j=0;
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			if(board[i][j]==0){
				return 0;
			}
		}
	}
	return 1;
}

/*
 * isBoardEmpty - "hidden" fucntion that returns 1 if board is empty, 0 otherwise
 */
int isBoardEmpty(){
	int i=0,j=0;
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			if(board[i][j]!=0){
				return 0;
			}
		}
	}
	return 1;
}


/* validate - "hidden" function that validates the board by calling ILPSolver.
 * Returns 1 if board validation passed, 0 otherwise.
 * Will print message to user only if printMessages argument is 1*/
int validate(int printMessage)
{
	if(gameMode == SOLVE || gameMode==EDIT){
		int passed = 0;
		if(isBoardErroneous()){
			if(!allFilled() && printMessage ==1){ /* only print if board is not all filled */
				printf("Error: board contains erroneous values\n");
			}
			return 0;
		}
		passed = ILPSolver(board,fixed,solvedBoard,blockHeight,blockWidth,dim);
		if(passed==1){
			if(printMessage){
				printf("Validation passed: board is solvable\n");
			}
			return 1;
		}
		/*case there was a runtime-problem - ILPSolver prints error massege and
		 * we will return from current doCommand call. */
		else if (passed == 0){
			return 0;
		}
		else{
			if(printMessage){
				printf("Validation failed: board is not solvable\n");
			}
			return 0;
		}

	}
	else{
		printf("ERROR: invalid command\n");
		return 0;
	}
}

/*
 * Memory commands- pertaining to main board, fixed board, solved board, and undoRedo List, these 2 funcs
 * create and free the memory needed. Memory Creator returns 1 if all is good, FATAL_ERROR if calloc failed.
 */
void MainMemoryFreer()
{
	int i=0;
	/*freeing if needed*/
	if(board!=NULL){
		for(i=0;i<dim;i++)
			free(board[i]);
		free(board);
	}
	if(fixed!=NULL){
		for(i=0;i<dim;i++)
			free(fixed[i]);
		free(fixed);
	}
	if(solvedBoard !=NULL){
			for(i=0;i<dim;i++)
				free(solvedBoard[i]);
			free(solvedBoard);
	}
	if(undoRedo!=NULL)
		undoRedoDestroyer(undoRedo);
}

int MainMemoryCreator()
{
	int i=0,ret=1;
	/*new allocation*/
	board=(int**)calloc(dim,sizeof(int*));
	fixed=(int**)calloc(dim,sizeof(int*));
	solvedBoard=(int**)calloc(dim,sizeof(int*));
	undoRedo=undoRedoCreator(dim);
	if(board==NULL || fixed==NULL || solvedBoard==NULL)
		ret= FATAL_ERROR;
	for(i=0;i<dim && ret!=FATAL_ERROR;i++){
		board[i]=(int*)calloc(dim,sizeof(int));
		fixed[i]=(int*)calloc(dim,sizeof(int));
		solvedBoard[i]=(int*)calloc(dim,sizeof(int));
		if(board[i]==NULL || fixed[i]==NULL || solvedBoard[i]==NULL)
		{
			ret=FATAL_ERROR;
			break;
		}
	}
	return ret;
}
/*
 * END OF MEMORY COMMANDS
 */

/*
 * OpenFileHelper- "hidden" func that opens file at fileName for edit/solve commands.
 */
int OpenFileHelper(char* fileName)
{
	LoadFileList *li;
	int ret=1, ofres=0;
	li=LFLCreator();
	if(li==NULL)
	{
		printErrorMessage("calloc");
		return FATAL_ERROR;
	}
	ofres=openFile(li,fileName,gameMode==SOLVE?1:0);
	if(ofres==1)
	{
		MainMemoryFreer();
		blockHeight=li->rowsPerBlock;
		blockWidth=li->colsPerBlock;
		dim=blockWidth*blockHeight;
		if(MainMemoryCreator()==FATAL_ERROR)
		{
			printErrorMessage("calloc");
			return FATAL_ERROR;
		}
		li->curr=li->head;
		while(li->curr!=NULL)
		{
			board[li->curr->row][li->curr->col]=li->curr->val;
			fixed[li->curr->row][li->curr->col]=li->curr->isFixed;
			li->curr=li->curr->next;
		}
		undoRedoAppend(undoRedo,board);
	}
	else{
			if(gameMode==EDIT)
				printf("Error: File cannot be opened\n");
			else
				printf("Error: File doesn't exist or cannot be opened\n");
			ret= 0;
	}
	LFLDestructor(li);
	return ret;
}

/*
 * printsForRedoUndo- "hidden" func. takes care of correct printing for undo redo using these parameters.
 * 			isUndo- 1 if undo, 0 if redo
 * 			row- current row
 * 			col- current col
 * 			num1- number at cell before change
 * 			num2- number at cell after change
 */
void printsForRedoUndo(int isUndo, int row, int col, int num1, int num2)
{
	printf("%s %d,%d: from ",isUndo==1?"Undo":"Redo",col,row);
	if(num1==0)
		printf("_");
	else
		printf("%d",num1);
	printf(" to ");
	if(num2==0)
		printf("_");
	else
		printf("%d",num2);
	printf("\n");
}


/*
 * problemCellAssignment - "hidden" function being used by randomFill function.
 * Deals with situations were first random assignment of value to board[i][j] was not valid,
 * which caused a problem with filling the board. Tries to fill cell randomly.
 * Returns 1 if successful random value assignment was found
 * 0 otherwise (case all possible values cause an erroneous board).
 */
int problemCellAssignment(int i, int j){
	int numOptions = dim;
	int x=0,count=0,ind=0;
	int* OpArr = (int*)calloc(dim,sizeof(int));
	/* Initialize OpArr with ones*/
	for(; x<dim;x++){
		OpArr[x]=1;
	}

	while(numOptions>0){ /* while there are still value options to try*/
		x = rand() % numOptions; /* x between 0 and (numOptions-1)*/
		/* we will want to go to the x'th index in OpArr that has value 1*/
		count = 0;
		ind=0;
		while(count < x){
			if(OpArr[ind]==1){
				count++;
			}
			ind++; /* we need i+1 which represents the actual value*/
		}
		ind++;
		/* try and put value in cell*/
		board[i][j]=ind;
		if(!isBoardErroneous()){
			free(OpArr);
			return 1; /* successful*/
		}
		/* case not successful*/
		OpArr[ind-1]=0; /* delete option*/
		numOptions--;
	}
	free(OpArr);
	return 0; /* we went through all options for values, and they were all unsuccessful*/
}


/*
 * randomFill- "hidden" function used by generate function.
 * Recieves int X, and two int arrays in length X, that represent row and col coordinates
 * of X different cells in the game board. Tries to randomly fill these cells with valid values.
 * If reached a cell that has no legal value available - mission failed - return 0.
 * otherwise - a valid assignment was found - return 1.
 */
int randomFill(int X,int *arri,int *arrj){
	int k=0, count=0, status=0;
	while(count<X){
		k = (rand() % dim)+1; /* first try any random number for cell, from range 1-dim*/
		board[arri[count]][arrj[count]]=k;
		if(isBoardErroneous()){ /* there was a problem with the first try of cell's assignment*/
			status = (problemCellAssignment(arri[count],arrj[count])); /* try randomly all options.*/
			if(status==0){
				return 0; /* reached a dead end*/
			}
		}
		count++;
	}
	/* case all X cells were filled*/
	return 1;
}


/*
 * generate - "hidden" function that generates a new puzzle out of an empty board.
 * Randomly choose x cells. Run ILPSolver (uo tp 1000 times if needed).
 * randomly choose y cells and delete them.
 * update the game board
 * return 1 if succeeded, 0 otherwise.
 */
int generate(int X, int Y){
	int tries=0,i=0,j=0;
	int step1Success = 0;
	int cellAssignSuccesss = 0;
	int randCount = 0;
	int* arri = (int*)calloc(X,sizeof(int));
	int* arrj = (int*)calloc(X,sizeof(int));
	int* arri2 = (int*)calloc(Y,sizeof(int)); /* for step 2 */
	int* arrj2 = (int*)calloc(Y,sizeof(int));
	/*step 1: up to 1,000 tries: choose X cells, fill them randomly with valid values and try to solve using ILP*/
	while(!step1Success && tries<1000){
		randCount = 0;
		cellAssignSuccesss = 0;
		while(randCount<X){
			arri[randCount] = rand() % dim; /* [i][j] coordinated are between  0 - (dim-1)*/
			arrj[randCount] = rand() % dim;
			if(board[arri[randCount]][arrj[randCount]]==0){ /*cell was not already chosen..*/
				board[arri[randCount]][arrj[randCount]]=1;
				randCount++;
			}
		}/*now X cells were randomly chosen*/

		/*re clean board*/
		for(i=0; i<dim; i++){
			for(j=0;j<dim; j++){
				board[i][j]=0;
			}
		}

		/*next - we'll try to fill them randomly with valid values*/
		if(randomFill(X,arri,arrj)){ /*try to randomly fill chosen cells with legal values*/
			cellAssignSuccesss = 1; /* case succeed*/
		}
		else{ /* case failed - wipe out the board back*/
			for(i=0; i<dim; i++){
				for(j=0;j<dim; j++){
					board[i][j]=0;
				}
			}
		}

		/* if we were able to assign all X cells with valid values - we need to make sure the board is solvable with ILP*/
		if(cellAssignSuccesss){
			/*update fixedBoard*/
			for(i=0;i<dim;i++){
				for(j=0;j<dim;j++){
					fixed[i][j]=0; /* make sure fixed is empty */
					if(board[i][j]!=0){
						fixed[i][j]=1;
					}
				}
			}

			if(ILPSolver(board,fixed,solvedBoard,blockHeight,blockWidth,dim)){/* if there is a solution*/
				step1Success = 1;
			}
		}
		else{
			tries++;
		}
	}

	if(!step1Success){ /* case while loop stopped after 1,000 unsuccessful iterations*/
		free(arri);
		free(arrj);
		free(arri2);
		free(arrj2);
		return 0;
	}
	/* continue to step 2: erase Y randomly chosen cells and clear their value. Print board,
	 make board the solvedBoard*/
	for(i=0;i<dim;i++){
		for(j=0; j<dim; j++){
			board[i][j] = solvedBoard[i][j];
			fixed[i][j]=0; /*clean fixed */
		}
	}

	/*choose Y random cells to clean*/
	randCount = 0;
	while(randCount<Y){
		arri2[randCount] = rand() % dim; /* [i][j] coordinated are between  0 - (dim-1)*/
		arrj2[randCount] = rand() % dim;
		if(!(board[arri2[randCount]][arrj2[randCount]]==0)){ /*cell was not already chosen..*/
			board[arri2[randCount]][arrj2[randCount]]=0;
			randCount++;
		}
	}
	free(arri);
	free(arrj);
	free(arri2);
	free(arrj2);
	return 1;
}

/*
 * All the following are "hidden" functions, which doCommand (at the bottom) calls to do the actual commands.
 */

void resetCommand()
{
	int i=0,j=0;
	UndoRedoList *dummy;
	if(gameMode==SOLVE || gameMode==EDIT){
		undoRedo->curr=undoRedo->head;
		if(undoRedo->head->next!=NULL)
		{
			dummy=undoRedoCreator(dim);
			dummy->head=undoRedo->head->next;
			undoRedo->head->next=NULL;
			dummy->head->prev=NULL;
			undoRedoDestroyer(dummy);
		}
		for(i=0;i<dim;i++)
			for(j=0;j<dim;j++)
				board[i][j]=undoRedo->head->nodeBoard[i][j];
		printf("Board reset\n");
		printBoard();
	}
	else{
		printf("ERROR: invalid command\n");
	}
}

void saveCommand(char* fileName)
{
	if(gameMode==INIT)
		printf("ERROR: invalid command\n");
	else{
			if(gameMode==EDIT && isBoardErroneous()==1)
				printf("Error: board contains erroneous values\n");
			else
			{
				if(gameMode==EDIT && validate(0)!=1)
					printf("Error: board validation failed\n");
				else
				{
					if(saveFile(board,fixed,fileName,gameMode,blockHeight,blockWidth)==1)
						printf("Saved to: %s\n",fileName);
					else
						printf("Error: File cannot be created or modified\n");
				}
			}
		}
}

void setCommand(Command *inpCommand)
{
	int row=0,col=0, append=0;
	if(gameMode == EDIT ||gameMode == SOLVE){
			if((inpCommand->validity==1)&& numInRange(inpCommand->arg1,1,dim)&&numInRange
					(inpCommand->arg2,1,dim)&&numInRange(inpCommand->arg3,0,dim)){ /* checks if X Y Z are in range 1-N, N=dim */
				row=inpCommand->arg2-1;
				col=inpCommand->arg1-1;
				if(fixed[row][col]==1 && gameMode==SOLVE){
					printf("Error: cell is fixed\n");
					return;
				}
				/* in this case, we can set the board! */
				if(board[row][col]!=inpCommand->arg3)
					append=1;
				board[row][col]=inpCommand->arg3;
				if(append==1)
					undoRedoAppend(undoRedo,board);
				/* board print may be executed by main... */
				printBoard();
				if(allFilled()){ /* call allFilled to check if all board cells are filled */

					if(validate(0)==1){ /*validate board*/
						printf("Puzzle solved successfully\n"); /* case board is filled and valid - end of game */
						gameMode=INIT;
					}
					else{
						printf("Puzzle solution erroneous\n"); /*case board is filled and not valid */
					}
				}
			}	/* end of numbers in range*/
			else{ /* numbers are not in range*/
				printf("Error: value not in range 0-%d\n",dim);
			}
	}
	else{ /* not in edit or solve mode */
		printf("ERROR: invalid command\n");
	}
}

/*Makes validation checks only, the generation itself is executed by generate funciton. */
void generateCommand(Command* inpCommand)
{
	int i=0,j=0;
	if(gameMode == EDIT){
		if(!isBoardEmpty()){
			printf("Error: board is not empty\n");
			return;
		}
		/* checks if any integers were given as X Y, and wheather they are in the right range */
		if((inpCommand->validity==1)&&numInRange(inpCommand->arg1,0,dim*dim)&&numInRange(inpCommand->arg2,0,dim*dim)){ /* checks if X Y Z are in range 0-Dim*Dim */
			/* in this case, we can generate a new board!! */
			if(!generate(inpCommand->arg1,inpCommand->arg2)){ /*generation process*/
				for(i=0;i<dim;i++){ /* if fails - reclean board*/
					for(j=0;j<dim;j++){
						board[i][j]=0;
					}
				}
				printf("Error: puzzle generator failed\n");
			}
			else{/*puzzle generation successful*/
				printBoard();
				undoRedoAppend(undoRedo,board);
			}
		}	/* end of numbers in range*/
		else{ /* numbers are not in range*/
			printf("Error: value not in range 0-%d\n",dim*dim);
			return;
		}
	}
	else{/* not in edit or solve mode */
		printf("ERROR: invalid command\n");
	}
}

void undoCommand()
{
	int i=0,j=0;
	if(gameMode==INIT)
		printf("ERROR: invalid command\n");
	else
	{
		/*if dummy node at head, nothing to undo*/
		if(undoRedo->curr->prev==NULL)
			printf("Error: no moves to undo\n");
		else
		{
			undoRedo->curr=undoRedo->curr->prev;
			for(i=0;i<dim;i++)
				for(j=0;j<dim;j++)
					if(undoRedo->curr->nodeBoard[i][j]!=undoRedo->curr->next->nodeBoard[i][j])
					{
						board[i][j]=undoRedo->curr->nodeBoard[i][j];
						printsForRedoUndo(1,i+1,j+1,
								undoRedo->curr->next->nodeBoard[i][j],
								undoRedo->curr->nodeBoard[i][j]);
					}
		}
	}
}

void redoCommand()
{
	int i=0,j=0;
	if(gameMode==INIT)
		printf("ERROR: invalid command\n");
	else
	{
		if(undoRedo->curr->next==NULL)
			printf("Error: no moves to redo\n");
		else
		{
			undoRedo->curr=undoRedo->curr->next;
			for(i=0;i<dim;i++)
				for(j=0;j<dim;j++)
					if(undoRedo->curr->nodeBoard[i][j]!=undoRedo->curr->prev->nodeBoard[i][j])
					{
						board[i][j]=undoRedo->curr->nodeBoard[i][j];
						printsForRedoUndo(0,i+1,j+1,
								undoRedo->curr->prev->nodeBoard[i][j],
								undoRedo->curr->nodeBoard[i][j]);
					}
		}
	}
}

void hintCommand(Command* inpCommand)
{
	int row=0,col=0,i=0;
	if(gameMode == SOLVE){
		 /* checks if any integers were given as X Y Z */
			if(!((inpCommand->validity==1)&&(numInRange(inpCommand->arg1,1,dim)&&numInRange(inpCommand->arg2,1,dim)))){ /* checks if X Y Z are integers in range 1-N, N=dim*/
				printf("Error: value not in range 1-%d\n",dim);
				return;
			} /* end of numbers not in range*/
			if(isBoardErroneous()){ /* case board is erroneous - don't execute*/
				printf("Error: board contains erroneous values\n");
				return;
			}
			row=inpCommand->arg2-1;
			col=inpCommand->arg1-1;
			if(fixed[row][col]==1){  /* case cell is fixed - don't execute*/
				printf("Error: cell is fixed\n");
				return;

			}
			if(board[row][col]!=0){ /* case cell is not empty - don't execute*/
				printf("Error: cell already contains a value\n");
				return;
			}
			i=ILPSolver(board,fixed,solvedBoard,blockHeight,blockWidth,dim);
			if(i==-1){ /*case solution is infinite or unbounded*/
				printf("Error: board is unsolvable\n");
				return;
			}
			/*case there was a runtime-problem - ILPSolver prints error massege and
			 * we will return from current hintCommand call. */
			else if (i == 0){
				return;
			}
			else{
				printf("Hint: set cell to %d\n",solvedBoard[row][col]);
				return;
			}
		}
	else{/* not in edit or solve mode */
		printf("ERROR: invalid command\n");
	}
}
/*
 * END OF HIDDEN DOCOMMAND HELPERS
 */

/*
 * doCommand - checks type of command using switch case.
 * Checks validity and prints error message if needed,
 * else - calls corresponding function to execute the given command.
 */
int doCommand(Command* inpCommand){
	int sols=0,res=0,ret=0;
	setvbuf(stdin,NULL,_IONBF,0);
	setvbuf(stdout,NULL,_IONBF,0);
switch(inpCommand->commands){

	case SOLVE_COMMAND:
		gameMode=SOLVE;
		res=OpenFileHelper(inpCommand->fileName);
		if(res!=1)
		{
			if(res==0)
				gameMode=INIT;
			else if(res==FATAL_ERROR)
				ret=FATAL_ERROR;
		}
		break;

	case EDIT_COMMAND:
		gameMode=EDIT;
		res=OpenFileHelper(inpCommand->fileName);
		if(res!=1)
		{
			if(res==0)
				gameMode=INIT;
			else if(res==FATAL_ERROR)
				ret=FATAL_ERROR;
		}
		break;

	case RESET_COMMAND: /*ALMOST DONE*/
		resetCommand();
		break;


	case EXIT_COMMAND: /*Available at ant time and mode*/
		/*FREE MEMORY*/
		printf("Exiting...\n");
		MainMemoryFreer();
		break;

	case SAVE_COMMAND:
		saveCommand(inpCommand->fileName);
		break;

	case MARKERRORS_COMMAND: /*ALMOST DONE*/
		if(gameMode==SOLVE){
			if(inpCommand->arg1==0 || inpCommand->arg1==1){
						markErrors=inpCommand->arg1;
			}
			else{ /* case we are in solve mode but the input is invalid */
				printf("Error: the value should be 0 or 1\n");
			}
		}
		else{ /* case we are not in solve mode */
			printf("ERROR: invalid command\n");
		}
		break;

	case PRINTBOARD_COMMAND: /*DONE*/

		if(gameMode!=INIT){

			printBoard();
		}
		else
			printf("ERROR: invalid command\n");
		break;

	case SET_COMMAND: /*ALMOST DONE*/
		setCommand(inpCommand);
		break;

	case VALIDATE_COMMAND: /*DONE*/
		validate(1);
		break;

	case GENERATE_COMMAND:
		generateCommand(inpCommand);
		break;

	case UNDO_COMMAND:
		undoCommand();
		break;

	case REDO_COMMAND:
		redoCommand();
		break;

	case HINT_COMMAND:
		hintCommand(inpCommand);
		break;

	case NUMSOLUTIONS_COMMAND:
		if(gameMode==INIT)
			printf("ERROR: invalid command\n");
		else

		{
			if(isBoardErroneous()==1)
				printf("Error: board contains erroneous values\n");
			else
			{
				sols=numSols(board,blockHeight,blockWidth);
				if(sols==FATAL_ERROR)
				{
					printErrorMessage("calloc");
					ret=FATAL_ERROR;
					break;
				}
				printf("Number of solutions: %d\n",sols);
				if(sols!=0)
				{
					if(sols==1)
						printf("This is a good board!\n");
					else
						printf("The puzzle has more than 1 solution, try to edit it further\n");
				}
			}

		}
		break;

	case AUTOFILL_COMMAND:
		if(gameMode!=SOLVE)
			printf("ERROR: invalid command\n");
		else
		{
			if(isBoardErroneous()==1)
				printf("Error: board contains erroneous values\n");
			else{
				ret=Autofill(board, blockHeight, blockWidth);
				if(ret==FATAL_ERROR)
				{
					printErrorMessage("calloc");
					ret=FATAL_ERROR;
					break;
				}
				if(ret==1)
					undoRedoAppend(undoRedo,board);
				printBoard();
				if(allFilled() && validate(0)==1){ /*validate board*/
					printf("Puzzle solved successfully\n"); /* case board is filled and valid - end of game */
					gameMode=INIT;
				}
			}
		}
		break;

	case INVALID_COMMAND: /*DONE*/
		printf("ERROR: invalid command\n");
		break;

	case IGNORE_COMMAND:
		break;

	default :
		break;
}
return ret;
}

