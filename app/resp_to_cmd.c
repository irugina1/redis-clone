#include "resp_to_cmd.h"
#include <ctype.h>


cmd_object_t* resp_to_command(resp_object_t* obj ){
	// struct we'll return
	cmd_object_t *cmd = malloc(sizeof(cmd_object_t));
	// read command name from resp parsed object and make lowercase
	char* cmd_name = obj->value.array.elements[0]->value.string;
	for(int i = 0; cmd_name[i]; i++){
		cmd_name[i] = tolower(cmd_name[i]);
	}
	// set cmd type
	if (strcmp(cmd_name, "echo") == 0) {
		cmd->type = CMD_ECHO;
	}
	else if (strcmp(cmd_name, "set") == 0) {
		cmd->type = CMD_SET;
	}
	else if (strcmp(cmd_name, "get") == 0) {
		cmd->type = CMD_GET;
	}
	else{
		cmd->type = CMD_PING;
		return cmd;
	}
	// set number of args
	int num_args = obj->value.array.len;
	cmd->num_args = num_args;
	// set the args
	cmd->args = malloc(sizeof(char *) * num_args);
	
	for (int i =0; i < num_args; i++){
		strcpy(cmd->args[i], obj->value.array.elements[i]->value.string);
	}
	
	return cmd;
}
