#include <stdio.h>
#include "StringLib.h"
#include "FieldsLib.h"
#include "EventFunctions.h"

#pragma once
int getIdCounter(FILE*);
bool isHex(char c);
TFieldArray parseInputString(char*);
void buildJson(TString*, TEvent*);