#ifndef IOMOD_H_
#define IOMOD_H_

/*
 * openFile- open and read file at fileName according to mode given by boolean isSolve and load info into
 * LoadFileList struct. return 0 if any file functions failed (including original file opening), 1 otherwise.
 */
int openFile(LoadFileList *li, char* fileName, int isSolve);

/*
 * saveFile- save board values, marking fixed cells, into file at fileName, according to gameMode.
 * blockHeight and blockWidth are needed for first line of saved file.
 */
int saveFile(int** board, int** fixed, char* fileName, int gameMode, int blockHeight, int blockWidth);

#endif
