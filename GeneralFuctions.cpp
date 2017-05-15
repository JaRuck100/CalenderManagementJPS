#include "GeneralFuctions.h"
#include "StringLib.h"
#include <stdio.h>
#include <iostream>
#include <io.h>

using namespace std;

/*
	�ffnet das Userfile
	Gibt Pointer auf das ge�ffnete File zur�ck
*/
FILE* openUserFile() {
	FILE* userFile;
	//	Pr�ft ob eine User-Datei vorhanden ist
	if (access("User.txt", 00) == 0) {
		userFile = fopen("User.txt", "r+");
		if (userFile == NULL) {
			fclose(userFile);
			return NULL;
		}
	}
	// Wenn nicht wird eine neu erstellt
	else {
		userFile = fopen("User.txt", "w+");
		if (userFile == NULL) {
			fclose(userFile);
			return NULL;
		}
		int userIdCounter = 0;
		fwrite(&userIdCounter, sizeof(int), 1, userFile);
	}
	return userFile;
}

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
	Generiert die Spalte einer Zeile einer Html Tabelle mit dem Inhalt von tableColumnOfRowContent
	Speichert dem String in tableContent
*/
void generateColumnOfRow(TString* tableContent, char* tableColumnOfRowContent) {
	addToString(tableContent, "            <td>");
	addToString(tableContent, tableColumnOfRowContent);
	addToString(tableContent, "</td>\n");
}

/*
	Liest die Z�hler-ID aus einem File
*/
int getIdCounter(FILE* file) {
	int idCounter = 0;
	fseek(file, 0, SEEK_SET);
	fread(&idCounter, sizeof(int), 1, file);
	idCounter++;
	return idCounter;
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
	Speichert die Daten eines Users in userFile
	Name, Passwort
*/
void saveUserInFile(FILE* userFile, char* userName, char* userPasword) {
	TUser* newUser = new TUser;
	int userIdCounter = getIdCounter(userFile);
	
	strcpy(newUser->name, userName);
	// Verschl�sselung ben�tigt
	strcpy(newUser->password, userPasword);
	newUser->id = userIdCounter;

	fseek(userFile, 0, SEEK_END);
	fwrite(newUser, sizeof(TEvent), 1, userFile);
	fseek(userFile, 0, SEEK_SET);
	fwrite(&userIdCounter, sizeof(int), 1, userFile);
	delete newUser;
}

/*
Konvertiert einen char zu einem String
Gibt String zur�ck
*/
char* charToSTring(char character) {
	char string[2] = "";
	string[0] = character;
	string[1] = '\0';
	return string;
}
