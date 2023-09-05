#ifndef RESP_TO_COMMAND_H
#define RESP_TO_COMMAND_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
	CMD_ECHO,
	CMD_SET,
	CMD_GET
} cmd_type_t;

typedef struct cmd_object {
	cmd_type_t type;
	int num_args;
	char ** args;
} cmd_object_t;

cmd_object_t* resp_to_command(const char **input);

#endif // RESP_TO_COMMAND_H
