#include "StringLib.h"
#include <iostream>

#pragma warning(disable:4996)
using namespace std;


/*
Kreiert TString aus char*
*/
TString initializeString(char* string) {
	TString tString;
	// bufferSize behinaltet auch das '/0'- Zeichen
	tString.bufferSize = strlen(string) + 1;
	tString.string = new char[tString.bufferSize];
	strcpy(tString.string, string);
	return tString;

}

/*
Löscht einen TString
*/
void safeDeleteString(TString* string) {
	if (string->bufferSize > 0 && string->string != NULL) {
		delete[] string->string;
		string->string = NULL;
		string->bufferSize = 0;
	}
}

/*
Fügt einen String an TString destination an
*/
void addToString(TString* destination, const char* const source) {
	TString finishedString;
	int length = destination->bufferSize + strlen(source);
	finishedString.bufferSize = length;
	finishedString.string = new char[length];
	strcpy(finishedString.string, destination->string);
	strcat(finishedString.string, source);
	copyString(destination, finishedString);
	safeDeleteString(&finishedString);
}

/*
Fügt einen Char an TString destination an
*/
void addToString(TString* destination, char source) {
	char cStringified[2] = { source, 0 };
	addToString(destination, cStringified);
}

/*
Fügt einen TString an TString destination an
*/
void addToString(TString* destination, TString source) {
	TString finishedString;
	int length = destination->bufferSize + source.bufferSize - 1;
	finishedString.bufferSize = length;
	finishedString.string = new char[length];
	strcpy(finishedString.string, destination->string);
	strcat(finishedString.string, source.string);
	copyString(destination, finishedString);
	safeDeleteString(&finishedString);
}

/*
Copiert einen char* an den Platz vom TString destination
*/
void copyString(TString* destination, char* source) {
	safeDeleteString(destination);
	*destination = initializeString(source);
}

/*
Copiert einen TString an den Platz vom TString destination
*/
void copyString(TString* destination, TString source) {
	copyString(destination, source.string);
}

/*
Vergleicht zwei TStrings (zunächst wird gleiche Länge überprüft)
gibt false zurück, falls sie nicht gleich sind
*/
bool compareString(TString string1, TString string2) {
	if (string1.bufferSize != string2.bufferSize)
	{
		return false;
	}
	else
	{
		for (size_t i = 0; i < string1.bufferSize; i++)
		{
			if (string1.string[i] != string2.string[i])
			{
				return false;
			}
		}
	}
	return true;
}