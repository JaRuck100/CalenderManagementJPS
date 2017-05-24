#include <stdio.h>
#include "FieldsLib.h"
#pragma once

struct TEvent
{
	int id;
	int userId;
	char eventName[100 + 1] = { 0 };
	char date[10 + 1] = { 0 };
	char time[5 + 1] = { 0 };
};

FILE* openEventFile();
void buildEventFromFileArray(TEvent*, TFieldArray);
void saveEventInFile(FILE*, char*, char*, char*, int);
void saveEventInFile(FILE*, TFieldArray, int);
//FILE* searchForEventPosition(FILE*, int);
int changeEvent(FILE*, TFieldArray, int);
int deleteEvent(FILE*, int);
