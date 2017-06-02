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
		addToString(&content, generateEventTableContent(userId));
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

int main() {
	try
	{
		int _userId = 1;
		char input[500 + 1] = { 0 };
		
		// nimmt die url die vom request kommt 
		// die konfig auf dem apache ist so eingestellt, dass, wenn eine datei nicht vorhanden ist
		// auf de exe gelinkt wird
		// apache config auf git vorhanden
		char* request_uri = getenv("REQUEST_URI");
		//char request_uri [30] = { 0 };
		//strcpy(request_uri, "/add_event");


		//entscheidung, was passieren soll und wohin geleitet weerden soll
		if (strcmp(request_uri, "/show_events") == 0) {
			printOutHtmlPage("../htdocs/Events.htm");
			return 0;
		} 
		else if (strcmp(request_uri, "/create_events") == 0) {
			printOutHtmlPage("../htdocs/HTMLPage.htm");
			return 0;
		}
		else if (strcmp(request_uri, "/menue") == 0) {
			printOutHtmlPage("../htdocs/Menue.htm");
			return 0;
		}
		else if (strcmp(request_uri, "/add_event") == 0) {
			cin >> input;
			FILE* eventsFile;
			eventsFile = openEventFile();
			if (eventsFile == NULL)
			{
				errorPage("fehler beim öffnen");
			}
			TFieldArray inputFields = parseInputString(input);
			saveEventInFile(eventsFile, inputFields, _userId);
			fclose(eventsFile);
			printOutHtmlPage("../htdocs/menue.htm");
			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if (strcmp(request_uri, "/delete_event") == 0) {
			cin >> input;
			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = atoi(inputFields.fields[0].value.string);
			if (deleteEvent(eventsFile, eventId) != 0) {
				errorPage("fehler beim löschen");
			}
			printOutHtmlPage("../htdocs/Events.htm");
			fclose(eventsFile);

			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if(strcmp(request_uri, "/login") == 0)
		{
			errorPage(request_uri);
			return 0;
		}
		else if (strcmp(request_uri, "/change_password") == 0)
		{
			errorPage(request_uri);
			return 0;
		}
		else if (strcmp(request_uri, "/change_event") == 0) {
			cin >> input;
			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = atoi(inputFields.fields[0].value.string);
			printOutHtmlPage("../htdocs/ChangeEvent.htm", _userId, eventId);
			fclose(eventsFile);
			safeDeleteFieldArray(&inputFields);
			return 0;
		}
		else if (strcmp(request_uri, "/changed_event") == 0) {
			cin >> input;
			TFieldArray inputFields = parseInputString(input);
			FILE* eventsFile;
			eventsFile = openEventFile();
			int eventId = 0;
			for (size_t i = 0; i < inputFields.length; i++)
			{
				if (strcmp(inputFields.fields[i].name.string, "eventId") == 0) {
					eventId = atoi(inputFields.fields[i].value.string);
				}
			}
			changeEvent(eventsFile, inputFields, eventId);
			fclose(eventsFile);
			printOutHtmlPage("../htdocs/Events.htm");
			safeDeleteFieldArray(&inputFields);
			
			return 0;
		}
		else if(strcmp(request_uri, "/show_events_ajax") == 0 ) {
			TString eventsJson = initializeString("");
			FILE* eventsFile = openEventFile();
			TEvent* event = new TEvent;
			fseek(eventsFile, sizeof(int), SEEK_SET);
			addToString(&eventsJson, "[ ");
			while (!feof(eventsFile))
			{

				fread(event, sizeof(TEvent), 1, eventsFile);
				if (feof(eventsFile)) {
					continue;
				}

				addToString(&eventsJson, " { \"eventName\":\"");
				TString nameString = initializeString("");
				for (int i = 0; i < strlen(event->eventName); ++i) {
					char c = event->eventName[i];
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
				addToString(&eventsJson, nameString);
				safeDeleteString(&nameString);
				addToString(&eventsJson, "\", \"date\":\"");
				addToString(&eventsJson, event->date);
				addToString(&eventsJson, "\", \"time\":\"");
				addToString(&eventsJson, event->time);
				addToString(&eventsJson, "\", \"userId\":");
				char stringUserId[6 + 1] = { 0 };
				itoa(event->userId, stringUserId, 10);
				addToString(&eventsJson, stringUserId);
				addToString(&eventsJson, ", \"id\":");
				char stringEventId[6 + 1] = { 0 };
				itoa(event->id, stringEventId, 10);
				addToString(&eventsJson, stringEventId);

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
			cout << eventsJson.string;
			//zum testen 
			//errorPage(eventsJson.string);
			
		}
		else {
			errorPage(request_uri);
			return 1;
		}
	}
	catch (const std::exception&)
	{
		errorPage("boom!");
	}
}

void main1() {
	FILE* currentUserFile;
	int _userId = 0;
	if (access("Current_User.txt", 00) == 0) {
		currentUserFile = fopen("Current_User.txt", "r");
		if (currentUserFile == NULL) {
			//fehler
		}
		fread(&_userId, sizeof(int), 1, currentUserFile);
	}
	else {
		// fehler
	}
	fclose(currentUserFile);

	char input[500 + 1] = { 0 };
	cin >> input;
	TFieldArray inputFields = parseInputString(input);

	for (size_t i = 0; i < inputFields.length; i++)
	{
		if (strcmp(inputFields.fields[i].name.string, "url") == 0)
		{
			if (strcmp(inputFields.fields[i].name.string, "/register") == 0) {
				FILE* userFile;
				userFile = openUserFile();
				TUser* newUser = new TUser;
				char* repetedPassword;
				for (size_t i = 0; i < inputFields.length; i++)
				{
					if (strcmp(inputFields.fields[i].name.string, "username") == 0) {
						strcpy(newUser->name, inputFields.fields[i].value.string);
					} else if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
						strcpy(newUser->password, inputFields.fields[i].value.string);
					} else if (strcmp(inputFields.fields[i].name.string, "passwordRepeate") == 0) {
						strcpy(repetedPassword, inputFields.fields[i].value.string);
					}
				}
				//suchen nach user by name
				// rückgabe user
				//user gefudnen -> prüfen passwörter gleich
				//schreiben user in File

				delete newUser;
				fclose(userFile);

			} 
			  else if (strcmp(inputFields.fields[i].name.string, "/show_events") == 0) {
				TString eventsJson = initializeString("");
				FILE* eventsFile = openEventFile();
				TEvent* event = new TEvent;
				fseek(eventsFile, sizeof(int), SEEK_SET);
				addToString(&eventsJson, "[ ");
				while (!feof(eventsFile))
				{

					fread(event, sizeof(TEvent), 1, eventsFile);
					if (feof(eventsFile)) {
						continue;
					}
					if (event->userId != _userId) {
						continue;
					}
					addToString(&eventsJson, " { \"eventName\":\"");
					TString nameString = initializeString("");
					for (int i = 0; i < strlen(event->eventName); ++i) {
						char c = event->eventName[i];
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
					addToString(&eventsJson, nameString);
					safeDeleteString(&nameString);
					addToString(&eventsJson, "\", \"date\":\"");
					addToString(&eventsJson, event->date);
					addToString(&eventsJson, "\", \"time\":\"");
					addToString(&eventsJson, event->time);
					addToString(&eventsJson, "\", \"userId\":");
					char stringUserId[6 + 1] = { 0 };
					itoa(event->userId, stringUserId, 10);
					addToString(&eventsJson, stringUserId);
					addToString(&eventsJson, ", \"id\":");
					char stringEventId[6 + 1] = { 0 };
					itoa(event->id, stringEventId, 10);
					addToString(&eventsJson, stringEventId);

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
			}

			else if(strcmp(inputFields.fields[i].name.string, "/delete") == 0) {
				int eventId = 0;
				for (size_t i = 0; i < inputFields.length; i++)
				{
					if (strcmp(inputFields.fields[i].name.string, "id") == 0) {
						eventId = atoi(inputFields.fields[i].name.string);
						FILE* eventFile = openEventFile();
						
						if (deleteEvent(eventFile, eventId) == 0){
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
			}
			else if (strcmp(inputFields.fields[i].name.string, "/event_search") == 0) {
				for (size_t i = 0; i < inputFields.length; i++)
				{
					int eventId = 0;
					if (strcmp(inputFields.fields[i].name.string, "id") == 0) {
						eventId = atoi(inputFields.fields[i].name.string);
						// search event by id 
						break;
					}

				}
				cout << "Content-type:text/html\r\n\r\n" << endl;
				//cout jason

			}
			else if (strcmp(inputFields.fields[i].name.string, "/change_event") == 0) {
				FILE* eventsFile; 
				eventsFile = openEventFile();
				for (size_t i = 0; i < inputFields.length; i++)
				{
					changeEvent(eventsFile, inputFields);

				}

			}
			else if (strcmp(inputFields.fields[i].name.string, "/change_password") == 0) {

			}
			else if (strcmp(inputFields.fields[i].name.string, "/logout") == 0) {

			}
			else if (strcmp(inputFields.fields[i].name.string, "/login") == 0){
				currentUserFile = fopen("Current_User.txt", "w");
				FILE* userFile;
				userFile = openUserFile();
				TUser* userLogin = new TUser;
				for (size_t i = 0; i < inputFields.length; i++)
				{
					if (strcmp(inputFields.fields[i].name.string, "username") == 0) {
						strcpy(userLogin->name, inputFields.fields[i].value.string);
						continue;
					}
					if (strcmp(inputFields.fields[i].name.string, "password") == 0) {
						strcpy(userLogin->password, inputFields.fields[i].value.string);
						continue;
					}
				
				}
				//user find
				//schreiben id in currentUserFile
				//user fefunden retrun "y" else retrun "n"


				delete userLogin;
				fclose(userFile);


			}
			else {

			}
		}
	}

	fclose(currentUserFile);
}