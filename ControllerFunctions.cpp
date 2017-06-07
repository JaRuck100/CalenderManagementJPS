#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"
#include "UserFunctions.h"

#pragma warning(disable:4996)
using namespace std;

/*
Registiert neuen User für die Anwendung
*/
void registerFunction(TFieldArray inputFields) {
	TUser* newUser = new TUser;
	char repetedPassword[40] = { 0 };
	for (size_t i = 1; i < inputFields.length; i++) {
		if (strcmp(inputFields.fields[i].name.string, "username") == 0) {
			strcpy(newUser->name, inputFields.fields[i].value.string);
		}
		else if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
			strcpy(newUser->password, inputFields.fields[i].value.string);
		}
		else if (strcmp(inputFields.fields[i].name.string, "passwordRepeat") == 0) {
			strcpy(repetedPassword, inputFields.fields[i].value.string);
		}
	}
	newUser->id = 0;
	FILE* userFile;
	userFile = openUserFile();
	bool isNewUser = true;
	if (userAlreadyExisting(userFile, newUser) == true) {
		cout << "Content-type:text/html\r\n\r\n";
		//Username schon vorhanden
		isNewUser = false;
		cout << "nu";
	}
	else if (strcmp(newUser->password, repetedPassword) != 0)
	{
		cout << "Content-type:text/html\r\n\r\n";
		//Passwort nicht richtig
		cout << "np";
		isNewUser = false;
	}
	else {
		isNewUser = true;
	}

	if (isNewUser == true) {
		// verschlüsseldung bisher nicht vorhanden
		int userIdCounter = 0;
		fseek(userFile, 0, SEEK_SET);
		fread(&userIdCounter, sizeof(int), 1, userFile);
		userIdCounter++;
		newUser->id = userIdCounter;
		fseek(userFile, 0, SEEK_END);
		fwrite(newUser, sizeof(TUser), 1, userFile);
		fseek(userFile, 0, SEEK_SET);
		fwrite(&userIdCounter, sizeof(int), 1, userFile);
		fclose(userFile);

		cout << "Content-type:text/html\r\n\r\n";
		cout << "y";
	}
	delete newUser;
	fclose(userFile);
}

/*
Prüft, ob der User noch eingeloggt ist
*/
void ceckUserFunction(TFieldArray inputFields, int userId) {
	if (strcmp(inputFields.fields[1].name.string, "id") == 0) {
		int ceckUserId = atoi(inputFields.fields[1].value.string);
		if (userId == ceckUserId) {
			cout << "Content-type:text/html\r\n\r\n";
			cout << "y";
		}
		else {
			cout << "Content-type:text/html\r\n\r\n";
			cout << "n";
		}
	}
	else {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
}

/*
Liest alle Events aus der Event-Datei
Baut aus den Events des Useres ein String Json-Array zusammen
*/
void showEventsFunction(TFieldArray inputFields, int userId) {
	TString eventsJson = initializeString("");
	FILE* eventsFile = openEventFile();
	TEvent* event = new TEvent;
	fseek(eventsFile, sizeof(int), SEEK_SET);
	addToString(&eventsJson, "[ ");
	while (!feof(eventsFile)) {
		fread(event, sizeof(TEvent), 1, eventsFile);
		if (feof(eventsFile) || event->userId != userId) {
			continue;
		}
		buildJson(&eventsJson, event);
		fread(event, sizeof(TEvent), 1, eventsFile);
		if (!feof(eventsFile)) {
			long sizeOfTEvent = sizeof(TEvent);
			addToString(&eventsJson, ",");
			fseek(eventsFile, -sizeOfTEvent, SEEK_CUR);
		}
	}
	addToString(&eventsJson, " ]");
	cout << "Content-type:text/html\r\n\r\n";
	cout << eventsJson.string;
	fclose(eventsFile);
	delete event;
}

/*
Löscht das Event mit der entsprechenden Id
*/
void deleteEventFunction(TFieldArray inputFields) {
	int eventId = 0;

	if (strcmp(inputFields.fields[1].name.string, "id") == 0) {
		eventId = atoi(inputFields.fields[1].value.string);
		FILE* eventFile = openEventFile();

		if (deleteEvent(eventFile, eventId) == 0) {
			cout << "Content-type:text/html\r\n\r\n";
			cout << "y";
		}
		else
		{
			cout << "Content-type:text/html\r\n\r\n";
			cout << "n";
		}
		fclose(eventFile);
	}
}

/*
Legt ein neues Event für den User an
*/
void addEventFunction(TFieldArray inputFields, int userId) {
	FILE* eventsFile;
	eventsFile = openEventFile();
	if (eventsFile == NULL) {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
	else {
		saveEventInFile(eventsFile, inputFields, userId);
		cout << "Content-type:text/html\r\n\r\n";
		cout << "y";
	}
	fclose(eventsFile);
}

/*
Sucht in der Event-Datei nach dem Event mit der entsprechenden Id
Gibt gebautes String Json-Object aus 
*/
void searchForEventFunction(TFieldArray inputFields) {
	TString eventJson = initializeString("");
	int eventId = 0;
	if (strcmp(inputFields.fields[1].name.string, "id") == 0) {
		eventId = atoi(inputFields.fields[1].value.string);
		FILE* eventsFile;
		eventsFile = openEventFile();
		fseek(eventsFile, sizeof(int), SEEK_SET);
		TEvent* searchedEvent = new TEvent;
		bool eventFound = false;
		while (!feof(eventsFile) && !eventFound)
		{
			fread(searchedEvent, sizeof(TEvent), 1, eventsFile);
			if (feof(eventsFile)) {
				continue;
			}
			if (searchedEvent->id == eventId) {
				eventFound = true;
				buildJson(&eventJson, searchedEvent);
			}
		}
		delete searchedEvent;
		fclose(eventsFile);
	}
	cout << "Content-type:text/html\r\n\r\n";
	cout << eventJson.string;
	safeDeleteString(&eventJson);
}

/*
Speichert das geänderte Event in der Event-Datei
*/
void changeEventFunction(TFieldArray inputFields) {
	FILE* eventsFile;
	eventsFile = openEventFile();
	if (changeEvent(eventsFile, inputFields) == 0) {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "y";
	}
	else
	{
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
	fclose(eventsFile);

}

/*
Speichert das geänderte Loginpassword des Users in der User-Datei
*/
void changePasswordFunction(TFieldArray inputFields, int userId) {
	TUser* user = new TUser;
	user = findUser(userId);
	if (user == NULL) {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
	else {
		char oldPassword[40 + 1] = { 0 };
		char newPassword[40 + 1] = { 0 };
		char passwordRepeat[40 + 1] = { 0 };
		for (size_t i = 1; i < inputFields.length; i++) {
			if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
				strcpy(oldPassword, inputFields.fields[i].value.string);
			}
			else if (strcmp(inputFields.fields[i].name.string, "newPassword") == 0) {
				strcpy(newPassword, inputFields.fields[i].value.string);
			}
			else if (strcmp(inputFields.fields[i].name.string, "passwordRepeat") == 0) {
				strcpy(passwordRepeat, inputFields.fields[i].value.string);
			}
		}
		bool rightOldPassword = (strcmp(user->password, oldPassword) == 0);
		bool rightNewPassword = (strcmp(newPassword, passwordRepeat) == 0);

		if (rightOldPassword && rightNewPassword) {
			cout << "Content-type:text/html\r\n\r\n";
			cout << "y";
			strcpy(user->password, newPassword);
			FILE* userInFileLocation = findUserDataLocation(*user);
			if (userInFileLocation == NULL) {
				cout << "Content-type:text/html\r\n\r\n";
				cout << "n";
			}
			else {
				saveUserInFile(userInFileLocation, user);
				fclose(userInFileLocation);
			}
		}
		else {
			cout << "Content-type:text/html\r\n\r\n";
			cout << "n";
		}
	}
	delete user;
}

/*
Loggt den User durch löschen der Datei für den momentanen User aus
*/
void logoutFunction(TFieldArray inputFields) {
	if (remove("Current_User.txt") == 0) {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "y";
	}
	else {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
}

/*
Loggt den user über Benutzername und password sein
Prüft, ob der User existiert und ob das Passwort korrekt ist
*/
void loginFunction(TFieldArray inputFields, FILE* currentUserFile) {
	currentUserFile = fopen("Current_User.txt", "w");
	if (currentUserFile == NULL) {
		// Session Probleme
	}
	FILE* userFile;
	userFile = openUserFile();
	TUser* userLogin = new TUser;
	for (size_t i = 1; i < inputFields.length; i++) {
		if (strcmp(inputFields.fields[i].name.string, "username") == 0) {
			strcpy(userLogin->name, inputFields.fields[i].value.string);
			continue;
		}
		else if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
			strcpy(userLogin->password, inputFields.fields[i].value.string);
			continue;
		}
	}
	fseek(userFile, sizeof(int), SEEK_SET);
	int userId = findUserByName(userFile, *userLogin);
	if (userId == -1) {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "n";
	}
	else {
		cout << "Content-type:text/html\r\n\r\n";
		cout << "y;" << userId;
		fwrite(&userId, sizeof(int), 1, currentUserFile);
	}
	delete userLogin;
	fclose(userFile);
	fclose(currentUserFile);
}
