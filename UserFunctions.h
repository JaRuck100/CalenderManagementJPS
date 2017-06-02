#pragma once
#include <iostream>
#include <stdio.h>

using namespace std;

struct  TUser
{
	int id;
	char name[40 + 1] = { 0 };
	char password[40 + 1] = { 0 };
};

FILE* openUserFile();
//void saveUserInFile(FILE*, char*, char*);
void saveUserInFile(FILE*, TUser);
TString enDecode(TString, bool);
void saveNewPassword(TUser, TString);
FILE* findUserDataLocation(TUser);
TUser* findUser(int);
int getUserIdCounter(FILE*);
bool userAlreadyExisting(FILE*, TUser);
//int findUserByName(FILE*, TUser);