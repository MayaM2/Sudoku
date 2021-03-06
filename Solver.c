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
 * updateNeighbors- an auxilliary func of autofill and numSols. updates binary array neighborsBin according
 *  to neighbors of cell <col,row> of board, ie values in row, col, and block. 1 at index i in neighborsBin
 *  denotes that value i+1 is a neighbor of cell <col,row>, 0 denotes that it is not.
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
 * isObv- an auxilliary func of autofill. checks if cell <col,row> is obviously filled, ie has one possible
 * value to be filled in. returns 1 if obvious, else 0.
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

int Autofill(int** board, int blockHeight, int blockWidth)
{
	int i=0,j=0, val=0, appended=0, dim=blockHeight*blockWidth, *neighborsBin, **copyBoard;
	neighborsBin=(int*)calloc(dim,sizeof(int));
	copyBoard=(int**)calloc(dim,sizeof(int*));
	if(neighborsBin==NULL || copyBoard==NULL)
		return FATAL_ERROR;
	for(i=0;i<dim;i++)
	{
		copyBoard[i]=(int*)calloc(dim,sizeof(int));
		if(copyBoard[i]==NULL)
			return FATAL_ERROR;
	}
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			copyBoard[i][j]=board[i][j];
	for(i=0;i<dim;i++)
	{
		for(j=0;j<dim;j++)
		{
			if(copyBoard[i][j]==0)
			{
				val=isObv(copyBoard,i,j,blockHeight,blockWidth,neighborsBin);
				if(val!=0){
					appended=1;
					board[i][j]=val;
					printf("Cell <%d,%d> set to %d\n",j+1,i+1,val);
				}
			}
		}
	}
	for(i=0;i<dim;i++)
		free(copyBoard[i]);
	free(copyBoard);
	free(neighborsBin);
	return appended;
}


/*
 * nextCell- retrieves next cell to be checked in board. updates indexes array accordingly. [-2,-2] denotes
 * going before first empty cell, and [-1,-1] denotes going after last empty cell. auxilliary to numSols.
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


int numSols(int** board, int blockHeight, int blockWidth)
{
	int sols=0, i=0, currInd=0, indexes[2]={0,0},row=0,col=0, isForward=1, dim=blockHeight*blockWidth;
	int* bin;
	RecStack *rec = recStackCreator();
	RecStackNode *n;
	bin=(int*)calloc(dim,sizeof(int));
	if(bin==NULL || rec==NULL)
		return FATAL_ERROR;
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
					recStackPushInfo(rec,row,col,bin,dim);
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
	}
	while(rec->head!=NULL);
	recStackDestroyer(rec);
	free(bin);
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
	double *sol = (double*)calloc(dim*dim*dim,sizeof(double*));
	int *intSol = (int*)calloc(dim*dim*dim,sizeof(int*));
	int *ind = (int*)calloc(dim,sizeof(int*));
	double *val = (double*)calloc(dim,sizeof(double*));
	double *obj = (double*)calloc(dim*dim*dim,sizeof(double*)); /*coeffs of obj. function*/
	char *vtype = (char*)calloc(dim*dim*dim,sizeof(char*));
	int optimstatus;
	int i=0, j=0, k=0, ii=0,jj=0,count=0;

	/*Create environment*/
	error = GRBloadenv(&env,NULL);
	/*here no log file will be written since an empty string was given.*/
	if (error || env == NULL){
		goto QUIT;
	}

	/*prevent prints to stdout*/
	error = GRBsetintparam(env, "OutputFlag", 0);
	if (error) goto QUIT;

	/*Create empty model*/
	error = GRBnewmodel(env,&model,"ILPSolve",0,NULL,NULL,NULL,NULL,NULL);
	if (error || env == NULL){
		goto QUIT;
	}

	/*Add variables*/
	for(i=0;i<dim*dim*dim;i++){
		obj[i]=0.0;
		vtype[i]=GRB_BINARY;
	}

	error = GRBaddvars(model,dim*dim*dim,0,NULL,NULL,NULL,obj,NULL,NULL,vtype,NULL);
	if(error){
		goto QUIT;
	}

	/*Update model - to integrate new variables*/
	error = GRBupdatemodel(model);
	if(error){
		goto QUIT;
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
				goto QUIT;
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
				goto QUIT;
			} /*end of if error*/
		}/* end of specific  [i,j] cell*/
	}/* go through all possible [i,j]*/

	/*Add col constraints - for each j index*/
	for(k=0;k<dim;k++){
		for(i=0;i<dim;i++){
			for(j=0;j<dim;j++){ /*sum variable values over all the col*/
				ind[j]=i*dim*dim+j*dim+k;
				val[j]=1.0;
			}
			error= GRBaddconstr(model,dim,ind,val,GRB_EQUAL,1.0,NULL);
			if(error){
				goto QUIT;
			} /*end of if error*/
		}/* end of specific  [i,j] cell*/
	}/* go through all possible [i,j]*/

	/*Add block constraints*/
	for(k=0;k<dim;k++){
		for(ii=0;ii<blockWidth;ii++){
			for(jj=0;jj<blockHeight;jj++){
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
					goto QUIT;
				} /*end of if error*/
			}
		}
	}

	/*Add filled-cell constraints*/
	for(i=0;i<dim;i++){
		for(j=0;j<dim;j++){
			if(board[i][j]!=0){ /* if certain cell is set to be fixed*/
				k=board[i][j];
				ind[0]=i*dim*dim+j*dim+k-1;
				val[0]=1.0; /*coeff of constraint is 1*/
				error = GRBaddconstr(model,1,ind,val,GRB_EQUAL,1.0,NULL); /*add constraint: 1*X=1
				 for the certain X that's in the i*dim*dim+j*dim+k place in variables array.*/
				if(error){
					goto QUIT;
				} /*end of if error*/
			}
		}
	}


	/*Optimize model*/
	error = GRBoptimize(model);
	if(error){
		goto QUIT;
	}

	/*write model to sudoku.lp */
	/*error = GRBwrite(model,"sudoku.lp");
	if(error) goto QUIT;*/


	/*Capture solution information*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS,&optimstatus); /*query the status of the optimization process
	 by retrieving the values of the status attribute.*/
	if(error){
		goto QUIT;
	}

	/*case of infinite or unbounded solution*/
	if(optimstatus == GRB_INF_OR_UNBD){

		free(sol);
		free(intSol);
		free(ind);
		free(val);
		free(obj);
		free(vtype);
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return -1;
	}

	/*case an optimum was found*/
	if(optimstatus == GRB_OPTIMAL){
		error = GRBgetdblattrarray(model,GRB_DBL_ATTR_X,0,dim*dim*dim,sol);
		if(error){
			goto QUIT;
		}

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
		free(sol);
		free(intSol);
		free(ind);
		free(val);
		free(obj);
		free(vtype);
		GRBfreemodel(model); /* free model memory*/
		GRBfreeenv(env); /* free environment memory*/
		return 1;
	}
	else{
		goto QUIT;
	}

	QUIT:
	/*case there was a runtime problem- Free model*/
	free(sol);
	free(intSol);
	free(ind);
	free(val);
	free(obj);
	free(vtype);
	GRBfreemodel(model); /* free model memory*/
	GRBfreeenv(env); /* free environment memory*/
	return 0;
}

