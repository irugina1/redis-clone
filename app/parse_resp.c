#include <stdio.h>
#include <stdlib.h>

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

resp_object_t* parse_resp_string(const char **input){
	return NULL;
}

resp_object_t* parse_resp_integer(const char **input){
	return NULL;
}

resp_object_t* parse_resp_bulk_string(const char **input){
	return NULL;
}

resp_object_t* parse_resp_array(const char **input){
	return NULL;
}

resp_object_t* parse_resp(const char **input){
	switch (**input){
		case '+':
			(*input)++; // move to next char
			return parse_resp_string(input);
		case '-':
			(*input)++; // move to next char
			return parse_resp_string(input);
		case ':':
			(*input)++; // move to next char
			return parse_resp_integer(input);
		case '$':
			(*input)++; // move to next char
			return parse_resp_bulk_string(input);
		case '*':
			(*input)++; // move to next char
			return parse_resp_array(input);
		default:
			return NULL;
	}
}

int main(){
	const char *message = "+PONG\r\n";
    printf("message = %s", message);
    const char ** current_index = &(message);
    resp_object_t * obj = parse_resp(current_index);
}
