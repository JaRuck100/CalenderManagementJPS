#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "UserFunctions.h"
#include "EventFunctions.h"

using namespace std;


/*
Öffnet das Userfile
Gibt Pointer auf das geöffnete File zurück
*/
FILE* openUserFile() {
	FILE* userFile;
	//	Prüft ob eine User-Datei vorhanden ist
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
Speichert die Daten eines Users in userFile
Name, Passwort
*/
void saveUserInFile(FILE* userFile, char* userName, char* userPasword) {
	TUser* newUser = new TUser;
	int userIdCounter = getIdCounter(userFile);

	strcpy(newUser->name, userName);
	// Verschlüsselung benötigt
	strcpy(newUser->password, userPasword);
	newUser->id = userIdCounter;

	fseek(userFile, 0, SEEK_END);
	fwrite(newUser, sizeof(TEvent), 1, userFile);
	fseek(userFile, 0, SEEK_SET);
	fwrite(&userIdCounter, sizeof(int), 1, userFile);
	delete newUser;
}