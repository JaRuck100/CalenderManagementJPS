#include <stdio.h>
#include "StringLib.h"

#pragma once
TString getHtmlFromFile(FILE*);
void generateColumnOfRow(TString*, char*);
int getIdCounter(FILE*);
void errorPage(char* reason);