#pragma once
#include <iostream>
#include <stdio.h>
#include "StringLib.h"

using namespace std;

struct  TUser
{
	int id;
	char name[41 + 1];
	char password[41 + 1];
};

TUser createNewUser(TString username, TString passwordDecoded);
FILE* openUserFile();
int findUserByName(FILE*, TUser);
bool userAlreadyExisting(FILE*, TUser*);
int getUserIdCounter(FILE*);
void saveUserInFile(FILE*, TUser*);
TUser* findUser(int);
FILE* findUserDataLocation(TUser);
/*
verschlüsselt das übergebene Passwort und vergleicht dann die Übereinstimmung
*/
bool checkPassword(TUser user, TString passwordToCheck);
// local
void saveNewPassword(TUser *user, TString nPassword);
bool alterUserpassword(TUser *user, TString opassword, TString npassword1, TString npassword2);
TString enDecode(TString, bool);