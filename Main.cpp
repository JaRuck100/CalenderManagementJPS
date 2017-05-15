#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "GenerateContentFunctions.h"

#pragma warning(disable:4996)
using namespace std;


/*
	Schreibt den Inhalt von htmlFile in einen TString.
*/
TString getHtmlFromFile(FILE* htmlFile) {
	char htmlPage[500 + 1];
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
	htmlPage[a] = '\0';
	TString htmlPageString = initializeString(htmlPage);
	return htmlPageString;
}

/*
	Splitet die Seite am Platzhalter auf.
	Gibt Platzhalter zurück.
*/
//TString htmlPageSplit(TString htmlPageString, TString* abovePlaceholderHtml, TString* underPlaceholderHtml) {
//	TString placeholder = initializeString("");
//	int length = htmlPageString.length;
//
//	for (int i = 0; i < length; i++)
//	{
//		if (htmlPageString.string[i] == '[' && htmlPageString.string[i + 1] == '@') {
//			int j = i + 2;
//			for (j; htmlPageString.string[j] != ']'; j++)
//			{
//
//				char placeholderCharacterString[2] = "";
//				strcpy(placeholderCharacterString, charToSTring(htmlPageString.string[j]));
//				addToString(&placeholder, placeholderCharacterString);
//			}
//			i = j + 1;
//			for (int k = i; k < length; k++)
//			{
//				char underPlaceholderHtmlCharacterString[2] = "";
//				strcpy(underPlaceholderHtmlCharacterString, charToSTring(htmlPageString.string[k]));
//				addToString(underPlaceholderHtml, underPlaceholderHtmlCharacterString);
//			}
//			i = length;
//		}
//		else
//		{
//			char abovePlaceholderHtmlCharacterString[2] = "";
//			strcpy(abovePlaceholderHtmlCharacterString, charToSTring(htmlPageString.string[i]));
//			addToString(abovePlaceholderHtml, abovePlaceholderHtmlCharacterString);
//		}
//	}
//	return placeholder;
//}

/*
	Prüft welcher placeholder übergeben wurde.
	Ruft Funktion auf, die Inhalt für den Platzhalter zusammenbaut.
	Gibt den Inhalt, der anstelle des Platzhalters kommt, zurück.
*/
TString replacePlaceholder(TString placeholder, int userId) {
	TString content = initializeString("");

	if (strcmp( placeholder.string, "[@TABLE]") == 0) {
		addToString(&content, generateEventTableContent(userId));
	}
	else if (strcmp(placeholder.string, "[@USER]") == 0) {
		content = initializeString( "Uwe");
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

	return htmlPage.string;
}

int main() {
	//FILE* eventFile;
	//eventFile = openEventFile();
	//if (eventFile == NULL)
	//{
	//	return 1;
	//}
	//saveEventInFile(eventFile, "Party bei Kathrin", "13.02.2017", "18:30", 1);
	//fclose(eventFile);

	FILE* htmlFile;
	htmlFile = fopen("Events.htm", "r+");
	if (htmlFile == NULL) {
		cout << "ERROR beim öffnen der Datei" << endl;
		return 1;
	}
	TString htmlPage = initializeString(generateHtmlPage(htmlFile, 1));
	// Ausgabe neuer HTML-Seite
	cout << htmlPage.string;
	safeDeleteString(&htmlPage);

	fclose(htmlFile);
	return 0;
}