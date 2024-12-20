#include "parse_resp.h"

void print_raw(const char *s) {
	while (*s) {
		switch (*s) {
			case '\n': printf("\\n"); break;
			case '\t': printf("\\t"); break;
			case '\r': printf("\\r"); break;
			case '\b': printf("\\b"); break;
			case '\f': printf("\\f"); break;
			case '\a': printf("\\a"); break;
			case '\\': printf("\\\\"); break;
			case '\"': printf("\\\""); break;
			default:
				if (isprint((unsigned char) *s)) {
					putchar(*s);
				} else {
					printf("\\x%02x", (unsigned char) *s);
				}
		}
		s++;
	}
}


static resp_object_t* parse_resp_string(const char **input){
	/*printf("\nparsing resp string\n");
	print_raw(*input);
	printf("\n");*/
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_STRING;
	obj->value.string = strndup(*input, end-*input);
	*input = end + 2;
	//printf("done parsing string %s\n", obj->value.string);
	return obj;
}

static resp_object_t* parse_resp_integer(const char **input){
	/*printf("\nparsing resp integer\n");
	print_raw(*input);
	printf("\n");*/
	char *end = strstr(*input, "\r\n");
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_INTEGER;
	char *int_val_as_string = strndup(*input, end-*input);
	obj->value.integer = atoi(int_val_as_string);
	free(int_val_as_string);
	int_val_as_string = NULL;
	*input = end + 2;
	//printf("done parsing integer %d\n", obj->value.integer);
	return obj;
}

static resp_object_t* parse_resp_bulk_string(const char **input){
	/*printf("\nparsing resp bulk string\n");
	print_raw(*input);
	printf("\n");*/
	// setup
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_BULK_STRING;
	// number of chars in bulk string
	char *end_len = strstr(*input, "\r\n");
	char *len_as_string = strndup(*input, end_len-*input);
	int len = atoi(len_as_string);
	free(len_as_string);
	len_as_string = NULL;
	*input = end_len + 2;
	// create bulk string
	obj->value.string = strndup(*input, len);
	*input = *input + len + 2;
	//printf("done parsing bulk string %s\n", obj->value.string);
	return obj;
}


static resp_object_t* parse_resp_array(const char **input){
	/*printf("\nparsing resp array\n");
	print_raw(*input);
	printf("\n");*/
	// setup
	resp_object_t *obj = malloc(sizeof(resp_object_t));
	obj->type = RESP_ARRAY;
	// number of elems in array
	char *end_count = strstr(*input, "\r\n");
	char *count_as_string = strndup(*input, end_count-*input);
	int count = atoi(count_as_string);
	free(count_as_string);
	count_as_string = NULL;
	obj->value.array.len = count;
	*input = end_count + 2;
	// create array itself
	obj->value.array.elements = malloc(sizeof(resp_object_t*) * count);
	for (long i = 0; i < count; i++) {
		resp_object_t* elem = parse_resp(input);
		obj->value.array.elements[i] = elem;
	}
	//printf("done parsing array\n");
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

void free_resp_object(resp_object_t *object) {
    if (object == NULL) {
        return; // Handle the case of a NULL pointer
    }

    switch (object->type) {
        case RESP_STRING:
            // Free the dynamically allocated string
            free(object->value.string);
	    object->value.string = NULL;
            break;
        case RESP_INTEGER:
            // No dynamically allocated memory to free for integers
            break;
        case RESP_BULK_STRING:
            // Free the dynamically allocated string
            free(object->value.string);
	    object->value.string = NULL;
            break;
        case RESP_ARRAY:
            // Free the elements in the array recursively
            for (size_t i = 0; i < object->value.array.len; i++) {
                free_resp_object(object->value.array.elements[i]);
		object->value.array.elements[i] = NULL;
            }
            // Free the array itself
            free(object->value.array.elements);
	    object->value.array.elements = NULL;
            break;
        default:
            // Handle any other types or errors here
            break;
    }

    // Finally, free the object itself
    free(object);
    object = NULL;
}

/*
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
	// test empty string
	const char *message4 = "+\r\n";
	printf("message = %s", message4);
	const char ** current_index4 = &(message4);
	resp_object_t * obj4 = parse_resp(current_index4);
	printf("message string was: %s\n", obj4->value.string);
}
*/
