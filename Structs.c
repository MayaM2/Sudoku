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

UndoRedoList* undoRedoCreator(){
	UndoRedoList* li;
	UndoRedoNode* n; /* DUMMY NODE!!!!*/
	n=(UndoRedoNode*)calloc(1,sizeof(UndoRedoNode));
	n->row=-1;
	n->col=-1;
	n->oldVal=-1;
	n->newVal=-1;
	n->isAutofilled=-1;
	li=(UndoRedoList*)calloc(1,sizeof(UndoRedoList));
	li->head=n;
	li->curr=li->head;
	li->tail=li->head;
	return li;
}

/*
 * "Hidden" func- destroy list nodes.
 */
void destroyNode(UndoRedoNode* n){
	if(n!=NULL){
		destroyNode(n->next);
		free(n);
	}
}

void undoRedoDestroyer(UndoRedoList* li){
	destroyNode(li->head);
	li->head=NULL;
	free(li);
}

void undoRedoAppend(UndoRedoList* li,int row, int col, int oldVal, int newVal, int isAutofilled)
{
	UndoRedoNode* n;
	UndoRedoList* dummy;
	n=(UndoRedoNode*)calloc(1,sizeof(UndoRedoNode));
	n->row=row;
	n->col=col;
	n->oldVal=oldVal;
	n->newVal=newVal;
	n->isAutofilled=isAutofilled;

	/*
	 * if current node has next, we should delete all values following current node. in any case, append new node
	 * after current node, and make current as well as tail of list.
	 */
	if(li->curr->next!=NULL){
		dummy=undoRedoCreator();
		dummy->head=li->curr->next;
		li->curr->next->prev=NULL;
		li->curr->next=NULL;
		undoRedoDestroyer(dummy);
	}
	li->curr->next=n;
	n->prev=li->curr;
	li->curr=li->curr->next;
	li->tail=li->curr;
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
 * "Hidden" func- destroy stack nodes.
 */
void destroyStackNode(RecStackNode* n){
	if(n!=NULL){
		destroyStackNode(n->next);
		free(n->board);
		free(n);
	}
}
void recStackDestroyer(RecStack* r){
	destroyStackNode(r->head);
	free(r);
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
		r->head->prev=n;
		r->head=n;
	}
}
RecStackNode* recStackNodeCreator(int row, int col, int isForward, int** board)
{
	RecStackNode* n;
	n=(RecStackNode*)calloc(1,sizeof(RecStackNode));
	n->board=board;
	n->col=col;
	n->isForward=isForward;
	n->next=NULL;
	n->prev=NULL;
	n->row=row;
	return n;
}
RecStackNode* recStackPop(RecStack* r)
{
	RecStackNode* n;
	n=r->head;
	if(n->next!=NULL)
		n->next->prev=NULL;
	r->head=n->next;
	n->next=NULL;
	return n;
}
