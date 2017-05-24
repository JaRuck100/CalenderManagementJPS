#include <iostream>
#include <stdio.h>
#include "GeneralFuctions.h"
#include "GenerateContentFunctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"
#include "UserFunctions.h"

#pragma warning(disable:4996)
using namespace std;


/*
Schreibt den Inhalt von htmlFile in einen TString.
*/
TString getHtmlFromFile(FILE* htmlFile) {
	char htmlPage[1000 + 1] = { 0 };
	int a = 0;
	char character;

	while (!feof(htmlFile)) {

		character = getc(htmlFile);
		if (feof(htmlFile)) {
			continue;
		}
		htmlPage[a] = character;
		a++;
	}
	//htmlPage[a] = '\0';
	TString htmlPageString = initializeString(htmlPage);
	return htmlPageString;
}


/*
Prüft welcher placeholder übergeben wurde.
Ruft Funktion auf, die Inhalt für den Platzhalter zusammenbaut.
Gibt den Inhalt, der anstelle des Platzhalters kommt, zurück.
*/
TString replacePlaceholder(TString placeholder, int userId, int eventId = 0) {
	TString content = initializeString("");

	if (strcmp(placeholder.string, "[@TABLE]") == 0) {
		addToString(&content, generateEventTableContent(userId));
	}
	else if (strcmp(placeholder.string, "[@USER]") == 0) {
		content = initializeString("Uwe");
	}
	else if (strcmp(placeholder.string, "[@EVENTID]") == 0) {
		char stringEventId[6 + 1] = { 0 };
		itoa(eventId, stringEventId, 10);
		content = initializeString(stringEventId);
	}


	return content;
} 


/*
Baut die neue HTML-Seite zusammen.
Gibt String zurück, in dem alle Platzhalter ersetzt wurden.
*/
char* generateHtmlPage(FILE* htmlFile, int userId, int eventId = 0) {
	// Setzt den Content-Type an den Anfang der HTML-Seite 
	TString htmlPage = initializeString("Content-type:text/html\r\n\r\n");
	TString htmlFromFile = getHtmlFromFile(htmlFile);
	// Fügt den restlichen Inhalt aus dem htmlFromFile hinzu.
	addToString(&htmlPage, htmlFromFile);

	// bei Error wird 0 übergeben
	if (userId != 0) {
		// Fügt die Platzhalter in ein Array ein
		TString htmlPageWithoutPlaceholder = initializeString("");
		TStringArray placeholders = initializeStringArray("[@TABLE]");
		addToStringArray(&placeholders, "[@USER]");
		addToStringArray(&placeholders, "[@EVENTID]");
		TString content = initializeString("");

		fclose(htmlFile);

		// Geht das Array der Platzhalter durch und 
		// lässt die Platzhalter mit dem dem gewünschten Inhalt ersetzten
		for (size_t i = 0; i < placeholders.length; i++)
		{
			copyString(&content, replacePlaceholder(placeholders.strings[i], userId, eventId));
			copyString(&htmlPageWithoutPlaceholder, findAndReplace(htmlPage, placeholders.strings[i], content));
			copyString(&htmlPage, htmlPageWithoutPlaceholder);
			safeDeleteString(&htmlPageWithoutPlaceholder);
		}
		safeDeleteString(&htmlFromFile);
		deleteStringArray(&placeholders);
		safeDeleteString(&content);
	}
	return htmlPage.string;
}

/*
Zerlegt einen Inputstring in die Feldnamen und deren dazugehörigen Werte
Gibt ein TFieldArray mit den einzelnen name-value-Paaren zurück
*/
TFieldArray parseInputString(char* inputString) {
	TField field = initializeField("", "");
	TFieldArray fieldArray;
	TString fieldName = initializeString("");
	TString fieldValue = initializeString("");
	bool isFirst = true;

	for (size_t i = 0; inputString[i] != '\0'; i++)
	{
		if (inputString[i] == '=')
		{
			//field name fertig
			i++;
			for (i; inputString[i] != '&' && inputString[i] != '\0'; i++)
			{
				if (inputString[i] == '%' && inputString[i + 1] == '3' && inputString[i + 2] == 'A') {
					char fieldValueCharacter[2] = "";
					strcpy(fieldValueCharacter, charToSTring(':'));
					addToString(&fieldValue, fieldValueCharacter);
					i += 2;
				}
				else if (inputString[i] == '+') {
					char fieldValueCharacter[2] = "";
					strcpy(fieldValueCharacter, charToSTring(' '));
					addToString(&fieldValue, fieldValueCharacter);
				}
				else{
				char fieldValueCharacter[2] = "";
				strcpy(fieldValueCharacter, charToSTring(inputString[i]));
				addToString(&fieldValue, fieldValueCharacter);

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
			char fieldNameCharacter[2] = "";
			strcpy(fieldNameCharacter, charToSTring(inputString[i]));
			addToString(&fieldName, fieldNameCharacter);
		}
	}

	safeDeleteField(&field);
	safeDeleteString(&fieldName);
	safeDeleteString(&fieldValue);
	return fieldArray;

}

/*
gibt die fertig gebaute html seite aus
nimmt die user und event id
wurde beim ändern des events benötigt
*/
void printOutHtmlPage(char* htmlPath, int userId, int eventId) {
	FILE* htmlFile;
	htmlFile = fopen(htmlPath, "r+");
	if (htmlFile == NULL) {
		cout << "Content-type:text/html\r\n\r\n" << endl;
		cout << "fail";
	}
	TString htmlPage = initializeString(generateHtmlPage(htmlFile, userId, eventId));
	cout << htmlPage.string;
	safeDeleteString(&htmlPage);
	fclose(htmlFile);
}

/*
gibt die fertig gebaute html seite aus
nimmt die user id
*/
void printOutHtmlPage(char* htmlPath) {
	FILE* htmlFile;
	htmlFile = fopen(htmlPath, "r+");
	if (htmlFile == NULL) {
		cout << "Content-type:text/html\r\n\r\n" << endl;
		cout << "fail";
	}
	TString htmlPage = initializeString(generateHtmlPage(htmlFile, 1));
	cout << htmlPage.string;
	safeDeleteString(&htmlPage);
	fclose(htmlFile);
}

int main() {
	try
	{
		int _userId = 1;
		char input[500 + 1] = { 0 };
		cin >> input;
		// nimmt die url die vom request kommt 
		// die konfig auf dem apache ist so eingestellt, dass, wenn eine datei nicht vorhanden ist
		// auf de exe gelinkt wird
		// apache config auf git vorhanden
		char* request_uri = getenv("REQUEST_URI");

		//entscheidung, was passieren soll und wohin geleitet weerden soll
		if (strcmp(request_uri, "/show_events") == 0) {
			printOutHtmlPage("../htdocs/Events.htm");
			return 0;
		} 
		else if (strcmp(request_uri, "/create_events") == 0) {
			printOutHtmlPage("../htdocs/HTMLPage.htm");
			return 0;
		}
		else if (strcmp(request_uri, "/menue") == 0) {
			printOutHtmlPage("../htdocs/Menue.htm");
			return 0;
		}
		else if (strcmp(request_uri, "/add_event") == 0) {

			FILE* eventsFile;
			eventsFile = openEventFile();
			if (eventsFile == NULL)
			{
				errorPage("fehler beim öffnen");
			}
			TFieldArray inputFields = parseInputString(input);
			saveEventInFile(eventsFile, inputFields, _userId);

			fclose(eventsFile);
			printOutHtmlPage("../htdocs/menue.htm");
			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if (strcmp(request_uri, "/delete_event") == 0) {

			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = atoi(inputFields.fields[0].value.string);
			if (deleteEvent(eventsFile, eventId) != 0) {
				errorPage("fehler beim löschen");
			}
			printOutHtmlPage("../htdocs/Events.htm");
			fclose(eventsFile);

			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if(strcmp(request_uri, "/login") == 0)
		{
			errorPage(request_uri);
			return 0;
		}
		else if (strcmp(request_uri, "/change_password") == 0)
		{
			errorPage(request_uri);
			return 0;
		}
		else if (strcmp(request_uri, "/change_event") == 0) {
			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = atoi(inputFields.fields[0].value.string);
			printOutHtmlPage("../htdocs/ChangeEvent.htm", _userId, eventId);
			fclose(eventsFile);
			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if (strcmp(request_uri, "/changed_event") == 0) {
			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = 0;
			for (size_t i = 0; i < inputFields.length; i++)
			{
				if (strcmp(inputFields.fields[i].name.string, "eventId") == 0) {
					eventId = atoi(inputFields.fields[i].value.string);
				}
			}
			changeEvent(eventsFile, inputFields, eventId);
			fclose(eventsFile);
			printOutHtmlPage("../htdocs/Events.htm");
			safeDeleteFieldArray(&inputFields);
			
			return 0;
		}
		else {
			errorPage(request_uri);
			return 1;
		}
	}
	catch (const std::exception&)
	{
		errorPage("boom!");
	}
}