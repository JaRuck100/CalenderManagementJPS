#include <stdio.h>
#include "FieldsLib.h"
#pragma once

void registerFunction(TFieldArray);
void ceckUserFunction(TFieldArray, int);
void showEventsFunction(TFieldArray, int);
void deleteEventFunction(TFieldArray);
void addEventFunction(TFieldArray, int);
void searchForEventFunction(TFieldArray);
void changeEventFunction(TFieldArray);
void changePasswordFunction(TFieldArray, int);
void logoutFunction(TFieldArray);
void loginFunction(TFieldArray, FILE*);