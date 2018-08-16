/*
 * Solver.c
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Enums.h"
#include "Structs.h"

/* Gurobi variables and declarations :*/
#include "gurobi_c.h"

/*
 * "Hidden" func- updates binary neighbors array.
 */
void updateNeighbors(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin)
{
	int i=0, j=0, val=0,dim=blockHeight*blockWidth;

	for(i=0;i<dim;i++)
		neighborsBin[i]=0;

	/* go over row */
	i=0;
	for(;i<dim;i++){
		val=board[row][i];
		if(val==0)
			continue;
		if(neighborsBin[val-1]==0)
			neighborsBin[val-1]=1;
	}

	/* go over col */
	i=0;
	for(;i<blockHeight*blockWidth;i++){
		val=board[i][col];
		if(val==0)
			continue;
		if(neighborsBin[val-1]==0)
			neighborsBin[val-1]=1;
	}

	/*go over block*/
	i=(row/blockHeight)*blockHeight;
	for(;i<(row/blockHeight)*blockHeight+blockHeight;i++){
		j=(col/blockWidth)*blockWidth;
		for(;j<(col/blockWidth)*blockWidth+blockWidth;j++){
			val=board[i][j];
			if(val==0)
				continue;
			if(neighborsBin[val-1]==0)
				neighborsBin[val-1]=1;
		}
	}
}

/*
 * "Hidden" func- check neighbors. if only one val is possible, return it, else 0;
 */
int isObv(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin)
{
	int i=0, dim=blockWidth*blockHeight,count=dim;
	updateNeighbors(board, row, col,blockHeight, blockWidth, neighborsBin);
	for(i=0;i<dim;i++)
		if(neighborsBin[i]==1)
			count--;
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
					if(appended==0){
						appended=1;
						undoRedoAppend(urli,i,j,0,val,1,1);
					}
					else
						undoRedoAppend(urli,i,j,0,val,1,0);
				}
			}
		}
	}
	if(appended==1)
	{
		while(urli->curr->prev->isAutofilled==1 && urli->curr->isAutofillStarter==0)
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
 * "Hidden" func- return next cell in board, according to direction. -1 -1 denotes trying to leave last cell,
 * -2 -2 denotes trying to go before first cell.
 */
void nextCell(int row, int col, int isForward,int dim, int* indexes){

	/* forward */
	if(isForward==1){
		if(row==dim-1 && col==dim-1){
			indexes[0]=-1;
			indexes[1]=-1;
		}
		else{
			if(col==dim-1){
				indexes[0]=row+1;
				indexes[1]=0;
			}
			else{
				indexes[0]=row;
				indexes[1]=col+1;
			}
		}
	}
	/* backwards*/
	else{
		if(row==0 && col==0){
			indexes[0]=-2;
			indexes[1]=-2;
		}
		else{
			if(col==0){
				indexes[0]=row-1;
				indexes[1]=dim-1;
			}
			else{
				indexes[0]=row;
				indexes[1]=col-1;
			}
		}
	}
}

void demoPrint(int sols, int dim, int** board)
{
	int j=0,k=0;
	printf("sol #%d:\n",sols);
	for(j=0;j<dim;j++)
	{
		for(k=0;k<dim;k++)
		{
			printf("%2d ",board[j][k]);
		}
		printf("\n");
	}
}

int numSols(int** board, int blockHeight, int blockWidth)
{
	int sols=0, i=0, currInd=0, indexes[2]={0,0},row=0,col=0, isForward=1, dim=blockHeight*blockWidth;
	int* bin;
	RecStack *rec = recStackCreator();
	RecStackNode *n;
	bin=(int*)calloc(dim,sizeof(int));
	do
	{
		/*
		 * Forward- find next empty cell. if -1 -1 (meaning, board is FULL), increment sols count, and
		 * prepare for reverse backtracking. else, update neighbors, change cell value into first possible
		 * value, and push into stack.
		 */
		if(isForward==1)
		{
			while(col!=-1 && board[row][col]!=0)
			{
				nextCell(row,col,isForward,dim,indexes);
				row=indexes[0];
				col=indexes[1];
			}
			if(row==-1 && col==-1)
			{
				isForward=0;
				sols++;
			}
			else
			{
				updateNeighbors(board,row,col,blockHeight,blockWidth,bin);
				for(i=0;i<dim;i++)
					if(bin[i]==0)
						break;
				if(i<dim)
				{
					board[row][col]=i+1;
					recStackPushInfo(rec,row,col,isForward,bin,dim);
				}
				else
					isForward=0;

			}

		}
		/*
		 * Backwards- check recStack head for new possible vals. if none exist, remove head, and change val
		 * in board to 0 ( ie- continue backwards). else, place new possible val, and go forwards.
		 */
		else
		{
			n=rec->head;
			row=n->row;
			col=n->col;
			currInd=board[n->row][n->col]-1;
			n->neighborsBin[currInd]=1;
			i=currInd+1;
			for(;i<dim;i++)
				if(n->neighborsBin[i]==0)
					break;
			if(i==dim)
			{
				board[n->row][n->col]=0;
				destroyStackNode(recStackPop(rec));
			}
			else
			{
				board[n->row][n->col]=i+1;
				row=n->row;
				col=n->col;
				isForward=1;
			}
		}
		/*if(currInd>=dim)
		{
			printf("WEEOOWEEEOO!!! %d %d %d %d\n", n->row,n->col,currInd,isForward);
			break;
		}
		printf("set [%d,%d] with %d, going %s\n",row,col,row==-1?0:board[row][col],isForward==1?"Forwards":"Backwards");
		if(board[row][col]>12)
			break;

		printf("head=");
		if(rec->head==NULL)
			printf("NULL\n");
		else
			printf("%d %d\n",rec->head->row,rec->head->col);
		if(row==-1 && isForward==0 && rec->head->row==11 && rec->head->col==6)
			count++;
		if(count>1)
			break;
		*/
	}
	while(rec->head!=NULL);
	if(n!=NULL)
		destroyStackNode(n);
	recStackDestroyer(rec);

	return sols;
}



/*
 * Will use Gurobi Optimizer to try and solve current board, and under the constrains
 * of the Sudoku game rules and the fixed numbers.
 * Will return 1 if there is a solution, -1 if there is no solution, or 0 if there was a runtime problem.
 * Will update extern int** SolvedBoard accordingly.
 */

int ILPSolver(int **board,int**fixed,int**solvedBoard,int blockHeight,int blockWidth, int dim){

	GRBenv *env = NULL; /*initialize environment*/
	GRBmodel *model = NULL; /*initialize model*/
	/* initialize program variables*/
	int error = 0;
	double sol[dim*dim*dim];
	int intSol[dim*dim*dim];
	int ind[dim];
	int val[dim];
	int obj[dim*dim*dim]; //coeffs of obj. function
	char vtype[dim*dim*dim];
	int optimstatus;
	double objval;
	int i=0, j=0, k=0, ii=0,jj=0,count=0;

	/*Create environment*/
	error = GRBloadenv(&env,""); //TODO - find out about logfiles in gurobi.
	/*here no log file will be written since an empty string was given.*/
	if (error || env == NULL){
		printf("Error %d : in GRBloadenv: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	/*Create empty model*/
	error = GRBnewmodel(env,&model,"ILPSolve",0,NULL,NULL,NULL,NULL,NULL);
	if (error || env == NULL){
		printf("Error %d : in GRBnewmodel: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
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
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	/*change objective sense to maximization - default is min*/
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if(error){
		printf("Error %d : in GRBsetintattr: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}


	/*Update model - to integrate new variables*/
	error = GRBupdatemodel(model);
	if(error){
		printf("Error %d : in GRBupdatemodel: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	/*Add constrains: there are five kinds: cells, rows, cols, block, fixed*/

	/*Add cell constraints - for each [i,j] cell, only allow to hold one value*/
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			for(k=0;k<dim;k++){ /*sum variable values over all possible k's for specific [i,j]*/
				ind[k]=i*dim*dim+j*dim+k;
				val[k]=1.0;
			}
			error= GRBaddconstr(model,dim,ind,val,GRB_EQUAL,1.0,NULL);
			if(error){
				printf("Error %d : in GRBaddconstr: %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model); /* free model memory*/
				GRBfreeenv(env); /* free environment memory*/
				return 0;
			} /*end of if error*/
		}/* end of specific  [i,j] cell*/
	}/* go through all possible [i,j]*/


	/*Add row constraints - for each i index*/
	for(k=0;k<dim;k++){
		for(j=0;j<dim;j++){
			for(i=0;i<dim;i++){ /*sum variable values over all the row*/
				ind[i]=i*dim*dim+j*dim+k;
				val[i]=1.0;
			}
			error= GRBaddconstr(model,dim,ind,val,GRB_EQUAL,1.0,NULL);
			if(error){
				printf("Error %d : in GRBaddconstr: %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model); /* free model memory*/
				GRBfreeenv(env); /* free environment memory*/
				return 0;
			} /*end of if error*/
		}/* end of specific  [i,j] cell*/
	}/* go through all possible [i,j]*/


	/*Add col constraints - for each j index*/
	for(i=0;i<dim;i++){
		for(k=0;k<dim;k++){
			for(j=0;j<dim;j++){ /*sum variable values over all the col*/
				ind[j]=i*dim*dim+j*dim+k;
				val[j]=1.0;
			}
			error= GRBaddconstr(model,dim,ind,val,GRB_EQUAL,1.0,NULL);
			if(error){
				printf("Error %d : in GRBaddconstr: %s\n", error, GRBgeterrormsg(env));
				GRBfreemodel(model); /* free model memory*/
				GRBfreeenv(env); /* free environment memory*/
				return 0;
			} /*end of if error*/
		}/* end of specific  [i,j] cell*/
	}/* go through all possible [i,j]*/


	/*Add block constraints*/
	for(k=0;k<dim;k++){
		for(ii=0;ii<blockHeight;ii++){
			for(jj=0;jj<blockWidth;jj++){
				count=0;
				for(i=ii*blockHeight; i<(ii+1)*blockHeight;i++){
					for(j=jj*blockWidth;j<(jj+1)*blockWidth;j++){
						ind[count]=i*dim*dim+j*dim+k;
						val[count]=1.0;
						count++;
					}
				}
				error = GRBaddconstr(model,dim,ind,val,GRB_EQUAL,1.0,NULL);
				if(error){
					printf("Error %d : in GRBaddconstr: %s\n", error, GRBgeterrormsg(env));
					GRBfreemodel(model); /* free model memory*/
					GRBfreeenv(env); /* free environment memory*/
					return 0;
				} /*end of if error*/

			}

		}
	}


	/*Add fixed-cell constraints*/
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			if(fixed[i][j]==1){ /* if certain cell is set to be fixed*/
				k=board[i][j];
				ind[0]=i*dim*dim+j*dim+k;
				val[0]=1; /*coeff of constraint is 1*/
				error = GRBaddconstr(model,1,ind,val,GRB_EQUAL,1.0,NULL); /*add constraint: 1*X=1
				 for the certain X that's in the i*dim*dim+j*dim+k place in variables array.*/
				if(error){
					printf("Error %d : in GRBaddconstr: %s\n", error, GRBgeterrormsg(env));
					GRBfreemodel(model); /* free model memory*/
					GRBfreeenv(env); /* free environment memory*/
					return 0;
				} /*end of if error*/
			}
		}
	}

	/*Optimize model*/
	error = GRBoptimize(model);
	if(error){
		printf("Error %d : in GRBoptimize: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	/*Capture solution information*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS,&optimstatus); /*query the status of the optimization process
	 by retrieving the values of the status attribute.*/
	if(error){
		printf("Error %d : in GRBgetintattr: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if(error){
		printf("Error %d : in GRBgetdblattr: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /*free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	error = GRBgetdblattarray(model,GRB_DBL_ATTR_X,0,dim*dim*dim,sol);
	if(error){
		printf("Error %d : in GRBgetdblattarray: %s\n", error, GRBgeterrormsg(env));
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 0;
	}

	/*case an optimum was found*/
	if(optimstatus == GRB_OPTIMAL){
		/*update intSol to int values of double sol array*/
		for(i=0;i<dim*dim*dim;i++){
					intSol[i]= floor(sol[i]);
		}

		/*update solvedBoard according to sol array*/
		for (i=0; i<dim; i++){ /*cols*/
			for(j=0;j<dim; j++){ /*rows*/
				for(k=0; k<dim;k++){ /*values from 0 to dim-1*/
					if(intSol[i*dim*dim+j*dim+k]==1){
						solvedBoard[i][j]=(k+1); /*sets the value to the cell*/
					}
				}
			}
		}
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 1;
	}

	// case of infinite or unbounded solution
	if(optimstatus == GRB_INF_OR_UNBD){
		printf("Unsolvable\n");
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return -1;
	}

	/*case there was a runtime problem- Free model*/
	GRBfreemodel(model); /* free model memory*/
	GRBfreeenv(env); /* free environment memory*/
	return 0;
}

