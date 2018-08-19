#include <stdlib.h>
#include <stdio.h>
#include "Structs.h"
#include "Enums.h"

int openFile(LoadFileList* li, char* fileName, int isSolve)
{
	FILE* fp;
	int m,n,curr,i=0,j=0, isFixed=0;
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stderr,NULL,_IONBF,0);
	setvbuf(stdin,NULL,_IONBF,0);
	/*
	 * this is temporary!! there is probably a default value instead of 81.
	 */
	if(fileName==NULL)
	{
		if(isSolve==0){
			li->colsPerBlock=3;
			li->rowsPerBlock=3;
			return 1;
		}
		else
			return 0;
	}
	else
	{
		fp=fopen(fileName,"r");
		if(fp==NULL)
			return 0;
		/* read m and n, create board accordingly*/
		fscanf(fp,"%d",&m);
		fscanf(fp,"%d",&n);
		li->colsPerBlock=m;
		li->rowsPerBlock=n;
		for(i=0;i<m*n;i++)
		{
			for(j=0;j<m*n;j++)
			{
				isFixed=0;
				fscanf(fp,"%2d",&curr);
				if(fgetc(fp)=='.')
					isFixed=1;
				else
					fseek(fp,-1,SEEK_CUR);
				LFLAppend(li,i,j,curr,isFixed);
				if(isFixed==1)
					fseek(fp,1,SEEK_CUR);
			}
		}

	}
	fclose(fp);
	return 1;
}


int saveFile(int** board, int** fixed, char* fileName, int gameMode, int blockHeight, int blockWidth)
{
	int i=0,j=0, dim=blockHeight*blockWidth;
	FILE *fp;
	fp=fopen(fileName,"w");
	if(fp==NULL)
		return 0;
	fprintf(fp,"%d %d\n",blockWidth,blockHeight);
	for(i=0;i<dim;i++)
	{
		for(j=0;j<dim;j++)
		{
			fprintf(fp,"%d",board[i][j]);
			if((gameMode==EDIT && board[i][j]!=0) || fixed[i][j]==1)
				fprintf(fp,".");
			if(j<dim-1)
				fprintf(fp," ");

		}
		if(i<dim-1)
			fprintf(fp,"\n");
	}
	fclose(fp);
	return 1;
}
