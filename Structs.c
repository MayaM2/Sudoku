#include <stdlib.h>
#include <stdio.h>
#include "Structs.h"
/*
 * Command
 */

Command* commandCreator(int commands, int validity, int arg1, int arg2, int arg3)
{
	Command* c;
	c=(Command*)calloc(1,sizeof(Command));
	c->fileName=(char*)calloc(256,sizeof(char));
	c->commands=commands;
	c->arg1=arg1;
	c->arg2=arg2;
	c->arg3=arg3;
	c->validity=validity;
	return c;
}

void commandDestructor(Command * c){
	free(c->fileName);
	free(c);
}


/*
 * Load File List
 */
LoadFileList* LFLCreator()
{
	LoadFileList *li;
	li=(LoadFileList*)calloc(1, sizeof(LoadFileList));
	li->curr=NULL;
	li->head=NULL;
	li->colsPerBlock=0;
	li->rowsPerBlock=0;
	return li;
}

void LFLNodeDelete(LoadFileNode *n)
{
	if(n!=NULL)
	{
		LFLNodeDelete(n->next);
		free(n);
	}
}
void LFLDestructor(LoadFileList *li)
{
	LFLNodeDelete(li->head);
	free(li);
}
void LFLAppend(LoadFileList* li,int row, int col, int val, int isFixed)
{
	LoadFileNode *n;
	n=(LoadFileNode*)calloc(1,sizeof(LoadFileNode));
	n->row=row;
	n->col=col;
	n->val=val;
	n->isFixed=isFixed;
	n->next=NULL;
	if(li->head==NULL)
		li->head=n;
	else
		li->curr->next=n;
	li->curr=n;
}


/*
 * Undo-Redo List
 */
UndoRedoList* undoRedoCreator(int dim){
	UndoRedoList* li;
	li=(UndoRedoList*)calloc(1,sizeof(UndoRedoList));
	li->head=NULL;
	li->curr=li->head;
	li->tail=li->head;
	li->dim=dim;
	return li;
}

/*
 * "Hidden" funcs- create and destroy list nodes.
 */

UndoRedoNode* createURNode(int dim, int** board)
{
	int i=0,j=0;
	UndoRedoNode* n;
	n=(UndoRedoNode*)calloc(1,sizeof(UndoRedoNode));
	n->nodeBoard=(int**)calloc(dim,sizeof(int*));
	for(i=0;i<dim;i++)
		n->nodeBoard[i]=(int*)calloc(dim,sizeof(int));
	for(i=0;i<dim;i++)
		for(j=0;j<dim;j++)
			n->nodeBoard[i][j]=board[i][j];
	return n;
}

void destroyNode(UndoRedoNode* n, int dim){
	int i=0;
	if(n!=NULL){
		destroyNode(n->next,dim);
		for(i=0;i<dim;i++)
			free(n->nodeBoard[i]);
		free(n->nodeBoard);
		free(n);
	}
}

void undoRedoDestroyer(UndoRedoList* li){
	destroyNode(li->head,li->dim);
	li->head=NULL;
	free(li);
}

void undoRedoAppend(UndoRedoList* li, int** board)
{
	UndoRedoList *dummy;
	UndoRedoNode *n=createURNode(li->dim,board);
	if(li->curr==NULL)
	{
		li->head=n;
		li->curr=li->head;
		li->tail=li->head;
	}
	else
	{
		if(li->curr->next!=NULL){
			dummy=undoRedoCreator(li->dim);
			dummy->head=li->curr->next;
			li->curr->next->prev=NULL;
			li->curr->next=NULL;
			undoRedoDestroyer(dummy);
		}
		li->curr->next=n;
		li->curr->next->prev=li->curr;
		li->curr=li->curr->next;
		li->tail=li->curr;
	}
}






/*
 * num-solutions Stack
 */

RecStack* recStackCreator(){
	RecStack* r;
	r=(RecStack*)calloc(1,sizeof(RecStack));
	r->head=NULL;
	return r;
}

/*
 * destroy stack nodes.
 */
void destroyStackNode(RecStackNode* n){
	if(n!=NULL){
		destroyStackNode(n->next);
		free(n->neighborsBin);
		free(n);
	}
}
void recStackDestroyer(RecStack* r){
	destroyStackNode(r->head);
	free(r);
}
RecStackNode* recStackNodeCreator(int row, int col, int* bin, int dim)
{
	RecStackNode* n;
	int i=0;
	n=(RecStackNode*)calloc(1,sizeof(RecStackNode));
	n->neighborsBin=(int*)calloc(dim, sizeof(int));
	for(i=0;i<dim;i++)
		n->neighborsBin[i]=bin[i];
	n->col=col;
	n->next=NULL;
	n->row=row;
	return n;
}

void recStackPush(RecStack* r, RecStackNode* n)
{
	if(r->head==NULL)
	{
		r->head=n;
	}
	else
	{
		n->next=r->head;
		r->head=n;
	}
}

void recStackPushInfo(RecStack* r, int row, int col, int* bin, int dim)
{
	recStackPush(r,recStackNodeCreator(row,col,bin,dim));
}


RecStackNode* recStackPop(RecStack* r)
{
	RecStackNode* n;
	n=r->head;
	r->head=n->next;
	n->next=NULL;
	return n;
}
