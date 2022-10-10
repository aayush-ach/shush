#include "console.h"
#include "utils.h"





void getPrompt(char* ps,int t){
    struct passwd *pwd = getpwuid(getuid());
    char* user = pwd ->pw_name;
    char sysname[250];
    gethostname(sysname,250);
    char p[2048];
    char path[1024];
    char* cwd = (char*)malloc(1024);
    getcwd(cwd,1024);
    strcpy(path, cwd);
    relative(path);
    if(t > 1)
    {
        sprintf(ps,"<%s@%s:%stook %d seconds> ",user,sysname,path,t);
    }
    else sprintf(ps,"<%s@%s:%s> ",user,sysname,path);
    return;
}



