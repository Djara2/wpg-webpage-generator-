#ifndef wpgstring_h
#define wpgstring_h

enum StringError {
	STRING_ERROR_NONE,
	STRING_ERROR_BAD_LENGTH,
	STRING_ERROR_NULL_POINTER,
	STRING_ERROR_FAILED_REALLOC
};

struct String {
	char *data;
	unsigned short length;
	unsigned short capacity;
};

struct String* string_init();

struct String* string_create(char *data, unsigned short length);

enum StringError string_set(struct String *string, char *data, unsigned short length);

enum StringError string_clear(struct String *string);

struct String* string_splice(struct String *string, unsigned short start, unsigned short end, short step);

void string_destroy(struct String *string);
#endif
