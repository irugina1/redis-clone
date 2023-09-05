#ifndef RESP_PARSER_H
#define RESP_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	RESP_STRING,
	RESP_ERROR,
	RESP_INTEGER,
	RESP_BULK_STRING,
	RESP_ARRAY,
} resp_type_t;

typedef struct resp_object {
	resp_type_t type;
	union {
		int integer;
		char* string;
		struct {
			size_t len;
			struct resp_object **elements;
		} array;
	} value;
} resp_object_t;

resp_object_t* parse_resp(const char **input);

#endif // RESP_PARSER_H
