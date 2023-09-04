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

resp_object_t* parse_resp_string(const char **input){
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_STRING;
	obj->value.string = strndup(*input, end-*input);
	*input = end + 2;
	return obj;
}

resp_object_t* parse_resp_integer(const char **input){
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_INTEGER;
	obj->value.integer = atoi(strndup(*input, end-*input));
	*input = end + 2;
	return obj;
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
	// test string
	const char *message = "+PONG\r\n";
	printf("message = %s", message);
	const char ** current_index = &(message);
	resp_object_t * obj = parse_resp(current_index);
	printf("message string was: %s\n", obj->value.string);
	// test integer
	const char *message2 = ":10\r\n";
	printf("message = %s", message2);
	resp_object_t * obj2 = parse_resp(&message2);
	printf("message string was: %d\n", obj2->value.integer);
}
