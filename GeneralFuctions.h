#pragma once
#include <stdio.h>
#include "StringLib.h"

struct  TUser
{
	int id;
	char name[40 + 1];
	char password[40 + 1];
};

struct TEvent
{
	int id;
	int userId;
	char eventName[100 + 1];
	char date[10 + 1];
	char time[5 + 1];
};


FILE* openUserFile();
FILE* openEventFile();
void generateColumnOfRow(TString*, char*);
int getIdCounter(FILE*);
void saveEventInFile(FILE*, char*, char*, char*, int);
void saveUserInFile(FILE*, char*, char*);
char* charToSTring(char);