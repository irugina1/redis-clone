#include "parse_resp.h"

static resp_object_t* parse_resp_string(const char **input){
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_STRING;
	obj->value.string = strndup(*input, end-*input);
	*input = end + 2;
	return obj;
}

static resp_object_t* parse_resp_integer(const char **input){
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_INTEGER;
	obj->value.integer = atoi(strndup(*input, end-*input));
	*input = end + 2;
	return obj;
}

static resp_object_t* parse_resp_bulk_string(const char **input){
	// setup
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_BULK_STRING;
	// number of chars in bulk string
	char *end_len = strstr(*input, "\r\n");
	int len = atoi(strndup(*input, end_len-*input));
	*input = end_len + 2;
	// create bulk string
	obj->value.string = strndup(*input, len);
	*input = *input + len + 2;
	return obj;
}

static resp_object_t* parse_resp_array(const char **input){
	// setup
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_ARRAY;
	// number of elems in array
	char *end_count = strstr(*input, "\r\n");
	int count = atoi(strndup(*input, end_count-*input));
	obj->value.array.len = count;
	*input = end_count + 2;
	// create array itself
	obj->value.array.elements = malloc(sizeof(resp_object_t*) * count);
	for (long i = 0; i < count; i++) {
		obj->value.array.elements[i] = parse_resp(input);
	}
	return obj;
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
	// test echo
	const char *message3 = "*2\r\n$4\r\nECHO\r\n$3\r\nhey\r\n";
	resp_object_t * obj3 = parse_resp(&message3);
	printf("number of elems in array was : %zu\n", obj3->value.array.len);
	printf("first element in array had string %s\n", obj3->value.array.elements[0]->value.string);
	printf("second element in array had string %s\n", obj3->value.array.elements[1]->value.string);
}
