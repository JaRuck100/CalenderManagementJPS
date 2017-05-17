#include "StringLib.h"
#include <iostream>
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
Kreiert TStringArray aus TString
*/
TStringArray initializeStringArray(TString string) {
	TStringArray tStringArray;
	tStringArray.length = 1;
	tStringArray.strings = new TString[tStringArray.length];
	//tStringArray.strings[0] = initializeString(string.string);
	tStringArray.strings[0].string = new char[string.bufferSize];
	copyString(tStringArray.strings, string);
	return tStringArray;
}

/*
Kreiert TStringArray aus char*
*/
TStringArray initializeStringArray(char*string) {
	TString temp = initializeString(string);
	TStringArray tStringArray = initializeStringArray(temp);
	return tStringArray;
}

/*
Löscht die Inhalte und das TStringArray selbst
*/
void deleteStringArray(TStringArray* stringArray) {
	if (stringArray->length > 0 && stringArray->strings != NULL) {
		for (int i = 0; i < stringArray->length; i++)
		{
			safeDeleteString(&stringArray->strings[i]);
		}
		stringArray->length = 0;
		delete[] stringArray->strings;
		stringArray->strings = NULL;
	}
}

/*
Fügt einem TStringArray einen TString hinzu
*/
void addToStringArray(TStringArray* stringArray, TString string) {
	TString* temp = stringArray->strings;
	++stringArray->length;
	stringArray->strings = new TString[stringArray->length];
	for (size_t i = 0; i < stringArray->length - 1; i++)
	{
		copyString(&stringArray->strings[i], temp[i]);
	}
	safeDeleteString(temp);
	stringArray->strings[stringArray->length - 1] = initializeString(string.string);
}

/*
Fügt einem TStringArray einen String hinzu
*/
void addToStringArray(TStringArray* stringArray, char* string) {
	TString temp = initializeString(string);
	addToStringArray(stringArray, temp);
	safeDeleteString(&temp);
}
/*
Sucht in string nach dem Vorkommen von search
Gibt TString zurück, in dem search in string durch replace ersetzt wurde
*/
TString findAndReplace(TString string, TString search, TString replace) {
	char* subString = strstr(string.string, search.string);
	if (subString == NULL) {
		return string;
	}
	size_t positionOfSubstring = subString - string.string;
	char* stringBegin = new char[string.bufferSize - strlen(subString)];
	char* afterSubstring = new char[strlen(subString) - search.bufferSize + 1];
	if (afterSubstring == NULL) {
		return string;
	}
	strncpy(stringBegin, string.string, positionOfSubstring);
	stringBegin[positionOfSubstring] = '\0';
	TString finishedString = initializeString(stringBegin);
	addToString(&finishedString, replace);
	strcpy(afterSubstring, subString + search.bufferSize);

	addToString(&finishedString, afterSubstring);
	delete[] stringBegin;
	delete[] afterSubstring;
	return finishedString;
}