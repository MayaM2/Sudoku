/*
 * Solver.h
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#ifndef SOLVER_H_
#define SOLVER_H_

void updateNeighbors(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin);
int isObv(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin);
void Autofill(int** board, UndoRedoList *urli, int blockHeight, int blockWidth);
void nextCell(int row, int col, int isForward,int dim, int* indexes);
void demoPrint(int sols, int dim, int** board);
int numSols(int** board, int blockHeight, int blockWidth);
int ILPSolver(int **board,int**fixed,int**solvedBoard,int blockHeight,int blockWidth, int dim);

#endif /* SOLVER_H_ */
