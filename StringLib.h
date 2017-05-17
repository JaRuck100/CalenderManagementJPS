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

TString initializeString(char*);
void safeDeleteString(TString*);
void addToString(TString*, const char* const);
void addToString(TString*, TString);
void copyString(TString*, char*);
void copyString(TString* destination, TString source);
TStringArray initializeStringArray(TString);
TStringArray initializeStringArray(char*);
void deleteStringArray(TStringArray*);
void addToStringArray(TStringArray*, TString);
void addToStringArray(TStringArray*, char*);
TString findAndReplace(TString, TString, TString);