#include <iostream>
#include <stdio.h>
#include <io.h>
#include "UserFunctions.h"
#include "StringLib.h"

#pragma warning(disable:4996)
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
			return NULL;
		}
	}
	// Wenn nicht wird eine neu erstellt
	else {
		userFile = fopen("User.txt", "w+");
		if (userFile == NULL) {
			return NULL;
		}
		int userIdCounter = 0;
		fwrite(&userIdCounter, sizeof(int), 1, userFile);
	}
	return userFile;
}

/*
Sucht in der Datei nach dem Namen des Useres
Ist Name vorhanden, prüfen ob Password richtig ist -> Rückgabe der User-Id
Sonst Rückgabe 0
*/
int findUserByName(FILE* userFile, TUser user) {
	TUser* userInFile = new TUser;
	while (!feof(userFile))
	{
		fread(userInFile, sizeof(TUser), 1, userFile);
		if (feof(userFile)) {
			break;
		}
		if (strcmp(userInFile->name, user.name) == 0) {
			if (strcmp(userInFile->password, user.password) == 0) {
				return userInFile->id;
			}
			else {
				break;
			}
		}
	}
	delete userInFile;
	return 0;
}

/*
Prüft, ob es schon einen user mit dem Usernamen gibt
Gibt true zurück, wenn Name schon vorhanden
*/
bool userAlreadyExisting(FILE* userFile, TUser* user) {
	fseek(userFile, sizeof(int), SEEK_SET);
	bool userFound = false;
	TUser* searchUser = new TUser;
	while (!feof(userFile) && !userFound)
	{
		fread(searchUser, sizeof(TUser), 1, userFile);
		if (feof(userFile)) {
			continue;
		}
		if (strcmp(user->name, searchUser->name) == 0) {
			userFound = true;
		}
	}
	delete searchUser;
	return userFound;
}

/*
Liest die Zähler-ID aus dem File
*/
int getUserIdCounter(FILE* file) {
	long temp = ftell(file);
	int idCounter;
	fseek(file, 0, SEEK_SET);
	fread(&idCounter, sizeof(int), 1, file);
	fseek(file, temp, SEEK_SET);
	return idCounter;
}

/*
Speichert die Daten eines Users an der Stelle userlocationInFile im File.
ID-Management ist enthalten. Bei keiner (0), oder einer zu großen, wird eine passende ID eingefügt.
Speicherort, Name, Passwort
*/
void saveUserInFile(FILE* userlocationInFile, TUser* userToSave) {
	int userIdCounter = getUserIdCounter(userlocationInFile);
	if (userToSave->id == NULL || userToSave->id >userIdCounter || userToSave->id < 0)
	{
		userIdCounter++;
		userToSave->id = userIdCounter;
	}
	fwrite(userToSave, sizeof(TUser), 1, userlocationInFile);
	fseek(userlocationInFile, 0, SEEK_SET);
	fwrite(&userIdCounter, sizeof(int), 1, userlocationInFile);
}

/*
Findet User per ID aus Userfile.
Parameter TUser, in den geschrieben wird; ID des Users
NULL, wenn keiner gefunden wurde
*/
TUser* findUser(int userId) {
	FILE* f = openUserFile();
	TUser* tempUser = new TUser;
	fseek(f, sizeof(int), SEEK_SET);
	while (!feof(f))
	{
		fread(tempUser, sizeof(TUser), 1, f);
		if (feof(f))
		{
			continue;
		}
		if (tempUser->id == userId)
		{
			fclose(f);
			return tempUser;
		}
	}
	fclose(f);
	return NULL;
}

/*
Findet User nach seiner ID im UserFile und gibt dessen Stelle zurück oder eof.
*/
FILE* findUserDataLocation(TUser userToFind) {
	FILE* f = openUserFile();
	TUser* tempUser = new TUser;
	fseek(f, sizeof(int), SEEK_SET);
	while (!feof(f))
	{
		fread(tempUser, sizeof(TUser), 1, f);
		if (feof(f))
		{
			continue;
		}
		if (tempUser->id == userToFind.id)
		{
			delete tempUser;
			long sizeOfTUser = sizeof(TUser);
			fseek(f, -sizeOfTUser, SEEK_CUR);
			return f;
		}
	}
	delete tempUser;
	return f;
}

/*
verschlüsselt das übergebene Passwort und vergleicht dann die Übereinstimmung
*/
bool checkPassword(TUser user, TString passwordToCheck) {
	return compareString(initializeString(user.password), enDecode(passwordToCheck, false));
}

/*
local
speichert user mit neuem password, egal ob neuer user oder vorhandener
*/
void saveNewPassword(TUser *user, TString npassword) {
	FILE* userinfile = findUserDataLocation(*user);
	npassword = enDecode(npassword, false);
	strcpy(user->password, npassword.string);
	saveUserInFile(userinfile, user);
}

/*
braucht aktuellen user, altes password zur bestätigung und zwei mal das neue password zur kontrolle
gibt false zurück, wenn das alte password falsch ist oder die neuen nicht übereinstimmen
*/
bool alterUserpassword(TUser *user, TString opassword, TString npassword1, TString npassword2) {
	if (checkPassword(*user, opassword))
	{
		if (compareString(npassword1, npassword2))
		{
			saveNewPassword(user, npassword1);
			return true;
		}
	}
	return false;
}

/*
local
verschlüsselt den tstring bei false, entschlüsselt bei true
gibt neuen verschlüsselten tstring zurück
*/
TString enDecode(TString toalter, bool decode) {
	int codebits = 6;
	TString toReturn = initializeString("");
	if (decode)
	{
		char decoded;
		for (size_t i = 0; i < toalter.bufferSize; i++)
		{
			decoded = toalter.string[i] - codebits;
			toReturn.string[i] = decoded;
		}
		toReturn.string[toalter.bufferSize-1] = '\0';
		toReturn.bufferSize = toalter.bufferSize;
	}
	else
	{
		char encoded;
		for (size_t i = 0; i < toalter.bufferSize; i++)
		{
			encoded = toalter.string[i] + codebits;
			toReturn.string[i] = encoded;
		}
		toReturn.string[toalter.bufferSize - 1] = '\0';
		toReturn.bufferSize = toalter.bufferSize;
	}
	return toReturn;
}

