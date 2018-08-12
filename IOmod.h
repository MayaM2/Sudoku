#ifndef IOMOD_H_
#define IOMOD_H_
int openFile(LoadFileList *li, char* fileName, int isSolve);

int saveFile(int** board, int** fixed, char* fileName, int gameMode, int blockHeight, int blockWidth);

#endif
