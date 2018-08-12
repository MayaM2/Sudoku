/*
 * Game.h
 *
 *  Created on: 8 баев 2018
 *      Author: User
 */

#ifndef GAME_H_
#define GAME_H_
int isErroneous(int col, int row);
void printBoard();

/* MAYA: */
void doCommand(Command* command);
int generate();
int hint();
int validate();

#endif /* GAME_H_ */
