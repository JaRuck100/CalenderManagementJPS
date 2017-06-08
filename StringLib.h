#pragma once
struct TString
{
	int bufferSize;
	char* string;
};

TString initializeString(char*);
void safeDeleteString(TString*);
void addToString(TString*, const char* const);
void addToString(TString*, TString);
void copyString(TString*, char*);
void copyString(TString* destination, TString source);
void addToString(TString*, char);
/*
Vergleicht zwei TStrings (zun�chst wird gleiche L�nge �berpr�ft)
gibt false zur�ck, falls sie nicht gleich sind
*/
bool compareString(TString string1, TString string2);