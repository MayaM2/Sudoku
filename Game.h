/*
 * Game.h
 *
 *  Created on: 8 баев 2018
 *      Author: User
 */

#ifndef GAME_H_
#define GAME_H_

/*
 * doCommand- the only function in use by main. Accepts the parsed command struct from parser (through main),
 * and does work accordingly. most functions in use by doCommand are "hidden" (exist only in source file, and
 * not in this header file). returns FATAL_ERROR if fatal error is encountered, any other number otherwise.
 */
int doCommand(Command* inpCommand);

#endif /* GAME_H_ */
