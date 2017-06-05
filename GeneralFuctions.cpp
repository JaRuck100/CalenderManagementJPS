#include <iostream>
#include <stdio.h>
#include "GeneralFuctions.h"

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
Generiert die Spalte einer Zeile einer Html Tabelle mit dem Inhalt von tableColumnOfRowContent
Speichert dem String in tableContent
*/
void generateColumnOfRow(TString* tableContent, char* tableColumnOfRowContent) {
	addToString(tableContent, "            <td>");
	addToString(tableContent, tableColumnOfRowContent);
	addToString(tableContent, "</td>\n");
}

/*
Liest die Zähler-ID aus einem File
*/
int getIdCounter(FILE* file) {
	int idCounter = 0;
	fseek(file, 0, SEEK_SET);
	fread(&idCounter, sizeof(int), 1, file);
	idCounter++;
	return idCounter;
}

void errorPage(char* reason) {
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<html> \n\t<head> \n\t\t<title> Error</title> \n\t</head>\n";
	cout << "\t<body> \n\t\t<h1>500 Internal Server Error</h1>\n";
	cout << "\t\t<p>" << reason << "</p>\n";
	cout << "\t</body>\n";
	cout << "</html>";
}