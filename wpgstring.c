#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "wpgstring.h"

struct String* string_init() {
	struct String *new_string = malloc(sizeof(struct String));
	if (new_string == NULL) {
		fprintf(stderr, "[string_create] Failed to allocate memory for a new String struct on the heap.\n");
		return NULL;
	}

	new_string->length = 0;
	new_string->capacity = 32;
	new_string->data = malloc(sizeof(char) * 32);
	if (new_string->data == NULL) {
		fprintf(stderr, "[string_create] Failed to allocate memory for default 32 bytes of array memory for the String to be created.\n");
		free(new_string);
		return NULL;
	}

	return new_string;
}

struct String* string_create(char *data, unsigned short length) {
	if (data == NULL) {
		fprintf(stderr, "[string_create] Cannot create a new String using a character pointer that points to NULL.\n");
		return NULL;
	}
	
	if (length == 0) {
		fprintf(stderr, "[string_create] Cannot create a new String of length 0.\n");
		return NULL;
	}
	
	// Attempt to allocate String struct itself.
	struct String *new_string = malloc(sizeof(struct String));
	if (new_string == NULL) {
		fprintf(stderr, "[string_create] Failed to allocate memory for a new String on the heap.\n");
		return NULL;
	}

	// Attempt to allocate data buffer.
	new_string->data = malloc(sizeof(char) * (length + 1));
	if (new_string->data == NULL) {
		fprintf(stderr, "[string_create] Failed to allocate %hu bytes of memory for the provided string \"%s\".\n", length, data);
		free(new_string);
		return NULL;
	}

	// Set bounds
	new_string->length = length;
	new_string->capacity = length + 1;

	// Copy string data
	strcpy(new_string->data, data);
	new_string->data[length] = '\0'; 

	return new_string;
}

enum StringError string_set(struct String *string, char *data, unsigned short length) {
	if (string == NULL) {
		fprintf(stderr, "[string_set] Cannot set data for a String pointer that points to NULL.\n");
		return STRING_ERROR_NULL_POINTER;
	}

	if (data == NULL) {
		fprintf(stderr, "[string_set] Cannot set data with a char pointer that points to NULL.\n");
		return STRING_ERROR_NULL_POINTER;
	}

	if (length == 0) {
		fprintf(stderr, "[string_set] Cannot set a String with a length of 0. Try to use string_clear for this functionality.\n");
		return STRING_ERROR_BAD_LENGTH;
	}

	if (length >= string->capacity) {
		string->data = realloc(string->data, sizeof(char) * (length + 1));
		if (string->data == NULL) {
			fprintf(stderr, "[string_set] Failed to reallocate the data buffer of the string to be set.\n");
			free(string);
			return STRING_ERROR_FAILED_REALLOC;
		}
	}
	
	string->length = length;
	string->capacity = length + 1;
	strcpy(string->data, data);
	string->data[length] = '\0';

	return STRING_ERROR_NONE;
}

enum StringError string_clear(struct String *string) {
	if (string == NULL) {
		fprintf(stderr, "[string_clear] Cannot reset the data buffer of a String with a NULL pointer.\n");
		return STRING_ERROR_NULL_POINTER;
	}

	if (string->data == NULL) {
		fprintf(stderr, "[string_clear] Cannot reset the data buffer of a String with a NULL pointer data buffer.\n");
		return STRING_ERROR_NULL_POINTER;
	}

	string->length = 0;
	memset(string->data, 0, string->capacity);
	return STRING_ERROR_NONE;
}

struct String* string_splice(struct String *string, unsigned short start, unsigned short end, short step) {
	if (start == end) {
		fprintf(stderr, "[string_splice] Cannot splice a string using equal start and end boundaries.\n");
		return NULL;
	}

	if (step == 0) {
		fprintf(stderr, "[string_splice] Cannot use a step of 0.\n");
		return NULL;
	}
	
	if (start > end) {
		if (step < 0) {
			unsigned short placeholder = 0;
			placeholder = start;
			start = end;
			end = placeholder;
		}
		else {
			fprintf(stderr, "[string_splice] Cannot have a negative step (%hd) while having a start index (%hu) that is greater than the end index (%hu).\n", step, start, end);
			return NULL;
		}
	}
	
	struct String *new_string = malloc(sizeof(struct String));
	if (new_string == NULL) {
		fprintf(stderr, "[string_splice] Failed to allocate memory for a new String struct on the heap.\n");
		return NULL;
	}
	new_string->length = 0;
	new_string->capacity = ((end - start) / abs(step)) + 1; 
	new_string->data = malloc(sizeof(char) * new_string->capacity); 
	if (new_string->data == NULL) {
		fprintf(stderr, "[string_splice] Failed to allocate %hu bytes of memory for the substring.\n", new_string->capacity);
		free(new_string);
		return NULL;
	}
		
	for (ssize_t i = start; i < end; i += step) {
		if (new_string->length >= new_string->capacity) {
			new_string->capacity *= 2;
			new_string->data = realloc(new_string->data, sizeof(char) * new_string->capacity);
			if (new_string->data == NULL) { 
				fprintf(stderr, "[string_splice] Failed to reallocate additional memory to hold the substring.\n");
				free(new_string);
				return NULL;
			}
		}
		new_string->data[new_string->length] = string->data[i]; 
		new_string->length++;
	}
	new_string->data[new_string->length] = '\0';

	return new_string;
}

void string_destroy(struct String *string) {
	if (string == NULL) {
		fprintf(stderr, "[string_destroy] Cannot free the memory of a String pointer that points to NULL.\n");
		return;
	}

	if (string->data != NULL)
		free(string->data);

	free(string);
}
