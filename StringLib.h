#pragma once
struct TString
{
	int bufferSize;
	char* string;
};

struct TStringArray
{
	int length;
	TString* strings;
};

/* Noch nicht verwendet
   Möglicherweise später für die Rückgabe der Formulare
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
*/

TString initializeString(char*);
void safeDeleteString(TString*);
void addToString(TString*, const char*);
void addToString(TString*, TString);
void copyString(TString* destination, TString source);
TStringArray initializeStringArray(TString);
TStringArray initializeStringArray(char*);
void deleteStringArray(TStringArray*);
void addToStringArray(TStringArray*, TString);
void addToStringArray(TStringArray*, char*);
TString findAndReplace(TString, TString, TString);