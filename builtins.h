#ifndef __BUILT_H__
#define __BUILT_H__
#include "shush.h"
#include "comm.h"
#include <ctype.h>

void echo(char* tokens[1024],int n);
void pwd();
void cd(char* tokens[1024],int n);
void ls_perms(unsigned int bits);
void ls_fileDetails(char* name,struct stat fStat,int *column);
void ls(char* tokens[1024],int outFlag);


#endif