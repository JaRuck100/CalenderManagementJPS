#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "EventFunctions.h"


#pragma warning(disable:4996)
using namespace std;


/*
Erzeugt den Inhalt f�r die Eventtabelle abh�gig von userId
Gibt den in HTML formatierten INhalt der Tabelle zur�ck
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
	// �berspringt die int am Anfang der Datei,
	// in dem der Id-Z�hler f�r die Events liegt
	fseek(eventsFile, sizeof(int), SEEK_SET);
	TEvent* userEvent = new TEvent;
	if (userEvent == NULL) {
		return tableContent;
	}
	//FILE* tableHtmlFile;
	//tableHtmlFile = fopen("TableHtml.htm", "r");
	//if (tableHtmlFile == NULL)
	//{
	//	return tableContent;
	//}
	//TString tableHtml = getHtmlFromFile(tableHtmlFile);
	//TString tableHtmlCopy = initializeString("");
	//copyString(&tableHtmlCopy, tableHtml);

	while (!feof(eventsFile))
	{
		//copyString(&tableHtmlCopy, tableHtml);
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
			char stringEventId[6 + 1] = { 0 };
			itoa(userEvent->id, stringEventId, 10);
			addToString(&tableContent, "        <tr>\n");
			generateColumnOfRow(&tableContent, userEvent->date);
			TString timeString = initializeString(userEvent->time);
			addToString(&timeString, " Uhr");
			generateColumnOfRow(&tableContent, timeString.string);
			generateColumnOfRow(&tableContent, userEvent->eventName);
			TString chnageButtonString = initializeString("\n                <form name='Test' action='/change_event' method=post> \n");
			addToString(&chnageButtonString, "                <input type=hidden name='eventId'>\n");
			addToString(&chnageButtonString, "                <input type = 'submit' value = '�ndern' onclick = 'eventId.value = ");
			addToString(&chnageButtonString, stringEventId);
			addToString(&chnageButtonString, "'>\n");
			addToString(&chnageButtonString, "                </form>\n            ");
			generateColumnOfRow(&tableContent, chnageButtonString.string);
			generateColumnOfRow(&tableContent, " ");
			TString  deleteButtonString = initializeString("\n                <form name='Test' action='/delete_event' method=post> \n");
			addToString(&deleteButtonString, "                <input type=hidden name='eventId'>\n");
			addToString(&deleteButtonString, "                <input type = 'submit' value = 'l�schen' onclick = 'eventId.value = ");
			// limitierung auf 999.999 Events
			// bei 1.000.000 -> boom!
			// keine bessere L�sung au�er itoa() selbst schreiben
			addToString(&deleteButtonString, stringEventId);
			addToString(&deleteButtonString, "'>\n");
			addToString(&deleteButtonString, "                </form>\n            ");
			generateColumnOfRow(&tableContent, deleteButtonString.string);
			addToString(&tableContent, "        </tr>\n");
		}

	}
	//safeDeleteString(&tableHtml);
	//safeDeleteString(&tableHtmlCopy);
	delete userEvent;
	fclose(eventsFile);
	//fclose(tableHtmlFile);

	return tableContent;
}