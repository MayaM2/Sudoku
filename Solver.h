/*
 * Solver.h
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#ifndef SOLVER_H_
#define SOLVER_H_

/*
 * updateNeighbors- an auxilliary func of autofill and numSols. updates binary array neighborsBin according
 *  to neighbors of cell <col,row> of board, ie values in row, col, and block. 1 at index i in neighborsBin
 *  denotes that value i+1 is a neighbor of cell <col,row>, 0 denotes that it is not.
 */
void updateNeighbors(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin);

/*
 * isObv- an auxilliary func of autofill. checks if cell <col,row> is obviously filled, ie has one possible
 * value to be filled in. returns 1 if obvious, else 0.
 */
int isObv(int** board, int row, int col,int blockHeight, int blockWidth, int* neighborsBin);

/*
 * Autofill- autofills board according to rules.
 */
int Autofill(int** board, int blockHeight, int blockWidth);

/*
 * nextCell- retrieves next cell to be checked in board. updates indexes array accordingly. [-2,-2] denotes
 * going before first empty cell, and [-1,-1] denotes going after last empty cell. auxilliary to numSols.
 */
void nextCell(int row, int col, int isForward,int dim, int* indexes);

/*
 * numSols- for num_solutions command. create stack, run while not empty, and count number of solutions.
 * return number of solutions, or FATAL_ERROR if fatal error (with calloc) occurred.
 */
int numSols(int** board, int blockHeight, int blockWidth);

/*
 * ILPSolver: uses Gurobi to solve a given Sudoku gameboard. Explicit details are given along the function itself.
 * Returns 1 if a solution was found, -1 if there is no solution, or 0 if there was a runtime problem.
 * If a solution was found - Updates the given gameboard to the solved solution.
 */
int ILPSolver(int **board,int**fixed,int**solvedBoard,int blockHeight,int blockWidth, int dim);

#endif /* SOLVER_H_ */
