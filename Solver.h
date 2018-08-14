/*
 * Solver.h
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#ifndef SOLVER_H_
#define SOLVER_H_

void Autofill(int** board, UndoRedoList *urli, int blockHeight, int blockWidth);

int numSols(int** board, int blockHeight, int blockWidth);

int ILPSolver();

#endif /* SOLVER_H_ */
