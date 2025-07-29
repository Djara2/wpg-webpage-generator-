#ifndef wpgstring_h
#define wpgstring_h

struct String {
	char *data;
	unsigned short length;
	unsigned short capacity;
};

struct String* string_init();

struct String* string_create(char *data, unsigned short length);

bool string_set(struct String *string, char *data, unsigned short length);

enum WPGError string_clear(struct String *string);

void string_destroy(struct String *string);
#endif
