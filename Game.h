/*
 * Game.h
 *
 *  Created on: 8 баев 2018
 *      Author: User
 */

#ifndef GAME_H_
#define GAME_H_
int isErroneous(int col, int row);
int isBoardErroneous(void);
void printBoard(void);
int numInRange(int num, int lower, int upper);
int validate(int** board,int blockHeight, int blockWidth, int dim, int printMessage);
int allfilled(int** board, int dim);
int isBoardEmpty(int** board, int dim);
void MainMemoryFreer();
void MainMemoryCreator();
void OpenFileHelper(char* fileName);
void printsForRedoUndo(int isUndo, int row, int col, int num1, int num2);
int problemCellAssignment(int i, int j);
int randomFill(int X,int *arri,int *arrj);
int generate(int X, int Y, int dim);
int doCommand(Command* inpCommand);
#endif /* GAME_H_ */
