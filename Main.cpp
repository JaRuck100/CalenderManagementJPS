#include <iostream>
#include <stdio.h>
#include "GeneralFuctions.h"
#include "GenerateContentFunctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"

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
TString replacePlaceholder(TString placeholder, int userId) {
	TString content = initializeString("");

	if (strcmp(placeholder.string, "[@TABLE]") == 0) {
		addToString(&content, generateEventTableContent(userId));
	}
	else if (strcmp(placeholder.string, "[@USER]") == 0) {
		content = initializeString("Uwe");
	}
	return content;
}

/*
Baut die neue HTML-Seite zusammen.
Gibt String zurück, in dem alle Platzhalter ersetzt wurden.
*/
char* generateHtmlPage(FILE* htmlFile, int userId) {
	// Setzt den Content-Type an den Anfang der HTML-Seite 
	TString htmlPage = initializeString("Content - Type: text / html\r\n\r\n");
	TString htmlFromFile = getHtmlFromFile(htmlFile);
	// Fügt den restlichen Inhalt aus dem htmlFromFile hinzu.
	addToString(&htmlPage, htmlFromFile);

	// bei Error wird 0 übergeben
	if (userId != 0) {
		// Fügt die Platzhalter in ein Array ein
		TString htmlPageWithoutPlaceholder = initializeString("");
		TStringArray placeholders = initializeStringArray("[@TABLE]");
		addToStringArray(&placeholders, "[@USER]");
		TString content = initializeString("");

		fclose(htmlFile);

		// Geht das Array der Platzhalter durch und 
		// lässt die Platzhalter mit dem dem gewünschten Inhalt ersetzten
		for (size_t i = 0; i < placeholders.length; i++)
		{
			copyString(&content, replacePlaceholder(placeholders.strings[i], userId));
			htmlPageWithoutPlaceholder = findAndReplace(htmlPage, placeholders.strings[i], content);
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
				char fieldValueCharacter[2] = "";
				strcpy(fieldValueCharacter, charToSTring(inputString[i]));
				addToString(&fieldValue, fieldValueCharacter);
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

void main() {
	try
	{


	char input[500] = { 0 };
	//strcpy(input ,"name=bla&date=12.01.17&time=12:34");
	//cin >> input;
	FILE* eventsFile;
	eventsFile = openEventFile();
	//if (eventFile == NULL)
	//{
	//	//return 1;
	//}
	//TFieldArray inputFields = parseInputString(input);
	//saveEventInFile(eventFile, inputFields, 1);
	////saveEventInFile(eventFile, "Party bei Kathrin", "13.02.2017", "18:30", 1);
	////fclose(eventFile);

	////FILE* htmlFile;
	////htmlFile = fopen("Events.htm", "r+");
	////if (htmlFile == NULL) {
	////	cout << "ERROR beim öffnen der Datei" << endl;
	////	return 1;
	////}
	////TString htmlPage = initializeString(generateHtmlPage(htmlFile, 1));
	////// Ausgabe neuer HTML-Seite
	////cout << htmlPage.string;
	////safeDeleteString(&htmlPage);

	////fclose(htmlFile);
	//TField field = initializeField("date", "01.01.17");
	//TFieldArray fieldArray = initializeFieldArray(field);
	//safeDeleteField(&field);
	////field = initializeField("time", "12:34");
	////addToFieldArray(&fieldArray, field);
	////safeDeleteField(&field);
	//field = initializeField("name", "Treffen mit Frida");
	//addToFieldArray(&fieldArray, field);

	////saveEventInFile(eventFile, fieldArray, 1);
	//safeDeleteField(&field);
	////safeDeleteFieldArray(&fieldArray);
	////fseek(eventFile, sizeof(int), SEEK_SET);
	//TEvent* newevent = new TEvent;
	////while (!feof(eventFile))
	////{
	////	fread(newevent, sizeof(TEvent), 1, eventFile);
	////	if (feof(eventFile)) {
	////		continue;
	////	}
	////	if (newevent->userId == 1)
	////	{
	////		cout << newevent->date << endl;
	////		cout << newevent->eventName << endl;
	////		cout << newevent->time << endl;
	////	}
	////}
	//changeEvent(eventFile, fieldArray, 1);
	//fseek(eventFile, sizeof(int), SEEK_SET);
	//cout << "Content-Type: text/html\r\n\r\n";

	//cout << "<html><head><title>CGI Test 2</title></head>";

	//cout << "<body>";

		//fread(newevent, sizeof(TEvent), 1, eventFile);

	/*	cout <<"<p>" << inputFields.fields[0].value.string <<" </p>"<< endl;
		cout << "<p>" << inputFields.fields[1].value.string << " </p>" << endl;
		cout << "<p>" << inputFields.fields[2].value.string << " </p>" << endl;*/
	//cout << input;
	//
	//cout << "</body></html>";
	//safeDeleteFieldArray(&inputFields); 
	//delete newevent;


	//saveEventInFile(eventsFile, "Treffen mit Britta", "22.03.16", "12:45", 1);
	//saveEventInFile(eventsFile, "Treffen mit Tina", "28.03.17", "18:25", 1);
	//saveEventInFile(eventsFile, "Geburstatg Tine", "12.12.18", "08:30", 1);
	//fseek(eventsFile, sizeof(int), SEEK_SET);

	//TEvent* newevent = new TEvent;
	//while (!feof(eventsFile))
	//	{
	//		fread(newevent, sizeof(TEvent), 1, eventsFile);
	//		if (feof(eventsFile)) {
	//			continue;
	//		}
	//		if (newevent->userId == 1)
	//		{
	//			cout << newevent->date << endl;
	//			cout << newevent->eventName << endl;
	//			cout << newevent->time << endl;
	//			cout << newevent->id << endl << endl;
	//		}
	//	}
	//delete newevent;


	fclose(eventsFile);
	
	//return 0;


	}
	catch (const std::exception&)
	{
		FILE* errorPage;
		errorPage = fopen("ErrorPage500.htm", "r");
		cout << generateHtmlPage(errorPage, 0);
	}
}