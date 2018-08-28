#include <stdlib.h>
#include <stdio.h>
#include "Structs.h"
#include "Enums.h"

int openFile(LoadFileList* li, char* fileName, int isSolve)
{
	FILE* fp;
	int m,n,curr,i=0,j=0, isFixed=0;
	long int endPos=0;
	char c;
	setvbuf(stdout,NULL,_IONBF,0);
	setvbuf(stderr,NULL,_IONBF,0);
	setvbuf(stdin,NULL,_IONBF,0);
	/*
	 * if no fileName supplied in edit, create 9 by 9 board.
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

		fseek(fp,0,SEEK_END);
		endPos=ftell(fp);
		fseek(fp,0,SEEK_SET);
		/* read m and n, create board accordingly*/
		if(fscanf(fp,"%d",&m)!=1)
		{
			printErrorMessage("fscanf");
			return 0;
		}
		if(fscanf(fp,"%d",&n)!=1)
		{
			printErrorMessage("fscanf");
			return 0;
		}
		li->colsPerBlock=n;
		li->rowsPerBlock=m;
		for(i=0;i<m*n;i++)
		{
			for(j=0;j<m*n;j++)
			{
				isFixed=0;
				if(fscanf(fp,"%2d",&curr)!=1)
				{
					printErrorMessage("fscanf");
					return 0;
				}
				if(ftell(fp)!=endPos)
				{
					c=fgetc(fp);
					if(c=='.')
						isFixed=1;
					else
					{
						if(c==EOF)
						{
							printErrorMessage("fgetc");
							return 0;
						}
						else
						{
							if(fseek(fp,-1,SEEK_CUR)!=0)
							{
								printErrorMessage("fseek");
								return 0;
							}
						}
					}
				}
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
	fprintf(fp,"%d %d\n",blockHeight,blockWidth);
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
		fprintf(fp,"\n");
	}
	fclose(fp);
	return 1;
}
