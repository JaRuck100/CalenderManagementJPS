#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"

using namespace std;

/*
�ffnet das Eventfile
Gibt Pointer auf das ge�ffnete File zur�ck
*/
FILE* openEventFile() {
	FILE* eventsFile;
	// Rr�ft, ob Anschriften-Datei vorhaden ist, wenn ja �ffnen
	if (access("Events.txt", 00) == 0) {
		eventsFile = fopen("Events.txt", "r+");
		if (eventsFile == NULL) {
			errorPage("fehler beim �ffnen");
			return NULL;
		}
	}
	// Wenn nicht vorhanden, pr�fen ob eine temp-Datei vorhanden ist
	else if (access("temp.txt", 00) == 0) {
		// wenn vorhanden, veruschen in Event umzubennen 
		if (rename("temp.txt", "Events.txt") != 0) {
			// wennn umbennen nicht funktioniert, erstellen einer neuen Datein und r�berkopeiren der Dateien aus temp 
			FILE* temp;
			temp = fopen("temp.txt", "r");
			eventsFile = fopen("Events.txt", "w+");
			if (eventsFile == NULL || temp == NULL) {
				errorPage("fehler beim �ffnen");
				return NULL;
			}

			TEvent* tempEvent = new TEvent;
			if (tempEvent == NULL) {
				errorPage("fehler beim erstellen");
				return NULL;
			}
			int eventIdCounter = 0;
			fread(&eventIdCounter, sizeof(int), 1, temp);
			fwrite(&eventIdCounter, sizeof(int), 1, eventsFile);
			while (!feof(temp))
			{
				fread(tempEvent, sizeof(TEvent), 1, temp);
				if (feof(temp)) {
					continue;
				}
				fwrite(tempEvent, sizeof(TEvent), 1, eventsFile);

				tempEvent = new TEvent;
				if (tempEvent == NULL) {
					return NULL;
				}
			}
			delete tempEvent;
			fclose(temp);
			fclose(eventsFile);
			remove("temp.txt");
		}
		// wenn Umbennung funktioniert, �ffnen der Event-Datei
		else {
			eventsFile = fopen("Events.txt", "r+");
			if (eventsFile == NULL) {
				return NULL;
			}
		}
	}
	// Wenn keine Dateien vorhanden sind, Event-Datei neu erstellen
	else {
		eventsFile = fopen("Events.txt", "w+");
		if (eventsFile == NULL) {
			return NULL;
		}
		int eventIdCounter = 0;
		fwrite(&eventIdCounter, sizeof(int), 1, eventsFile);

	}
	return eventsFile;
}

/*
Speichert die Daten eines Events in eventsFile
Name, Datum, Zeit, User-ID
*/
void saveEventInFile(FILE* eventsFile, char* eventName, char* eventDate, char* eventTime, int userId) {
	TEvent* newEvent = new TEvent;
	int eventIdCounter = getIdCounter(eventsFile);

	strcpy(newEvent->eventName, eventName);
	strcpy(newEvent->date, eventDate);
	strcpy(newEvent->time, eventTime);
	newEvent->userId = userId;
	newEvent->id = eventIdCounter;

	fseek(eventsFile, 0, SEEK_END);
	fwrite(newEvent, sizeof(TEvent), 1, eventsFile);
	fseek(eventsFile, 0, SEEK_SET);
	fwrite(&eventIdCounter, sizeof(int), 1, eventsFile);

	delete newEvent;
}

/*
Speichert die Daten eines Events in eventsFile
Nutzt ein Feldarray, in dem die Formularfelder mit Wert gespeichert sind
*/
void saveEventInFile(FILE* eventsFile, TFieldArray fieldArray, int userId) {
	TEvent* newEvent = new TEvent;
	int eventIdCounter = getIdCounter(eventsFile);
	//setzt datum, uhrzeit und name
	buildEventFromFileArray(newEvent, fieldArray);
	newEvent->userId = userId;
	newEvent->id = eventIdCounter;

	fseek(eventsFile, 0, SEEK_END);
	fwrite(newEvent, sizeof(TEvent), 1, eventsFile);
	fseek(eventsFile, 0, SEEK_SET);
	fwrite(&eventIdCounter, sizeof(int), 1, eventsFile);
	delete newEvent;
}

/*Sucht im EventFile nach einem Event by id
//Gibt die Position des Events im File zur�ck oder NULL
//*/
//FILE* searchForEventPosition(FILE* eventsFile, int eventId) {
//	TEvent* targetEvent = new TEvent;
//	fseek(eventsFile, sizeof(int), SEEK_SET);
//
//	while (!feof(eventsFile))
//	{
//		fread(targetEvent, sizeof(TEvent), 1, eventsFile);
//		if (feof(eventsFile)) {
//			continue;
//		}
//		if (targetEvent->id == eventId) {
//			fseek(eventsFile, -sizeof(TEvent), SEEK_CUR);
//			return eventsFile;
//		}
//	}
//	delete targetEvent;
//	return NULL;
//}

/*
baut aus einem filearray die werte in ein �bergebenes Event
*/
void buildEventFromFileArray(TEvent* event, TFieldArray fieldArray) {
	for (size_t i = 0; i < fieldArray.length; i++)
	{
		if (strcmp(fieldArray.fields[i].name.string, "name") == 0) {
			strcpy(event->eventName, fieldArray.fields[i].value.string);
		}

		if (strcmp(fieldArray.fields[i].name.string, "date") == 0) {
			strcpy(event->date, fieldArray.fields[i].value.string);
		}

		if (strcmp(fieldArray.fields[i].name.string, "time") == 0) {
			strcpy(event->time, fieldArray.fields[i].value.string);
		}
	}
}


/*
aucht in der Datei nach dem Event mit eventId
�ndert die Werte des events, die im FileArray array liegen, rest bleibt 
speichert das ge�nderete Event an die Stelle des alten Events
*/

int changeEvent(FILE* eventsFile, TFieldArray fieldArray) {
	//event-id rauasasuchen
	TEvent* targetEvent = new TEvent;
	fseek(eventsFile, sizeof(int), SEEK_SET);
	bool eventFound = false;
	while (!feof(eventsFile) && !eventFound)
	{
		fread(targetEvent, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile)) {
			continue;
		}
		if (targetEvent->id == eventId) {
			fseek(eventsFile, -sizeof(TEvent), SEEK_CUR);
			buildEventFromFileArray(targetEvent, fieldArray);
			fwrite(targetEvent, sizeof(TEvent), 1, eventsFile);
			eventFound = true;
			continue;
		}
	}
	delete targetEvent;
	if (eventFound) {
		return 0;
	}
	else {
		return 1;
	}

}

/*
l�scht das Event mit der Id eventId aus der Event-Datei
*/
int deleteEvent(FILE* eventsFile, int eventId) {
	FILE* temp;
	temp = fopen("temp.txt", "w");
	if (temp == NULL) {
		return 1;
	}

	fseek(eventsFile, 0, SEEK_SET);
	int EventIdCounter;
	fread(&EventIdCounter, sizeof(int), 1, eventsFile);
	fwrite(&EventIdCounter, sizeof(int), 1, temp);

	TEvent* event;
	while (!feof(eventsFile))
	{
		event = new TEvent;
		fread(event, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile) || event->id == eventId) {
			continue;
		}
		fwrite(event, sizeof(TEvent), 1, temp);
		delete event;
	}

	fclose(temp);
	fclose(eventsFile);
	if (remove("Events.txt") != 0) {
		errorPage("l�schen fehlgeschlagen");
		return 2;
	}
	if (rename("temp.txt", "Events.txt") != 0) {
		errorPage("umbennenung fehlgeschlagen");
		return 2;
	}
	eventsFile = openEventFile();

	delete event;
	return 0;
}