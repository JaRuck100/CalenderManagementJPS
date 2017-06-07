#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"

#pragma warning(disable:4996)
using namespace std;

/*
Öffnet das Eventfile
Gibt Pointer auf das geöffnete File zurück
*/
FILE* openEventFile() {
	FILE* eventsFile;
	// Rrüft, ob Anschriften-Datei vorhaden ist, wenn ja öffnen
	if (access("Events.txt", 00) == 0) {
		eventsFile = fopen("Events.txt", "r+");
		if (eventsFile == NULL) {
			return NULL;
		}
	}
	// Wenn nicht vorhanden, prüfen ob eine temp-Datei vorhanden ist
	else if (access("temp.txt", 00) == 0) {
		// wenn vorhanden, veruschen in Event umzubennen 
		if (rename("temp.txt", "Events.txt") != 0) {
			// wennn umbennen nicht funktioniert, erstellen einer neuen Datein und rüberkopeiren der Dateien aus temp 
			FILE* temp;
			temp = fopen("temp.txt", "r");
			eventsFile = fopen("Events.txt", "w+");
			if (eventsFile == NULL || temp == NULL) {
				return NULL;
			}

			TEvent* tempEvent = new TEvent;
			if (tempEvent == NULL) {
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
		// wenn Umbennung funktioniert, öffnen der Event-Datei
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
Nutzt ein Fieldarray, in dem die Formularfelder mit Wert gespeichert sind
*/
void saveEventInFile(FILE* eventsFile, TFieldArray fieldArray, int userId) {
	TEvent* newEvent = new TEvent;
	int eventIdCounter = getIdCounter(eventsFile);
	buildEventFromFileArray(newEvent, fieldArray);
	newEvent->userId = userId;
	newEvent->id = eventIdCounter;

	fseek(eventsFile, 0, SEEK_END);
	fwrite(newEvent, sizeof(TEvent), 1, eventsFile);
	fseek(eventsFile, 0, SEEK_SET);
	fwrite(&eventIdCounter, sizeof(int), 1, eventsFile);
	delete newEvent;
}

/*
Baut die Werte aus einem fieldarray in ein übergebenes Event
*/
void buildEventFromFileArray(TEvent* event, TFieldArray fieldArray) {
	for (size_t i = 1; i < fieldArray.length; i++)
	{
		if (strcmp(fieldArray.fields[i].name.string, "title") == 0) {
			strcpy(event->title, fieldArray.fields[i].value.string);
		}else if (strcmp(fieldArray.fields[i].name.string, "start") == 0) {
			strcpy(event->start, fieldArray.fields[i].value.string);
		}else if (strcmp(fieldArray.fields[i].name.string, "end") == 0) {
			strcpy(event->end, fieldArray.fields[i].value.string);
		}else if (strcmp(fieldArray.fields[i].name.string, "id") == 0) {
			event->id = atoi(fieldArray.fields[i].value.string);
		}else if (strcmp(fieldArray.fields[i].name.string, "description") == 0) {
			strcpy(event->description, fieldArray.fields[i].value.string);
		}
	}
}

/*
Läd Events aus der Datei und sucht nach dem Event mit der id aus dem Fieldarray
Speichert das neue/geänderte Event an die Stelle des alten Events
*/
int changeEvent(FILE* eventsFile, TFieldArray fieldArray) {
	TEvent* targetEvent = new TEvent;
	TEvent* changedEvent = new TEvent;
	buildEventFromFileArray(changedEvent, fieldArray);
	fseek(eventsFile, sizeof(int), SEEK_SET);
	bool eventFound = false;
	while (!feof(eventsFile) && !eventFound)
	{
		fread(targetEvent, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile)) {
			break;
		}
		if (targetEvent->id == changedEvent->id) {
			long sizeOfTEvent = sizeof(TEvent);
			fseek(eventsFile, -sizeOfTEvent, SEEK_CUR);	
			changedEvent->userId = targetEvent->userId;
			fwrite(changedEvent, sizeof(TEvent), 1, eventsFile);
			eventFound = true;
			break;
		}
	}
	delete targetEvent;
	delete changedEvent;
	if (eventFound) {
		return 0;
	}
	else {
		return 1;
	}
}

/*
Löscht das Event mit der Id eventId aus der Event-Datei
Erstellt eine temp Datei, speichert alle außer das Event mit eventid in tmp
Löscht Event-Datei und benennt temp in Event-Datei um 
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
	TEvent* event = new TEvent;
	while (!feof(eventsFile))
	{
		fread(event, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile) || event->id == eventId) {
			continue;
		}
		fwrite(event, sizeof(TEvent), 1, temp);
	}
	delete event;
	fclose(temp);
	fclose(eventsFile);
	if (remove("Events.txt") != 0) {
		return 2;
	}
	if (rename("temp.txt", "Events.txt") != 0) {
		return 2;
	}
	eventsFile = openEventFile();
	return 0;
}