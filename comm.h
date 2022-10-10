#ifndef __COMM_H__
#define __COMM_H__
#include "shush.h"
#include "console.h"
int getCommands(char* command,char** commands);
void execute_command(char command[1024],int and_bit);
void check_execute_command(char* command);
void history();
void add_history(char* command);
void piping(char* command,int bg);
#endif