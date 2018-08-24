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

int isBoardErroneous()
{
	int i=0,j=0;
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			if(isErroneous(j,i)==1)
				return 1;
	return 0;
}

void printBoard()
{
	int i=0,j=0;
	setvbuf(stdout,NULL,_IONBF,0);
	for(;i<dim;i++){
		if(i%blockHeight==0){
			for(j=0;j<4*dim+blockWidth+1;j++)
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
				if(fixed[i][j]==1)
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
	for(j=0;j<4*dim+blockWidth+1;j++)
		printf("-");
	printf("\n");
}

/*
 * returns 1 if a given num is between lower and upper given numbers inclusively.
 * 0 therwise.
 */
int numInRange(int num, int lower, int upper){
	if (num>=lower && num<=upper){
		return 1;
	}
	return 0;
}

/*
 * goes over all cells in board. returns 1 if all cells are filled,
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
 * Returns 1 if board is empty, 0 otherwise
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


/* returns 1 if validation passed, 0 otherwise
 * will print to user only if int printMessages == 1*/
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
 * "Hidden" functions- Freer frees old memory, Creator calls freer and creates new.
 * The reason for the split is that sometimes freer needs to be called alone (eg- exit command)
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
void MainMemoryCreator()
{
	int i=0;
	/*new allocation*/
	board=(int**)calloc(dim,sizeof(int*));
	fixed=(int**)calloc(dim,sizeof(int*));
	solvedBoard=(int**)calloc(dim,sizeof(int*));
	undoRedo=undoRedoCreator();
	for(i=0;i<dim;i++){
		board[i]=(int*)calloc(dim,sizeof(int));
		fixed[i]=(int*)calloc(dim,sizeof(int));
		solvedBoard[i]=(int*)calloc(dim,sizeof(int));
	}
}

/*
 * "Hidden" function- for both openfile commands (solve and edit)
 */
int OpenFileHelper(char* fileName)
{
	LoadFileList *li;
	int ret=1, ofres=0;
	li=LFLCreator();
	ofres=openFile(li,fileName,gameMode==SOLVE?1:0);
	if(ofres==1)
	{
		MainMemoryFreer();
		blockHeight=li->rowsPerBlock;
		blockWidth=li->colsPerBlock;
		dim=blockWidth*blockHeight;
		MainMemoryCreator();
		li->curr=li->head;
		while(li->curr!=NULL)
		{
			board[li->curr->row][li->curr->col]=li->curr->val;
			fixed[li->curr->row][li->curr->col]=li->curr->isFixed;
			li->curr=li->curr->next;
		}
	}
	else{
		if(ofres==0)
		{
			printf("Error: File cannot be opened\n");
			ret= 0;
		}
		/*FATAL ERROR*/
		else if (ofres==FATAL_ERROR)
			return FATAL_ERROR;
	}
	LFLDestructor(li);
	return ret;
}

/*
 * "hidden" func- for prints from redo/undo
 */
void printsForRedoUndo(int isUndo, int row, int col, int num1, int num2)
{
	printf("%s %d,%d: from ",isUndo==1?"Undo":"Redo",row,col);
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
 * to be used by randomFill function.
 * Deals with situations were first random assignment of value to board[i][j] was not valid.
 * returns 1 if successful random value assignment was found
 * 0 otherwise.
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
 * to be used by generate function.
 * recieve int X, and two int arrays in length X, that represent row and col coordinates
 * of X different cells in the game board. try to fill these cells with valid values.
 * if reached a cell that has no legal value available, mission failed - return 0.
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
 * Generates a new puzzle out of an empty board.
 * Randomly choose x cells. Run ILPSolver (uo tp 1000 times if needed)
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
 *doCommand - checks type of command using switch case. Checks validity and prints error message if needed, else - calls corresponding function.
 */
int doCommand(Command* inpCommand){
	int i=0,j=0, sols=0,res=0,ret=0;
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
		markErrors=1;
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
		if(gameMode==SOLVE || gameMode==EDIT){
			/*while node is NOT dummy, move backwards*/
			while(undoRedo->curr->row!=-1){
				board[undoRedo->curr->row][undoRedo->curr->col]=undoRedo->curr->oldVal;
				undoRedo->curr=undoRedo->curr->prev;
			}
			undoRedoDestroyer(undoRedo);
			undoRedo=undoRedoCreator();
			printf("Board reset\n");
			printBoard();
		}
		else{
			printf("ERROR: invalid command\n");
		}
		break;


	case EXIT_COMMAND: /*Available at ant time and mode*/
		printf("Exiting...\n");
		/*FREE MEMORY*/
		MainMemoryFreer();
		break;

	case SAVE_COMMAND:
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
						if(saveFile(board,fixed,inpCommand->fileName,gameMode,blockHeight,blockWidth)==1)
							printf("Saved to: %s\n",inpCommand->fileName);
						else
							printf("Error: File cannot be created or modified\n");
					}
				}
			}
		break;

	case MARKERRORS_COMMAND: /*ALMOST DONE*/
		if(gameMode==SOLVE){
			if(inpCommand->arg1==0 || inpCommand->arg1==1){
						markErrors=inpCommand->arg1;
						/*INSERT mark errors command*/
						/*print board accordingly */
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
		if(gameMode!=INIT)
			printBoard();
		else
			printf("ERROR: invalid command\n");
		break;

	case SET_COMMAND: /*ALMOST DONE*/
		if(gameMode == EDIT ||gameMode == SOLVE){
				if((inpCommand->validity==1)&& numInRange(inpCommand->arg1,1,dim)&&numInRange
						(inpCommand->arg2,1,dim)&&numInRange(inpCommand->arg3,0,dim)){ /* checks if X Y Z are in range 1-N, N=dim */
					if(fixed[inpCommand->arg1-1][inpCommand->arg2-1]==1){
						printf("Error: cell is fixed\n");
						break;
					}
					/* in this case, we can set the board! */
					undoRedoAppend(undoRedo,(inpCommand->arg1)-1,(inpCommand->arg2)-1,
							board[(inpCommand->arg1)-1][(inpCommand->arg2)-1],inpCommand->arg3,0,0);
					board[(inpCommand->arg1)-1][(inpCommand->arg2)-1]=inpCommand->arg3;
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
		break;

	case VALIDATE_COMMAND: /*DONE*/
		validate(1);
		break;

	case GENERATE_COMMAND:
		if(gameMode == EDIT){
			if(!isBoardEmpty()){
				printf("Error: board is not empty\n");
				break;
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
					printBoard(board);
				}
			}	/* end of numbers in range*/
			else{ /* numbers are not in range*/
				printf("Error: value not in range 0-%d\n",dim*dim);
				return 0;
			}
		}
		else{/* not in edit or solve mode */
			printf("ERROR: invalid command\n");
		}
		break;

	case UNDO_COMMAND:
		if(gameMode==INIT)
			printf("ERROR: invalid command\n");
		else
		{
			/*if dummy node at head, nothing to undo*/
			if(undoRedo->curr->row==-1)
				printf("Error: no moves to undo\n");
			else
			{
				while(i==0){
					i++;
					board[undoRedo->curr->row][undoRedo->curr->col]=undoRedo->curr->oldVal;
					printsForRedoUndo(1,undoRedo->curr->row+1,undoRedo->curr->col+1,
							undoRedo->curr->newVal,
							undoRedo->curr->oldVal);
					undoRedo->curr=undoRedo->curr->prev;
					if(undoRedo->curr->row==-1)
						break;
					if(undoRedo->curr->isAutofilled==1 && undoRedo->curr->next->isAutofilled==1 && undoRedo->curr->next->isAutofillStarter==0)
						i--;
				}
			}
		}
		break;

	case REDO_COMMAND:
		if(gameMode==INIT)
			printf("ERROR: invalid command\n");
		else
		{
			if(undoRedo->curr->next==NULL)
				printf("Error: no moves to redo\n");
			else
			{
				while(i==0){
					i++;
					undoRedo->curr=undoRedo->curr->next;
					printsForRedoUndo(0,undoRedo->curr->row+1,undoRedo->curr->col+1,
							undoRedo->curr->oldVal,
							undoRedo->curr->newVal);
					board[undoRedo->curr->row][undoRedo->curr->col]=undoRedo->curr->newVal;
					if(undoRedo->curr->next!=NULL && undoRedo->curr->isAutofilled==1 && undoRedo->curr->next->isAutofilled==1 && undoRedo->curr->next->isAutofillStarter==0)
						i--;
					if(undoRedo->curr->next==NULL)
						break;
				}
				if(undoRedo->curr==NULL)
					undoRedo->curr=undoRedo->tail;
			}
		}
		break;

	case HINT_COMMAND:
		if(gameMode == SOLVE){
			 /* checks if any integers were given as X Y Z */
				if(!((inpCommand->validity==1)&&(numInRange(inpCommand->arg1,1,dim)&&numInRange(inpCommand->arg2,1,dim)))){ /* checks if X Y Z are integers in range 1-N, N=dim*/
					printf("Error: value not in range 0-%d\n",dim);
					return 0;
				} /* end of numbers not in range*/
				if(isBoardErroneous()){ /* case board is erroneous - don't execute*/
					printf("Error: board contains erroneous values\n");
					return 0;
				}
				if(fixed[inpCommand->arg1-1][inpCommand->arg2-1]==1){  /* case cell is fixed - don't execute*/
					printf("Error: cell is fixed\n");
					return 0;

				}
				if(board[inpCommand->arg1-1][inpCommand->arg2-1]!=0){ /* case cell is not empty - don't execute*/
					printf("Error: cell already contains a value\n");
					return 0;
				}
				i=ILPSolver(board,fixed,solvedBoard,blockHeight,blockWidth,dim);
				if(!i){
					printf("Error: board is unsolvable\n");
					return 0;
				}
				else{
					printf("Hint: set cell to %d\n",solvedBoard[inpCommand->arg1-1][inpCommand->arg2-1]);
					return 0;
				}
			}
		else{/* not in edit or solve mode */
			printf("ERROR: invalid command\n");
		}
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
				if(sols!=0)
				{
					printf("Number of solutions: %d\n",sols);
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
				Autofill(board, undoRedo, blockHeight, blockWidth);
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

