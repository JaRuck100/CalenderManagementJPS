#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "UserFunctions.h"
#include "EventFunctions.h"

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
		//fclose(userFile);
		//userFile = fopen("User.txt", "r+");
	}
	return userFile;
}
/*
Speichert die Daten eines Users in userFile
Name, Passwort
*/
//void saveUserInFile(FILE* userFile, char* userName, char* userPasword) {
//	TUser* newUser = new TUser;
//	int userIdCounter = getIdCounter(userFile);
//
//	strcpy(newUser->name, userName);
//	 Verschlüsselung benötigt
//	strcpy(newUser->password, userPasword);
//	newUser->id = userIdCounter;
//
//	fseek(userFile, 0, SEEK_END);
//	fwrite(newUser, sizeof(TEvent), 1, userFile);
//	fseek(userFile, 0, SEEK_SET);
//	fwrite(&userIdCounter, sizeof(int), 1, userFile);
//	delete newUser;
//}


int findUserByName2(FILE* userFile, TUser user) {
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
	return - 1;
}


//int findUserIdByName(FILE* userFile, TUser loginUser) {
//	TUser* userInFile = new TUser;
//	while (!feof(userFile)) {
//		fread(userInFile, sizeof(TUser), 1, userFile);
//		if (feof(userFile)){
//			continue;
//		}
//		if (strcmp(userInFile->name, loginUser.name) == 0) {
//			if (strcmp(userInFile->password, loginUser.password) == 0) {
//				return userInFile->id;
//			}
//		}
//	}
//	return -1;
//	delete userInFile;
//}

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
		int unserNameLength = strlen(searchUser->name);
		if (strcmp(user->name, searchUser->name) == 0) {
			userFound = true;
		}
	}
	delete searchUser;
	return userFound;
}


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
	//TUser* newUser = new TUser;
	int userIdCounter = getUserIdCounter(userlocationInFile);
	if (userToSave->id == NULL || userToSave->id >userIdCounter || userToSave->id < 0)
	{
		userIdCounter++;
		userToSave->id = userIdCounter;
	}
	//strcpy(newUser->name, userToSave.name);
	//strcpy(newUser->password, userToSave.password);

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
Findet User nach seiner ID im UserFile und gibt dessen Stelle zurück oder NULL.
Es muss auf NULL geprüft werden!
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
			fseek(f, -(int)sizeof(TUser), SEEK_CUR);
			return f;
		}
	}
	fclose(f);
	return NULL;
}

void findUserDataLocation(TUser userToFind, FILE* userFile) {

	TUser* tempUser = new TUser;
	fseek(userFile, sizeof(int), SEEK_SET);
	while (!feof(userFile))
	{
		fread(tempUser, sizeof(TUser), 1, userFile);
		if (feof(userFile))
		{
			continue;
		}
		if (tempUser->id == userToFind.id)
		{
			delete tempUser;
			long sizeOfTEvent = sizeof(TUser);
			fseek(userFile, -sizeOfTEvent, SEEK_CUR);
			break;
		}
	}
}

/*
local
Speichert user mit neuem Password, egal ob neuer User oder vorhandener
*/
void saveNewPassword(TUser user, TString nPassword) {
	FILE* userInFile = findUserDataLocation(user);
	nPassword = enDecode(nPassword, false);
	strcpy(user.password, nPassword.string);
	saveUserInFile(userInFile, &user);
}

/*
Braucht aktuellen User, altes Password zur bestätigung und zwei mal das neue Password zur Kontrolle
Gibt false zurück, wenn das alte Password falsch ist oder die neuen nicht übereinstimmen
*/
//bool alterUserPassword(TUser user, TString oPassword, TString nPassword1, TString nPassword2) {
//	if (compareString(enDecode(initializeString(user.password), true), oPassword))
//	{
//		if (compareString(nPassword1, nPassword2))
//		{
//			saveNewPassword(user, nPassword1);
//			return true;
//		}
//	}
//	return false;
//}

/*
local
Verschlüsselt den TString bei false, entschlüsselt bei true
Gibt neuen verschlüsselten TString zurück
*/
TString enDecode(TString toAlter, bool decode) {
	int codeBits = 6;
	TString toReturn = initializeString("");
	if (decode)
	{
		char decoded;
		for (size_t i = 0; i < toAlter.bufferSize - 1; i++)
		{
			decoded = toAlter.string[i] - codeBits;
			toReturn.string[i] = decoded;
		}
	}
	else
	{
		char encoded;
		for (size_t i = 0; i < toAlter.bufferSize; i++)
		{
			encoded = toAlter.string[i] + codeBits;
			toReturn.string[i] = encoded;
		}
	}
	return toReturn;
}
