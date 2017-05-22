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
void saveUserInFile(FILE*, char*, char*);