#pragma once
#include "StringLib.h"

struct TField
{
	TString name;
	TString value;
};

struct TFieldArray
{
	int length;
	TField* fields;
};

TField initializeField(char*, char*);
TField initializeField(TString, TString);
void safeDeleteField(TField*);
void changeFieldValue(TField*, char*);
void copyField(TField*, TField);
TFieldArray initializeFieldArray(TField);
void safeDeleteFieldArray(TFieldArray*);
void addToFieldArray(TFieldArray*, TField);