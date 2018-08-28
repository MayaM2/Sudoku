#ifndef STRUCTS_H_
#define STRUCTS_H_


/*
 * Command
 */
typedef struct Command{
	int commands;
	int validity; /* until said otherwise */
	int arg1;
	int arg2;
	int arg3;
	char* fileName;
} Command;


/*
 * Constructor
 */
Command* commandCreator(int commands, int validity, int arg1, int arg2, int arg3);
/*
 * Destructor
 */
void commandDestructor(Command * c);

/*
 * Load file List - to save info from file read for board creation in main
 */
typedef struct lfNode{
	struct lfNode* next;
	int row;
	int col;
	int val;
	int isFixed;
} LoadFileNode;

typedef struct lfList{
	LoadFileNode* head;
	LoadFileNode* curr;
	int colsPerBlock;
	int rowsPerBlock;
} LoadFileList;
LoadFileList* LFLCreator();
void LFLDestructor(LoadFileList *li);
void LFLAppend(LoadFileList *li, int row, int col, int val, int isFixed);

/*
 * Undo-Redo List
 */
typedef struct urNode{
	struct urNode* prev;
	struct urNode* next;
	int** nodeBoard;
} UndoRedoNode;

typedef struct urList{
	UndoRedoNode* head;
	UndoRedoNode* tail;
	UndoRedoNode* curr;
	int dim;
} UndoRedoList;

/*
 * Constructor
 */
UndoRedoList* undoRedoCreator(int dim);
/*
 * Destructor
 */
void undoRedoDestroyer(UndoRedoList* li);
/*
 * create and append new node to undoredo list. if there are nodes following current node, remove them before appending.
 */
void undoRedoAppend(UndoRedoList* li, int** board);


/*
 * num-solutions Stack
 */
typedef struct stackNode{
	struct stackNode* prev;
	struct stackNode* next;
	int row;
	int col;
	int* neighborsBin;
} RecStackNode;

typedef struct stack{
	RecStackNode* head;
} RecStack;

/*
 * Creator
 */
RecStack* recStackCreator();
/*
 * Destructor
 */
void destroyStackNode(RecStackNode* n);
void recStackDestroyer(RecStack* r);
/*
 * create and push new node into stack (ie- add to FRONT of list). 1st one gets a node and adds, second
 * creates new node with given info and calls first.
 */
void recStackPush(RecStack* r, RecStackNode* n);
void recStackPushInfo(RecStack* r, int row, int col, int* bin, int dim);
/*
 * create stack node;
 */
RecStackNode* recStackNodeCreator(int row, int col, int* bin, int dim);
/*
 * return and update head of stack
 */
RecStackNode* recStackPop(RecStack* r);


void destroyNode(UndoRedoNode* n, int dim);
void LFLNodeDelete(LoadFileNode *n);


#endif /* STRUCTS_H_ */

