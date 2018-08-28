/*
 * Game.h
 *
 *  Created on: 8 баев 2018
 *      Author: User
 */

#ifndef GAME_H_
#define GAME_H_

/*
 *  isErroneous- checks if non-zero value at <col,row> exists in it's row, column, or block.
 *  returns 1 if so, 0 else.
 */
int isErroneous(int col, int row);

/*
 *  isBoardErroneous- checks if there are erroneous values in the board using isErroneous.
 *  returns 1 if so, 0 else.
 */
int isBoardErroneous(void);

/*
 *  printBoard- prints board per instructions.
 */
void printBoard(void);


int numInRange(int num, int lower, int upper);
int validate(int** board,int blockHeight, int blockWidth, int dim, int printMessage);
int allfilled(int** board, int dim);
int isBoardEmpty(int** board, int dim);

/*
 * Memory commands- pertaining to main board, fixed board, solved board, and undoRedo List, these 2 funcs
 * create and free the memory needed. Memory Creator returns 1 if all is good, FATAL_ERROR if calloc failed.
 */
void MainMemoryFreer();
int MainMemoryCreator();

/*
 * OpenFileHelper- opens file at fileName for edit/solve commands.
 */
void OpenFileHelper(char* fileName);

/*
 * printsForRedoUndo- takes care of correct printing for undo redo using these parameters.
 * 			isUndo- 1 if undo, 0 if redo
 * 			row- current row
 * 			col- current col
 * 			num1- number at cell before change
 * 			num2- number at cell after change
 */
void printsForRedoUndo(int isUndo, int row, int col, int num1, int num2);

int problemCellAssignment(int i, int j);
int randomFill(int X,int *arri,int *arrj);
int generate(int X, int Y, int dim);

/*
 * doCommand- the only function in use by main. Accepts the parsed command struct from parser (through main),
 * and does work accordingly. most functions in use by doCommand are "hidden" (exist only in source file, and
 * not in this header file). returns FATAL_ERROR if fatal error is encountered, any other number otherwise.
 */
int doCommand(Command* inpCommand);
#endif /* GAME_H_ */
