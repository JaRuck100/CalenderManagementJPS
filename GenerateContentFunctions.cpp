#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"


#pragma warning(disable:4996)
using namespace std;


/*
	Erzeugt den Inhalt für die Eventtabelle abhägig von userId
	Gibt den in HTML formatierten INhalt der Tabelle zurück
*/
TString generateEventTableContent(int userId) {

	TString tableContent = initializeString("");
	FILE* eventsFile;
	eventsFile = openEventFile();
	if (eventsFile == NULL)
	{
		return tableContent;
	}

	// Auslesen der Events eines Users
	// Überspringt die int am Anfang der Datei,
	// in dem der Id-Zähler für die Events liegt
	fseek(eventsFile, sizeof(int), SEEK_SET);
	TEvent* userEvent = new TEvent;
	if (userEvent == NULL) {
		delete userEvent;
		return tableContent;
	}

	while (!feof(eventsFile))
	{
		fread(userEvent, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile)) {
			continue;
		}

		/* Erzeugt Tabellenzeile, in der Die Daten des Events angezeigt werden
		   Datum
		   Uhrzeit
		   Name des Events
		*/
		if (userEvent->userId == userId) {
			addToString(&tableContent, "        <tr>\n");
			generateColumnOfRow(&tableContent, userEvent->date);
			char timeString[9 + 1];
			strcpy(timeString, userEvent->time);
			strcat(timeString, " Uhr");
			generateColumnOfRow(&tableContent, timeString);
			generateColumnOfRow(&tableContent, userEvent->eventName);
			addToString(&tableContent, "        </tr>\n");
		}

	}
	delete userEvent;
	fclose(eventsFile);

	return tableContent;
}