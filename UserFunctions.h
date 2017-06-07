#pragma once
#include <iostream>
#include <stdio.h>
#include "StringLib.h"

using namespace std;

struct  TUser
{
	int id;
	char name[41 + 1] = { 0 };
	char password[41 + 1] = { 0 };
};

FILE* openUserFile();
void saveUserInFile(FILE*, TUser*);
//TString enDecode(TString, bool);
void saveNewPassword(TUser, TString);
FILE* findUserDataLocation(TUser);
TUser* findUser(int);
int getUserIdCounter(FILE*);
bool userAlreadyExisting(FILE*, TUser*);
int findUserByName(FILE*, TUser);