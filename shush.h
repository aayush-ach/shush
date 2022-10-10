#ifndef HEADER
#define HEADER

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <pwd.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <time.h>
#include <grp.h>
#include <sys/select.h>
#include <signal.h>
#include <termios.h>
#define STDIN 0
#define STDOUT 1
void sig_chld_handler();
void sig_handler();

typedef struct  struct_bg_info{
    int pid;
    char* name;
} bg_info;

char** tokens;
char* def;
char* previous;
char* bgnames[100];
int bgids[100];
bg_info bg_proc[100];
int fgRunning;
int autocomplete(char inp[100]);
#endif