#include <stdio.h>
#include "FieldsLib.h"
#pragma once

struct TEvent
{
	int id;
	int userId;
	char title[100 + 1];
	char start[16 + 1];
	char end[16 + 1];
	char description[150 + 1];
};

FILE* openEventFile();
void buildEventFromFileArray(TEvent*, TFieldArray);
//void saveEventInFile(FILE*, char*, char*, char*, int);
void saveEventInFile(FILE*, TFieldArray, int);
//FILE* searchForEventPosition(FILE*, int);
int changeEvent(FILE*, TFieldArray);
int deleteEvent(FILE*, int);
