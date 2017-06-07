#include <iostream>
#include <stdio.h>
#include "GeneralFuctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"

#pragma warning(disable:4996)
using namespace std;


/*
Liest die Zähler-ID aus einem File
und erhöht sie um eins
*/
int getIdCounter(FILE* file) {
	int idCounter = 0;
	fseek(file, 0, SEEK_SET);
	fread(&idCounter, sizeof(int), 1, file);
	idCounter++;
	return idCounter;
}

/*
Prüft, ob ein Char zu den Hex-Werten gehört
*/
bool isHex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/*
Zerlegt einen Inputstring in die Feldnamen und deren dazugehörigen Werte
Gibt ein TFieldArray mit den einzelnen name - value - Paaren zurück
*/
TFieldArray parseInputString(char* inputString) {
	TField field = initializeField("", "");
	TFieldArray fieldArray;
	TString fieldName = initializeString("");
	TString fieldValue = initializeString("");
	bool isFirst = true;
	unsigned char fromHexTable[256] = { 0 };
	fromHexTable['0'] = 0;
	fromHexTable['1'] = 1;
	fromHexTable['2'] = 2;
	fromHexTable['3'] = 3;
	fromHexTable['4'] = 4;
	fromHexTable['5'] = 5;
	fromHexTable['6'] = 6;
	fromHexTable['7'] = 7;
	fromHexTable['8'] = 8;
	fromHexTable['9'] = 9;
	fromHexTable['A'] = 10;
	fromHexTable['B'] = 11;
	fromHexTable['C'] = 12;
	fromHexTable['D'] = 13;
	fromHexTable['E'] = 14;
	fromHexTable['F'] = 15;
	fromHexTable['a'] = 10;
	fromHexTable['b'] = 11;
	fromHexTable['c'] = 12;
	fromHexTable['d'] = 13;
	fromHexTable['e'] = 14;
	fromHexTable['f'] = 15;

	for (size_t i = 0; inputString[i] != '\0'; i++)
	{
		if (inputString[i] == '=')
		{
			// Bauen des Fieldwertes
			i++;
			for (i; inputString[i] != '&' && inputString[i] != '\0'; i++)
			{
				// Von der Webseite codierte Zeichen werden zum Speichern wieder zu Zeichen umgewandelt
				if (inputString[i] == '%' && isHex(inputString[i + 1]) && isHex(inputString[i + 2])) {
					int asciiValue = 0;
					char value = inputString[i + 1];
					// Errechnet sich den ascii-Wert des Zeichens
					asciiValue += fromHexTable[inputString[i + 1]] * 16;
					asciiValue += fromHexTable[inputString[i + 2]];
					unsigned char character = asciiValue;
					addToString(&fieldValue, character);
					i += 2;
				}
				else if (inputString[i] == '+') {
					addToString(&fieldValue, ' ');
				}
				else {
					addToString(&fieldValue, inputString[i]);
				}
			}

			safeDeleteField(&field);
			field = initializeField(fieldName, fieldValue);
			if (isFirst) {
				fieldArray = initializeFieldArray(field);
				isFirst = false;
				safeDeleteString(&fieldName);
				safeDeleteString(&fieldValue);
				fieldName = initializeString("");
				fieldValue = initializeString("");
			}
			else {
				addToFieldArray(&fieldArray, field);
				safeDeleteString(&fieldName);
				safeDeleteString(&fieldValue);
				fieldName = initializeString("");
				fieldValue = initializeString("");
				if (inputString[i] == '\0') {
					i--;
				}
			}
		}
		else {
			// Bauen des Fieldnamen
			addToString(&fieldName, inputString[i]);
		}
	}

	safeDeleteField(&field);
	safeDeleteString(&fieldName);
	safeDeleteString(&fieldValue);
	return fieldArray;

}

/*
Baut aus einem Event ein Json-String
*/
void buildJson(TString* jsonString, TEvent* event) {
	addToString(jsonString, " { \"title\":\"");
	TString nameString = initializeString("");
	for (int i = 0; i < strlen(event->title); ++i) {
		char c = event->title[i];
		switch (c)
		{
		case '\\':
		{
			addToString(&nameString, "\\\\");
			break;
		}
		case '"':
		{
			addToString(&nameString, "\\");
			addToString(&nameString, '"');
			break;
		}
		case '\n':
		{
			addToString(&nameString, "\\");
			addToString(&nameString, 'n');
			break;
		}
		default:
		{
			addToString(&nameString, c);
			break;
		}
		}
	}

	addToString(jsonString, nameString);
	safeDeleteString(&nameString);
	addToString(jsonString, "\", \"start\":\"");
	addToString(jsonString, event->start);
	addToString(jsonString, "\", \"end\":\"");
	addToString(jsonString, event->end);
	addToString(jsonString, "\", \"description\":\"");
	addToString(jsonString, event->description);
	addToString(jsonString, "\", \"userId\":");
	char stringUserId[6 + 1] = { 0 };
	itoa(event->userId, stringUserId, 10);
	addToString(jsonString, stringUserId);
	addToString(jsonString, ", \"id\":");
	char stringEventId[6 + 1] = { 0 };
	itoa(event->id, stringEventId, 10);
	addToString(jsonString, stringEventId);
	addToString(jsonString, " }");
}