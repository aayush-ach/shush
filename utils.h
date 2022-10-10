#ifndef __UTIL_H__
#define __UTIL_H__


#include "shush.h"
#include "comm.h"
#include "builtins.h"


void relative(char* path);
int fg(char* tokens[1024]);
int bg(char* tokens[1024]);
int pinfo(char* tokens[1024],int n);
void discover(char* tokens[1024],int no_args);
void disc_rec(char* dir_path,int dFlag,int fFlag,char* fName);
int jobs(int r,int s);
int bg_func(char* tokens[1024],int no_args);
void fg_func(char* tokens[1024],int no_args);
void rmproc(int pid);
void addproc(int pid, char* name);
typedef struct j_ob{
    int s_no;
    char status[20];
    char name[20];
    int pid;
} job;
int compr(const void* a,const void* b);
void sig(char* tokens[1024],int no_args);
#endif