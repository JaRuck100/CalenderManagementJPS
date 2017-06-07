#include <iostream>
#include "StringLib.h"
#include "FieldsLib.h"

/*
Erstellt aus zwei char* ein Field
name = name des Inputfeldes, value= Wert des Inputfeldes
*/
TField initializeField(char* name, char* value) {
	TField newField;
	newField.name = initializeString(name);
	newField.value = initializeString(value);
	return newField;
}

/*
Erstellt aus zwei TString ein Field
name = name des Inputfeldes, value= Wert des Inputfeldes
*/
TField initializeField(TString name, TString value) {
	TField newField = initializeField(name.string, value.string);
	return newField;
}

/*
Löscht ein TField
*/
void safeDeleteField(TField* field) {
	safeDeleteString(&field->name);
	safeDeleteString(&field->value);
}

/*
Copiert ein TField in einen anderen
*/
void copyField(TField* destination, TField source) {
	safeDeleteField(destination);
	*destination = initializeField(source.name.string, source.value.string);
 }

/*
Erzeugt ein TFieldArray aus einem Field;
*/
TFieldArray initializeFieldArray(TField field) {
	TFieldArray newFieldArray;
	newFieldArray.length = 1;
	newFieldArray.fields = new TField[newFieldArray.length];
	copyField(&newFieldArray.fields[0], field);
	return newFieldArray;
}

/*
Löscht die Inhalte und das TFieldArray selbst
*/
void safeDeleteFieldArray(TFieldArray* fieldArray) {
	if (fieldArray->length > 0 && fieldArray->fields != NULL) {
		for (size_t i = 0; i < fieldArray->length; i++)
		{
			safeDeleteField(&fieldArray->fields[i]);
		}
		fieldArray->length = 0;
		delete[] fieldArray->fields;
		fieldArray->fields = NULL;
	}
}

/*
Fügt ein Field dem Array hinzu
*/
void addToFieldArray(TFieldArray* fieldArray, TField field) {
	TField* temp = fieldArray->fields;
	++fieldArray->length;
	fieldArray->fields = new TField[fieldArray->length];
	for (size_t i = 0; i < fieldArray->length - 1; i++)
	{
		fieldArray->fields[i] = temp[i];
	}
	delete [] temp;
	fieldArray->fields[fieldArray->length - 1] = initializeField(field.name.string, field.value.string);
}
