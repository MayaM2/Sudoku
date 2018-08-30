/*
 * Parser.c
 *
 *  Created on: 9 Aug 2018
 *      Author: mayam
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "Enums.h"
#include "Structs.h"

/*
 * IsInteger - "hidden" function that returns 1 if a given string represents an integer,
 * returns 0 otherwise.
 */
int IsInteger(const char* s)
{
	int i=0;
	if(s==NULL){
		return 0;
	}
	for(i = 0; (unsigned)i < strlen(s); i ++)
	{
		/*ASCII value of 0 = 48, 9 = 57*/
		if ((unsigned)s[i] < 48 || (unsigned)s[i] > 57)
         return 0;
   }
   return 1;
}


void ParseCommand(Command *currCommand){
	char readFromUser[256]={0};
	char* userInp[4],c='a';
	int i=0;

	userInp[0]=NULL;
	userInp[1]=NULL;
	userInp[2]=NULL;
	userInp[3]=NULL;

	free(currCommand->fileName);
	currCommand->fileName=(char*)calloc(256,sizeof(char));

	/*Don't want to catch game prints...*/
	setvbuf(stdin,NULL,_IONBF,0);
	setvbuf(stdout,NULL,_IONBF,0);

	printf("Enter your command:\n");

	/* read input from user. if fgets is null, we have reached EOF */
	if(fgets(readFromUser, 256, stdin)==NULL){
		currCommand->commands=EXIT_COMMAND;
		return;
	}

	for(i=0;i<256 && c!='\n' && c!=EOF;i++)
		c=readFromUser[i];
	/*clear stdin. this also means that command is invalid!*/
	if(i==256)
	{
		currCommand->commands=INVALID_COMMAND;
		while((c=getchar())!='\n' && c!=EOF);
		return;
	}

	if(readFromUser[0]=='\n' || readFromUser[0]=='\r'){
		currCommand->commands=IGNORE_COMMAND;
		return;
	}

	/* split user's input by whitespaces */
	userInp[0] = strtok(readFromUser," \t\r\n");
	userInp[1] = strtok(NULL," \t\r\n");
	userInp[2] = strtok(NULL," \t\r\n");
	userInp[3] = strtok(NULL," \t\r\n");

	currCommand->validity=1;
	/* create the result command struct according to input */
	if(strcmp(userInp[0],"solve")==0){
		currCommand->commands=SOLVE_COMMAND;
		if(userInp[1]!=NULL)
			currCommand->fileName=strcpy(currCommand->fileName,userInp[1]);
		else
			currCommand->commands=INVALID_COMMAND;
	}

	else if(strcmp(userInp[0],"edit")==0){
		currCommand->commands=EDIT_COMMAND;
		if(userInp[1]!=NULL)
			currCommand->fileName=strcpy(currCommand->fileName,userInp[1]);
		else
		{
			free(currCommand->fileName);
			currCommand->fileName=NULL;
		}
	}
	/*in case numeric values are expeced - check if they are numeric,
	 * else - validity argument will be set to invalid input */
	else if(strcmp(userInp[0],"mark_errors")==0){
		currCommand->commands=MARKERRORS_COMMAND;
		if(IsInteger(userInp[1])){
			currCommand->arg1=atoi(userInp[1]);
		}
		else{
			currCommand->validity=0;
		}

	}
	else if(strcmp(userInp[0],"print_board")==0){
		currCommand->commands=PRINTBOARD_COMMAND;
	}
	else if(strcmp(userInp[0],"set")==0){
		currCommand->commands=SET_COMMAND;
		if(IsInteger(userInp[1]) && IsInteger(userInp[2]) && IsInteger(userInp[3])){
			currCommand->arg1=atoi(userInp[1]);
			currCommand->arg2=atoi(userInp[2]);
			currCommand->arg3=atoi(userInp[3]);
		}
		else{
			currCommand->validity=0;
		}
	}
	else if(strcmp(userInp[0],"validate")==0){
		currCommand->commands=VALIDATE_COMMAND;
	}
	else if(strcmp(userInp[0],"generate")==0){
		currCommand->commands=GENERATE_COMMAND;
		if(IsInteger(userInp[1]) && IsInteger(userInp[2])){
			currCommand->arg1=atoi(userInp[1]);
			currCommand->arg2=atoi(userInp[2]);
		}
		else{
			currCommand->validity=0;
		}

	}
	else if(strcmp(userInp[0],"undo")==0){
		currCommand->commands=UNDO_COMMAND;
	}
	else if(strcmp(userInp[0],"redo")==0){
		currCommand->commands=REDO_COMMAND;
	}

	else if(strcmp(userInp[0],"save")==0){
		currCommand->commands=SAVE_COMMAND;
		if(userInp[1]!=NULL)
			currCommand->fileName=strcpy(currCommand->fileName,userInp[1]);
		else
			currCommand->commands=INVALID_COMMAND;
	}
	else if(strcmp(userInp[0],"hint")==0){
		currCommand->commands=HINT_COMMAND;
		if(IsInteger(userInp[1]) && IsInteger(userInp[2])){
			currCommand->arg1=atoi(userInp[1]);
			currCommand->arg2=atoi(userInp[2]);
		}
		else{
			currCommand->validity=0;
		}

	}
	else if(strcmp(userInp[0],"num_solutions")==0){
		currCommand->commands=NUMSOLUTIONS_COMMAND;
	}
	else if(strcmp(userInp[0],"autofill")==0){
		currCommand->commands=AUTOFILL_COMMAND;
	}
	else if(strcmp(userInp[0],"reset")==0){
		currCommand->commands=RESET_COMMAND;
	}
	else if(strcmp(userInp[0],"exit")==0){
		currCommand->commands=EXIT_COMMAND;
	}

	/*
	 * in case no command was detected
	 */
	else {
		currCommand->commands=INVALID_COMMAND;
	}
	if(currCommand->validity==0)
		currCommand->commands=INVALID_COMMAND;
}

