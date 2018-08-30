#ifndef STRUCTS_H_
#define STRUCTS_H_


/*
 * Command struct: int commands - will contain a command Enum value according to the relevant command
 * validity: indicator to whether all parameters are indeed integers, relevant for commands
 * that accept integers as arguments. 1 if true, 0 false.
 * arg1, arg2, arg3: if given, command's integer arguments are being stored here.
 * char *fileName: if given, command's file name argument is being stored here.
 * At any given moment, only one such Command struct exist. It is initiated by main,
 * and being changed by parseCommand whenever a new command is being parsed.
 * When exiting the game, the command struct components are freed.
 *
 */
typedef struct Command{
	int commands;
	int validity;
	int arg1;
	int arg2;
	int arg3;
	char* fileName;
} Command;


/*
 * commandCreator is a function which initiates a command struct,
 * and allocates the needed memory for the command struct arguments.
 * This function is being called once in main.c.
 */
Command* commandCreator(int commands, int validity, int arg1, int arg2, int arg3);
/*
 * commandDestructor is a function that destructs a command struct, by freeing
 * dinamically allocated memory.
 * This function is being called once in main.c when exiting the game.
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
 * Destructors
 */
void destroyStackNode(RecStackNode* n);
void recStackDestroyer(RecStack* r);
/*
 * create and push new node into stack (ie- add to FRONT of list). 1st "hidden" one gets a node and adds,
 * second creates new node with given info and calls first.
 */
void recStackPushInfo(RecStack* r, int row, int col, int* bin, int dim);
/*
 * create stack node;
 */
RecStackNode* recStackNodeCreator(int row, int col, int* bin, int dim);
/*
 * return and update head of stack
 */
RecStackNode* recStackPop(RecStack* r);


#endif /* STRUCTS_H_ */

