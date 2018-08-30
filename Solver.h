/*
 * Solver.h
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#ifndef SOLVER_H_
#define SOLVER_H_

/*
 * Autofill- autofills board according to rules.
 */
int Autofill(int** board, int blockHeight, int blockWidth);


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
