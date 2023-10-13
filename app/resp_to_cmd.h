#ifndef RESP_TO_COMMAND_H
#define RESP_TO_COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include "parse_resp.h"

typedef enum {
	CMD_PING,
	CMD_ECHO,
	CMD_SET,
	CMD_GET
} cmd_type_t;

typedef struct cmd_object {
	cmd_type_t type;
	int num_args;
	char ** args;
} cmd_object_t;

cmd_object_t* resp_to_command(resp_object_t* obj);
void free_cmd_object(cmd_object_t *cmd);

#endif // RESP_TO_COMMAND_H

