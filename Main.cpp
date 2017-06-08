#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "FieldsLib.h"
#include "ControllerFunctions.h"
#include "UserFunctions.h"

#pragma warning(disable:4996)
using namespace std;


void main() {
	try
	{
		FILE* currentUserFile;
		int _userId = 0;
		if (access("Current_User.txt", 00) == 0) {
			currentUserFile = fopen("Current_User.txt", "r");
			if (currentUserFile != NULL) {
				fread(&_userId, sizeof(int), 1, currentUserFile);
				fclose(currentUserFile);
			}
		}
		else {
			// kein user eingeloggt
			// _userId == 0
		}

		char input[500 + 1] = { 0 };
		//cin.getline(input, 501);
		//testing strings
		//strcpy(input, "url=/register&username=test2&password=bla2&passwordRepeat=bla2");
		//strcpy(input, "url=/register&username=test6&password=bla6&passwordRepeat=bla6");
		//strcpy(input, "url=/login&username=test2&password=bla2");
		//strcpy(input, "url=/login&username=test6&password=bla6");
		//strcpy(input, "url=/login&username=test6&password=6test");
		//strcpy(input, "url=/logout");
		//strcpy(input, "url=/add_event&title=test3&start=2018-03-02 12:12&end=2018-06-06 23:23&description=parthard");
		//strcpy(input, "url=/show_events");
		//strcpy(input, "url=/event_search&id=1");
		//strcpy(input, "url=/change_password&oldPassword=bla1&newPassword=1test&passwordRepeat=1test");
		//strcpy(input, "url=/change_password&oldPassword=bla6&newPassword=6test&passwordRepeat=6test");
		//strcpy(input, "url=/delete&id=2");
		//strcpy(input, "url=/change_event&title=tested&start=2020-02-01 22:22&end=2020-02-01 23:23&description=balabla&id=1");
		//strcpy(input, "url=/check_user&id=1");

		TFieldArray inputFields = parseInputString(input);
		if (strcmp(inputFields.fields[0].name.string, "url") == 0) {
			if (strcmp(inputFields.fields[0].value.string, "/register") == 0) {
				registerFunction(inputFields);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/check_user") == 0) {
				ceckUserFunction(inputFields, _userId);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/show_events") == 0) {
				showEventsFunction(inputFields, _userId);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/delete") == 0) {
				deleteEventFunction(inputFields);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/add_event") == 0) {
				addEventFunction(inputFields, _userId);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/event_search") == 0) {
				searchForEventFunction(inputFields);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/change_event") == 0) {
				changeEventFunction(inputFields);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/change_password") == 0) {
				changePasswordFunction(inputFields, _userId);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/logout") == 0) {
				logoutFunction(inputFields);
			}
			else if (strcmp(inputFields.fields[0].value.string, "/login") == 0) {
				loginFunction(inputFields);
			}	
		}
		else {
			//error Page?
		}
		safeDeleteFieldArray(&inputFields);
	}
	catch (const std::exception&)
	{
		// Fehler bei der Speicherallozierung
	}
}