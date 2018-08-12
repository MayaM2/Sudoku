#include <stdlib.h>
#include <stdio.h>
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
	if ((unsigned)(num-lower) <= (upper-lower)){
		return 1;
	}
	return 0;
}

/* returns 1 if validation passed, 0 otherwise*/
/* will print to user only if int printMessages == 1*/
int validate(int** board,int blockHeight, int blockWidth, int dim, int printMessages)
{
	return 0;
}

/*
 * goes over all cells in board. returns 1 if all cells are filled,
 * 0 otherwise.
 */
int allfilled(int** board, int dim){
	for (int i = 0; i < dim; i++){
	  for (int j = 0; j < dim; j++){
		  if(board[i][j] ==0)
			  return 0;
	  }
	}
	return 1;
}

/*
 * "Hidden" functions- Freer frees old memory, Creator calls freer and creates new.
 * The reason for the split is that sometimes freer needs to be called alone (eg- exit command)
 */
void MainMemoryFreer()
{
	/*freeing if needed*/
	if(board!=NULL)
		free(board);
	if(fixed!=NULL)
		free(fixed);
	if(solvedBoard !=NULL)
		free(solvedBoard);
	if(undoRedo!=NULL)
		undoRedoDestroyer(undoRedo);
}
void MainMemoryCreator()
{
	int i=0;
	MainMemoryFreer();
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
void OpenFileHelper(char* fileName)
{
	LoadFileList *li;
	li=LFLCreator();
	if(openFile(li,fileName,gameMode)==1)
	{
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
	else
		printf("Error: File cannot be opened\n");
	LFLDestructor(li);
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
 *doCommand - checks type of command using switch case. Checks validity and prints error message if needed, else - calls corresponding function.
 */
void doCommand(Command* inpCommand){
	int i=0;
switch(inpCommand->commands){

	case SOLVE_COMMAND:
		gameMode=SOLVE;
		OpenFileHelper(inpCommand->fileName);
		break;

	case EDIT_COMMAND:
		gameMode=EDIT;
		markErrors=1;
		OpenFileHelper(inpCommand->fileName);

		/*not needed - openfile deals with null fileName*/
		/*
		if(inpCommand->fileName ==NULL){
			for (int i = 0; i < dim; i++)
			  for (int j = 0; j < dim; j++){
				  board[i][j] = 0;
			  }

			printBoard(board, fixed, gameMode, markErrors, blockHeight, blockWidth, dim);
			break;
		}
		else{
		}
		*/

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
					if(gameMode==EDIT && validate(board, blockHeight,blockWidth, dim, 0)!=1)
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
		printBoard(board, fixed, gameMode, markErrors, blockHeight, blockWidth, dim);
		break;

	case SET_COMMAND: /*ALMOST DONE*/
		if(gameMode == EDIT ||gameMode == SOLVE){
			if(inpCommand->validity==1){ /* checks if any integers were given as X Y Z */
				if(numInRange(inpCommand->arg1,1,dim)&&numInRange(inpCommand->arg2,1,dim)){ /* checks if X Y Z are in range 1-N, N=dim */
					if(fixed[inpCommand->arg1-1][inpCommand->arg2-1]==1){
						printf("Error: cell is fixed\n");
						break;
					}
					/* in this case, we can set the board! */
					undoRedoAppend(undoRedo,(inpCommand->arg1)-1,(inpCommand->arg2)-1,
							board[(inpCommand->arg1)-1][(inpCommand->arg2)-1],inpCommand->arg3,0);
					board[(inpCommand->arg1)-1][(inpCommand->arg2)-1]=inpCommand->arg3;
					/* board print may be executed by main... */
					printBoard();
					if(allfilled(board,dim)==1){ /* call allfilled to check if all board cells are filled */
					/*IN THIS CASE NO NEED FOR ILP SOLUTION... MEANWHILE WILL USE VALIDATE*/
						if(validate(board, blockHeight,blockWidth, dim, 0)==1){ /*validate board*/
							printf("Puzzle solved successfully\n"); /* case board is filled and valid - end of game */
							gameMode=INIT;
						}
						else{
							printf("Puzzle solution erroneous\n"); /*case board is filled and not valid */
						}
					}
				}	// end of numbers in range
				else{ // numbers are not in range
					printf("Error: value not in range 0-%d\n",dim);
				}
			}
		}
		else{ /* not in edit or solve mode */
			printf("ERROR: invalid command\n");
		}
		break;

	case VALIDATE_COMMAND:
		/*use ILP */
		validate(board, blockHeight,blockWidth, dim, 1);

		break;
	case GENERATE_COMMAND:
		/*use ILP */
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
					if(undoRedo->curr->isAutofilled==1 && undoRedo->curr->next->isAutofilled==1)
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
					if(undoRedo->curr->next!=NULL && undoRedo->curr->isAutofilled==1 && undoRedo->curr->next->isAutofilled==1)
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
		/*use ILP */
		break;

	case NUMSOLUTIONS_COMMAND:
		/* will use ROI's exhustive backtracking */
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
				if(validate(board, blockHeight,blockWidth, dim, 0)==1){ /*validate board*/
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
}

/*
int generate();

int hint();
*/



