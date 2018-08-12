/*
 * Solver.c
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#include <stdlib.h>
#include <stdio.h>
#include "Enums.h"
#include "Structs.h"

// Gurobi variables and declarations :
#include "gurobi_c.h"
extern int **board;
extern int**fixed;
extern int**solvedBoard;
extern int blockHeight;
extern int blockWidth;
extern int dim;
extern int gameMode;
extern int markErrors;

/*
 * "Hidden" func- check neighbors. if only one val is possible, return it, else 0;
 */
int isObv(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin){
	int i=0, j=0, val=0,dim=blockHeight*blockWidth;
	int count=dim;

	for(i=0;i<dim;i++)
		neighborsBin[i]=0;

	/* go over row */
	i=0;
	for(;i<dim;i++){
		val=board[row][i];
		if(val==0)
			continue;
		if(neighborsBin[val-1]==0){
			neighborsBin[val-1]=1;
			count--;
		}
	}

	/* go over col */
	i=0;
	for(;i<blockHeight*blockWidth;i++){
		val=board[i][col];
		if(val==0)
			continue;
		if(neighborsBin[val-1]==0){
			neighborsBin[val-1]=1;
			count--;
		}
	}

	/*go over block*/
	i=(row/blockHeight)*blockHeight;
	for(;i<(row/blockHeight)*blockHeight+blockHeight;i++){
		j=(col/blockWidth)*blockWidth;
		for(;j<(col/blockWidth)*blockWidth+blockWidth;j++){
			val=board[i][j];
			if(val==0)
				continue;
			if(neighborsBin[val-1]==0){
				neighborsBin[val-1]=1;
				count--;
			}
		}
	}

	if(count==1)
	{
		for(i=0;i<dim;i++)
			if(neighborsBin[i]==0)
				return i+1;
	}
	return 0;
}

void Autofill(int** board, UndoRedoList *urli, int blockHeight, int blockWidth)
{
	int i=0,j=0, val=0, appended=0, dim=blockHeight*blockWidth, *neighborsBin;
	neighborsBin=(int*)calloc(dim,sizeof(int));
	for(i=0;i<dim;i++)
	{
		for(j=0;j<dim;j++)
		{
			if(board[i][j]==0)
			{
				val=isObv(board,i,j,blockHeight,blockWidth,neighborsBin);
				if(val!=0){
					undoRedoAppend(urli,i,j,0,val,1);
					if(appended==0)
						appended=1;
				}
			}
		}
	}
	if(appended==1)
	{
		while(urli->curr->prev->isAutofilled==1)
			urli->curr=urli->curr->prev;
		while(urli->curr!=NULL)
		{
			board[urli->curr->row][urli->curr->col]=urli->curr->newVal;
			printf("Cell <%d,%d> set to %d\n",urli->curr->row+1,urli->curr->col+1,urli->curr->newVal);
			urli->curr=urli->curr->next;
		}
		urli->curr=urli->tail;
	}
}

/*
 * Will use Gurobi Optimizer to try and solve current board, and under the constrains
 * of the Sudoku game rules and the fixed numbers.
 * Will return 1 if there is a solution, -1 if there is no solution, or 0 if there was a runtime problem.
 * Will update extern int** SolvedBoard accordingly.
 */
int ILPSolver(void){

	GRBenv *env = NULL; //initialize environment
	GRBmodel *model = NULL; //initialize model
	// initialize program variables
	int error = 0;
	double sol[dim*dim*dim];
	int ind[dim*dim*dim];
	int val[dim*dim*dim];
	int obj[dim*dim*dim]; //coeffs of obj. function
	char vtype[dim*dim*dim];
	int optimstatus;
	double objval;
	int zero=0;
	int i=0;

	/*Create environment*/
	error = GRBloadenv(&env,""); //TODO - find out about logfiles in gurobi.
	//here no log file will be written since an empty string was given.
	if (error || env == NULL){
		printf("Error %d : in GRBloadenv: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/*Create empty model*/
	error = GRBnewmodel(env,&model,"ILPSolve",0,NULL,NULL,NULL,NULL,NULL);
	if (error || env == NULL){
		printf("Error %d : in GRBnewmodel: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/*Add variables*/
	for(i=0; i<dim*dim*dim;i++){
		obj[i]=1;
		vtype[i]=GRB_BINARY;
	}
	error = GRBaddvars(model,dim*dim*dim,0,NULL,NULL,NULL,obj,NULL,NULL,vtype,NULL);
	if(error){
		printf("Error %d : in GRBaddvars: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/*change objective sense to maximization - default is min*/
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if(error){
		printf("Error %d : in GRBsetintattr: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/*Add constrains: there are four kinds: rows, cols, block, fixed*/
	/*Add row constraints*/

	/*Add col constraints*/

	/*Add block constraints*/

	/*Add fixed-cell constraints*/

	/*Optimize model*/
	error = GRBoptimize(model);
	if(error){
		printf("Error %d : in GRBoptimize: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	/*Capture solution information*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS,&optimstatus); //query the status of the optimization process
	// by retrieving the values of the status attribute.
	if(error){
		printf("Error %d : in GRBgetintattr: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if(error){
		printf("Error %d : in GRBgetdblattr: %s\n", error, GRBgeterrormsg(env));
		return 0;
	}



	/*Free model*/
	GRBfreemodel(model); // free model memory
	GRBfreeenv(env); // free environment memory
	return 0;
}


