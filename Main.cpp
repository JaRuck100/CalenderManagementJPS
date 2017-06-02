#include <iostream>
#include <stdio.h>
#include <io.h>
#include "GeneralFuctions.h"
#include "GenerateContentFunctions.h"
#include "FieldsLib.h"
#include "EventFunctions.h"
#include "UserFunctions.h"

#pragma warning(disable:4996)
using namespace std;

/*
Prüft welcher placeholder übergeben wurde.
Ruft Funktion auf, die Inhalt für den Platzhalter zusammenbaut.
Gibt den Inhalt, der anstelle des Platzhalters kommt, zurück.
*/
TString replacePlaceholder(TString placeholder, int userId, int eventId = 0) {
	TString content = initializeString("");

	if (strcmp(placeholder.string, "[@TABLE]") == 0) {
		//addToString(&content, generateEventTableContent(userId));
	}
	else if (strcmp(placeholder.string, "[@USER]") == 0) {
		content = initializeString("Uwe");
	}
	else if (strcmp(placeholder.string, "[@EVENTID]") == 0) {
		char stringEventId[6 + 1] = { 0 };
		itoa(eventId, stringEventId, 10);
		content = initializeString(stringEventId);
	}


	return content;
} 


/*
Baut die neue HTML-Seite zusammen.
Gibt String zurück, in dem alle Platzhalter ersetzt wurden.
*/
char* generateHtmlPage(FILE* htmlFile, int userId, int eventId = 0) {
	// Setzt den Content-Type an den Anfang der HTML-Seite 
	TString htmlPage = initializeString("Content-type:text/html\r\n\r\n");
	TString htmlFromFile = getHtmlFromFile(htmlFile);
	// Fügt den restlichen Inhalt aus dem htmlFromFile hinzu.
	addToString(&htmlPage, htmlFromFile);

	// bei Error wird 0 übergeben
	if (userId != 0) {
		// Fügt die Platzhalter in ein Array ein
		TString htmlPageWithoutPlaceholder = initializeString("");
		TStringArray placeholders = initializeStringArray("[@TABLE]");
		addToStringArray(&placeholders, "[@USER]");
		addToStringArray(&placeholders, "[@EVENTID]");
		TString content = initializeString("");

		fclose(htmlFile);

		// Geht das Array der Platzhalter durch und 
		// lässt die Platzhalter mit dem dem gewünschten Inhalt ersetzten
		for (size_t i = 0; i < placeholders.length; i++)
		{
			copyString(&content, replacePlaceholder(placeholders.strings[i], userId, eventId));
			copyString(&htmlPageWithoutPlaceholder, findAndReplace(htmlPage, placeholders.strings[i], content));
			copyString(&htmlPage, htmlPageWithoutPlaceholder);
			safeDeleteString(&htmlPageWithoutPlaceholder);
		}
		safeDeleteString(&htmlFromFile);
		deleteStringArray(&placeholders);
		safeDeleteString(&content);
	}
	return htmlPage.string;
}

bool isHex(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/*
Zerlegt einen Inputstring in die Feldnamen und deren dazugehörigen Werte
Gibt ein TFieldArray mit den einzelnen name-value-Paaren zurück
*/
TFieldArray parseInputString(char* inputString) {
	TField field = initializeField("", "");
	TFieldArray fieldArray;
	TString fieldName = initializeString("");
	TString fieldValue = initializeString("");
	bool isFirst = true;
	unsigned char fromHexTable[256] = { 0 };
	fromHexTable['0'] = 0;
	fromHexTable['1'] = 1;
	fromHexTable['2'] = 2;
	fromHexTable['3'] = 3;
	fromHexTable['4'] = 4;
	fromHexTable['5'] = 5;
	fromHexTable['6'] = 6;
	fromHexTable['7'] = 7;
	fromHexTable['8'] = 8;
	fromHexTable['9'] = 9;
	fromHexTable['A'] = 10;
	fromHexTable['B'] = 11;
	fromHexTable['C'] = 12;
	fromHexTable['D'] = 13;
	fromHexTable['E'] = 14;
	fromHexTable['F'] = 15;
	fromHexTable['a'] = 10;
	fromHexTable['b'] = 11;
	fromHexTable['c'] = 12;
	fromHexTable['d'] = 13;
	fromHexTable['e'] = 14;
	fromHexTable['f'] = 15;


	for (size_t i = 0; inputString[i] != '\0'; i++)
	{
		if (inputString[i] == '=')
		{
			//field name fertig
			i++;
			for (i; inputString[i] != '&' && inputString[i] != '\0'; i++)
			{
				if (inputString[i] == '%' && isHex(inputString[i + 1]) && isHex(inputString[i + 2])) {
					int asciiValue = 0;
					char value = inputString[i + 1];
					asciiValue += fromHexTable[inputString[i + 1]] *16;
					asciiValue += fromHexTable[inputString[i + 2]];
					unsigned char character = asciiValue;
					addToString(&fieldValue, character);
					i += 2;
				}
				else if (inputString[i] == '+') {
					addToString(&fieldValue, ' ');
				}
				else {
					addToString(&fieldValue, inputString[i]);

				}
			}

			safeDeleteField(&field);
			field = initializeField(fieldName, fieldValue);
			if (isFirst) {
				fieldArray = initializeFieldArray(field);
				isFirst = false;
				safeDeleteString(&fieldName);
				safeDeleteString(&fieldValue);
				fieldName = initializeString("");
				fieldValue = initializeString("");
			}
			else {
				addToFieldArray(&fieldArray, field);
				safeDeleteString(&fieldName);
				safeDeleteString(&fieldValue);
				fieldName = initializeString("");
				fieldValue = initializeString("");
				if (inputString[i] == '\0') {
					i--;
				}
			}
		}
		else {
			addToString(&fieldName, inputString[i]);
		}
	}

	safeDeleteField(&field);
	safeDeleteString(&fieldName);
	safeDeleteString(&fieldValue);
	return fieldArray;

}

/*
gibt die fertig gebaute html seite aus
nimmt die user und event id
wurde beim ändern des events benötigt
*/
void printOutHtmlPage(char* htmlPath, int userId, int eventId) {
	FILE* htmlFile;
	htmlFile = fopen(htmlPath, "r+");
	if (htmlFile == NULL) {
		cout << "Content-type:text/html\r\n\r\n" << endl;
		cout << "fail";
	}
	TString htmlPage = initializeString(generateHtmlPage(htmlFile, userId, eventId));
	cout << htmlPage.string;
	safeDeleteString(&htmlPage);
	fclose(htmlFile);
}

/*
gibt die fertig gebaute html seite aus
nimmt die user id
*/
void printOutHtmlPage(char* htmlPath) {
	FILE* htmlFile;
	htmlFile = fopen(htmlPath, "r+");
	if (htmlFile == NULL) {
		cout << "Content-type:text/html\r\n\r\n" << endl;
		cout << "fail";
	}
	TString htmlPage = initializeString(generateHtmlPage(htmlFile, 1));
	cout << htmlPage.string;
	safeDeleteString(&htmlPage);
	fclose(htmlFile);
}

//int main() {
//	try
//	{
//		int _userId = 1;
//		char input[500 + 1] = { 0 };
//		
//		// nimmt die url die vom request kommt 
//		// die konfig auf dem apache ist so eingestellt, dass, wenn eine datei nicht vorhanden ist
//		// auf de exe gelinkt wird
//		// apache config auf git vorhanden
//		char* request_uri = getenv("REQUEST_URI");
//		//char request_uri [30] = { 0 };
//		//strcpy(request_uri, "/add_event");
//
//
//		//entscheidung, was passieren soll und wohin geleitet weerden soll
//		if (strcmp(request_uri, "/show_events") == 0) {
//			printOutHtmlPage("../htdocs/Events.htm");
//			return 0;
//		} 
//		else if (strcmp(request_uri, "/create_events") == 0) {
//			printOutHtmlPage("../htdocs/HTMLPage.htm");
//			return 0;
//		}
//		else if (strcmp(request_uri, "/menue") == 0) {
//			printOutHtmlPage("../htdocs/Menue.htm");
//			return 0;
//		}
//		else if (strcmp(request_uri, "/add_event") == 0) {
//			cin >> input;
//			FILE* eventsFile;
//			eventsFile = openEventFile();
//			if (eventsFile == NULL)
//			{
//				errorPage("fehler beim öffnen");
//			}
//			TFieldArray inputFields = parseInputString(input);
//			saveEventInFile(eventsFile, inputFields, _userId);
//			fclose(eventsFile);
//			printOutHtmlPage("../htdocs/menue.htm");
//			safeDeleteFieldArray(&inputFields);
//			return 0;
//		}
//		else if (strcmp(request_uri, "/delete_event") == 0) {
//			cin >> input;
//			TFieldArray inputFields = parseInputString(input);
//			FILE* eventsFile;
//			eventsFile = openEventFile();
//			int eventId = atoi(inputFields.fields[0].value.string);
//			if (deleteEvent(eventsFile, eventId) != 0) {
//				errorPage("fehler beim löschen");
//			}
//			printOutHtmlPage("../htdocs/Events.htm");
//			fclose(eventsFile);
//
//			safeDeleteFieldArray(&inputFields);
//			return 0;
//		}
//		else if(strcmp(request_uri, "/login") == 0)
//		{
//			errorPage(request_uri);
//			return 0;
//		}
//		else if (strcmp(request_uri, "/change_password") == 0)
//		{
//			errorPage(request_uri);
//			return 0;
//		}
//		else if (strcmp(request_uri, "/change_event") == 0) {
//			cin >> input;
//			TFieldArray inputFields = parseInputString(input);
//			FILE* eventsFile;
//			eventsFile = openEventFile();
//			int eventId = atoi(inputFields.fields[0].value.string);
//			printOutHtmlPage("../htdocs/ChangeEvent.htm", _userId, eventId);
//			fclose(eventsFile);
//			safeDeleteFieldArray(&inputFields);
//			return 0;
//		}
//		else if (strcmp(request_uri, "/changed_event") == 0) {
//			cin >> input;
//			TFieldArray inputFields = parseInputString(input);
//			FILE* eventsFile;
//			eventsFile = openEventFile();
//			int eventId = 0;
//			for (size_t i = 0; i < inputFields.length; i++)
//			{
//				if (strcmp(inputFields.fields[i].name.string, "eventId") == 0) {
//					eventId = atoi(inputFields.fields[i].value.string);
//				}
//			}
//			changeEvent(eventsFile, inputFields);
//			fclose(eventsFile);
//			printOutHtmlPage("../htdocs/Events.htm");
//			safeDeleteFieldArray(&inputFields);
//			
//			return 0;
//		}
//		else if(strcmp(request_uri, "/show_events_ajax") == 0 ) {
//			TString eventsJson = initializeString("");
//			FILE* eventsFile = openEventFile();
//			TEvent* event = new TEvent;
//			fseek(eventsFile, sizeof(int), SEEK_SET);
//			addToString(&eventsJson, "[ ");
//			while (!feof(eventsFile))
//			{
//
//				fread(event, sizeof(TEvent), 1, eventsFile);
//				if (feof(eventsFile)) {
//					continue;
//				}
//
//				addToString(&eventsJson, " { \"eventName\":\"");
//				TString nameString = initializeString("");
//				for (int i = 0; i < strlen(event->title); ++i) {
//					char c = event->title[i];
//					switch (c)
//					{
//					case '\\':
//					{
//						addToString(&nameString, "\\\\");
//
//						break;
//					}
//					case '"':
//					{
//						addToString(&nameString, "\\");
//						addToString(&nameString, '"');
//
//						break;
//					}
//					case '\n':
//					{
//						addToString(&nameString, "\\");
//						addToString(&nameString, 'n');
//
//						break;
//					}
//					default:
//					{
//						addToString(&nameString, c);
//						
//						break;
//					}
//					}
//				}
//				addToString(&eventsJson, nameString);
//				safeDeleteString(&nameString);
//				addToString(&eventsJson, "\", \"date\":\"");
//				addToString(&eventsJson, event->start);
//				addToString(&eventsJson, "\", \"time\":\"");
//				addToString(&eventsJson, event->end);
//				addToString(&eventsJson, "\", \"userId\":");
//				char stringUserId[6 + 1] = { 0 };
//				itoa(event->userId, stringUserId, 10);
//				addToString(&eventsJson, stringUserId);
//				addToString(&eventsJson, ", \"id\":");
//				char stringEventId[6 + 1] = { 0 };
//				itoa(event->id, stringEventId, 10);
//				addToString(&eventsJson, stringEventId);
//
//				fread(event, sizeof(TEvent), 1, eventsFile);
//				if (feof(eventsFile)) {
//					addToString(&eventsJson, " }");
//				}
//				else {
//					addToString(&eventsJson, " },");
//					fseek(eventsFile, -sizeof(TEvent), SEEK_CUR);
//				}
//				
//			}
//			
//			addToString(&eventsJson, " ]");
//			cout << eventsJson.string;
//			//zum testen 
//			//errorPage(eventsJson.string);
//			
//		}
//		else {
//			errorPage(request_uri);
//			return 1;
//		}
//	}
//	catch (const std::exception&)
//	{
//		errorPage("boom!");
//	}
//}
void buildJson(TString* jsonString, TEvent* event) {
	addToString(jsonString, " { \"title\":\"");
	TString nameString = initializeString("");
	for (int i = 0; i < strlen(event->title); ++i) {
		char c = event->title[i];
		switch (c)
		{
		case '\\':
		{
			addToString(&nameString, "\\\\");
			break;
		}
		case '"':
		{
			addToString(&nameString, "\\");
			addToString(&nameString, '"');

			break;
		}
		case '\n':
		{
			addToString(&nameString, "\\");
			addToString(&nameString, 'n');

			break;
		}
		default:
		{
			addToString(&nameString, c);

			break;
		}
		}

	}

	addToString(jsonString, nameString);
	safeDeleteString(&nameString);
	addToString(jsonString, "\", \"start\":\"");
	addToString(jsonString, event->start);
	addToString(jsonString, "\", \"end\":\"");
	addToString(jsonString, event->end);
	addToString(jsonString, "\", \"userId\":"); // gucken ob gleich
	char stringUserId[6 + 1] = { 0 };
	itoa(event->userId, stringUserId, 10);
	addToString(jsonString, stringUserId);
	addToString(jsonString, ", \"id\":");
	char stringEventId[6 + 1] = { 0 };
	itoa(event->id, stringEventId, 10);
	addToString(jsonString, stringEventId);
}

void main() {
	FILE* currentUserFile;
	int _userId = 0;
	if (access("Current_User.txt", 00) == 0) {
		currentUserFile = fopen("Current_User.txt", "r");
		if (currentUserFile == NULL) {
						//fehler
				
		}
		fread(&_userId, sizeof(int), 1, currentUserFile);
		fclose(currentUserFile);
		
	}
	else {
			
	}
	
	
	char input[500 + 1] = { 0 };

	//cin >> input;
	strcpy(input, "url=/register&username=test1&password=bla1&passwordRepeat=bla1");
	TFieldArray inputFields = parseInputString(input);
	
	for (size_t i = 0; i < inputFields.length; i++) {
		if (strcmp(inputFields.fields[i].name.string, "url") == 0) {
			for (size_t j = 0; j < inputFields.length; j++){
				if (strcmp(inputFields.fields[i].value.string, "/register") == 0) {
					FILE* userFile;
					userFile = openUserFile();
					TUser* newUser = new TUser;
					char repetedPassword[40] = {0};
					for (size_t i = 0; i < inputFields.length; i++) {

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

					bool isNewUser;
					if (userAlreadyExisting(userFile, *newUser) == true) {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						//username schon vorhanden
						isNewUser = false;
						cout << "un";
					}
					else if (strcmp(newUser->password, repetedPassword) != 0)
					{
						cout << "Content-type:text/html\r\n\r\n" << endl;
						//passwort nciht richtig
						cout << "pn";
						isNewUser = false;
					}
					else {
						isNewUser = true;
					}

					if (isNewUser == true) {
						fseek(userFile, 0, SEEK_END);
						// verschlüsseldung nicht vorhanden
						saveUserInFile(userFile, *newUser);
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "y";
					}	

					delete newUser;
					fclose(userFile);
					j = inputFields.length;
				}

				else if (strcmp(inputFields.fields[i].value.string, "/show_events") == 0) {
					TString eventsJson = initializeString("");
					FILE* eventsFile = openEventFile();
					TEvent* event = new TEvent;
					fseek(eventsFile, sizeof(int), SEEK_SET);
					addToString(&eventsJson, "[ ");
					while (!feof(eventsFile)) {
					
						fread(event, sizeof(TEvent), 1, eventsFile);
						if (feof(eventsFile)) {
							continue;
						
						}
						if (event->userId != _userId) {
							continue;
						
						}
						buildJson(&eventsJson, event);
					
						fread(event, sizeof(TEvent), 1, eventsFile);
						if (feof(eventsFile)) {
							addToString(&eventsJson, " }");
						
						}
						else {
							addToString(&eventsJson, " },");
							fseek(eventsFile, -sizeof(TEvent), SEEK_CUR);
						
						}
					
					}
				
					addToString(&eventsJson, " ]");
					cout << "Content-type:text/html\r\n\r\n" << endl;
					cout << eventsJson.string;
					fclose(eventsFile);
					//löschen des events
					fclose(currentUserFile);

				} else if (strcmp(inputFields.fields[i].value.string, "/delete") == 0) {
					int eventId = 0;
					for (size_t i = 0; i < inputFields.length; i++) {
						if (strcmp(inputFields.fields[i].name.string, "id") == 0) {
							eventId = atoi(inputFields.fields[i].name.string);
							FILE* eventFile = openEventFile();
						
							if (deleteEvent(eventFile, eventId) == 0) {
								fclose(eventFile);
								cout << "Content-type:text/html\r\n\r\n" << endl;
								cout << "y";
								break;
							
							}
							else 
							{
								fclose(eventFile);
								cout << "Content-type:text/html\r\n\r\n" << endl;
								cout << "n";
								break;
							}
						}
					}
					fclose(currentUserFile);
				}

				else if (strcmp(inputFields.fields[i].value.string, "/add_event") == 0) {
					FILE* eventsFile;
					eventsFile = openEventFile();
					if (eventsFile == NULL) {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "n";
					}
					else {
						TEvent*  newEvent = new TEvent;
						saveEventInFile(eventsFile, inputFields, _userId);
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "y";
						delete newEvent;
					}	
					fclose(eventsFile);
					fclose(currentUserFile);
				}

				else if (strcmp(inputFields.fields[i].value.string, "/event_search") == 0) {
					TString eventJson = initializeString("");
					for (size_t i = 0; i < inputFields.length; i++) {
						int eventId = 0;
						if (strcmp(inputFields.fields[i].name.string, "id") == 0) {
							eventId = atoi(inputFields.fields[i].name.string);
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
									addToString(&eventJson, "}");
								}
							}
							break;
							delete searchedEvent;
							fclose(eventsFile);
						}
					}


					cout << "Content-type:text/html\r\n\r\n" << endl;
					cout << eventJson.string;

					safeDeleteString(&eventJson);	
					fclose(currentUserFile);
					
				}
				else if (strcmp(inputFields.fields[i].value.string, "/change_event") == 0) {
					FILE* eventsFile;
					eventsFile = openEventFile();
					for (size_t i = 0; i < inputFields.length; i++) {
						if (changeEvent(eventsFile, inputFields) == 0) {
							cout << "Content-type:text/html\r\n\r\n" << endl;
							cout << "y";
						}
						else
						{
							cout << "Content-type:text/html\r\n\r\n" << endl;
							cout << "n";
						}
					}
					fclose(eventsFile);
					fclose(currentUserFile);
				}
				else if (strcmp(inputFields.fields[i].value.string, "/change_password") == 0) {
					
					TUser* user = new TUser;
					user = findUser(_userId);
					char* passwordRepeat;
					char* oldPassword;
					char* newPassword;
					for (size_t k = 0; k < inputFields.length; k++) {
						if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
							strcmp(oldPassword, inputFields.fields[i].value.string);
						} 
						if (strcmp(inputFields.fields[i].name.string, "newPassword") == 0) {
							strcmp(newPassword, inputFields.fields[i].value.string);
						}
						if (strcmp(inputFields.fields[i].name.string, "passwordRepeat") == 0) {
							strcmp(passwordRepeat, inputFields.fields[i].value.string);
						}
					}
					bool rightOldPassword = (strcmp(user->password, oldPassword) == 0);
					bool rightNewPassword = (strcmp(newPassword, passwordRepeat) == 0);
					

					if (rightOldPassword &&  rightNewPassword) {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "y";
						FILE* userInFileLocation;
						userInFileLocation = findUserDataLocation(*user);
						saveUserInFile(userInFileLocation, *user);
					}
					else {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "n";
					}
			
					delete user;
					fclose(currentUserFile);
				}
				else if (strcmp(inputFields.fields[i].name.string, "/logout") == 0) {
					fclose(currentUserFile);
					remove("Current_User.txt");


				}
				else if (strcmp(inputFields.fields[i].name.string, "/login") == 0) {
					currentUserFile = fopen("Current_User.txt", "w");
					FILE* userFile;
					userFile = openUserFile();
					TUser* userLogin = new TUser;
					for (size_t k = 0; k < inputFields.length; k++) {
						if (strcmp(inputFields.fields[k].name.string, "username") == 0) {
							strcpy(userLogin->name, inputFields.fields[i].value.string);
							continue;

						}
						if (strcmp(inputFields.fields[k].name.string, "password") == 0) {
							strcpy(userLogin->password, inputFields.fields[k].value.string);
							continue;

						}

					}
					fseek(userFile, sizeof(int), SEEK_SET);
					
					//_userId = findUserByName(userFile, *userLogin);
					if (userLogin->id == -1) {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "n";
					}
					else {
						cout << "Content-type:text/html\r\n\r\n" << endl;
						cout << "y";
						fwrite(&_userId, sizeof(int), 1, currentUserFile);
					}

					delete userLogin;
					fclose(userFile);
					fclose(currentUserFile);
				}
			}
			
			i = inputFields.length;
		}
		
	}
	

}