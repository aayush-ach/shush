#include "builtins.h"
extern char prev[1024];

int max(int a,int b)
{
    return (a > b?a:b);
}



int cmpr(const void* a,const void* b)
{
    const char *s1 = *( const char ** )a;
    const char *s2 = *( const char ** )b;
    
    while ( *s1 && tolower( ( unsigned char )*s1 ) == tolower( ( unsigned char )*s2 ) )
    {
        ++s1;
        ++s2;
    }
    
    return tolower( ( unsigned char )*s1 ) - tolower( ( unsigned char )*s2 );
}

void echo(char* tokens[1024],int no_args)
{
    for(int i=1;tokens[i] != NULL;i++)
    {
        printf("%s ",tokens[i]);
    }
    printf("\n");
    return;
}
void pwd()
{
    char* buf = (char*)malloc(1024);
    getcwd(buf,1024);
    printf("%s\n",buf);
}
void cd(char* tokens[1024],int no_args){

    char cdpath[1024];
    if(no_args == 1)
    {
        strcpy(cdpath, def);
        getcwd(prev,1024);
    }
    else if(tokens[1][0] == '~'){
        strcpy(cdpath, def);
        strcat(cdpath, tokens[1]+1);
        getcwd(prev,1024);
    }
    else if(!strcmp(tokens[1],"-"))
    {
        if(!strcmp(prev,"NULL"))
        {
            printf("cd: OLDPWD NOT SET\n");
            return;
        }
        else
        {
            printf("%s\n",prev);
            strcpy(cdpath,prev);
            getcwd(prev,1024);
        }
    }
    else{
        strcpy(cdpath, tokens[1]);
        getcwd(prev,1024);
    }
    if(chdir(cdpath) < 0){
        perror("Directory not found");
    }
    return;
}

void ls_perms(unsigned int bits)
{
    char perms[] = "----------";
     if (S_ISDIR(bits))
        perms[0] = 'd';
    else if (S_ISLNK(bits))
        perms[0] = 'l';
    else if (S_ISBLK(bits))
        perms[0] = 'b';
    else if (S_ISCHR(bits))
        perms[0] = 'c';
    else if (S_ISSOCK(bits))
        perms[0] = 's';
    else if (S_ISFIFO(bits))
        perms[0] = 'f';
    else if (S_ISREG(bits))
        perms[0] = '-';
    else
        perms[0] = '?';

    if (bits & S_IRUSR)
        perms[1] = 'r';
    if (bits & S_IWUSR)
        perms[2] = 'w';
    if (bits & S_IXUSR)
        perms[3] = 'x';
    if (bits & S_IRGRP)
        perms[4] = 'r';
    if (bits & S_IWGRP)
        perms[5] = 'w';
    if (bits & S_IXGRP)
        perms[6] = 'x';
    if (bits & S_IROTH)
        perms[7] = 'r';
    if (bits & S_IWOTH)
        perms[8] = 'w';
    if (bits & S_IXOTH)
        perms[9] = 'x';
    
    printf("%s ",perms);
}


void ls_fileDetails(char* name,struct stat fStat,int *column)
{
    ls_perms(fStat.st_mode);
    int l = 1;
    int x = fStat.st_nlink;
    while(x)
    {
        x/=10;
        if(x == 0)
        {
            break;
        }
        l++;
    }
    for(int i=1;i<column[1];i++)
    {
        printf(" ");
        
    }
    printf("%lu ",fStat.st_nlink);
    printf("%s ",getpwuid(fStat.st_uid)->pw_name);
    l = strlen(getpwuid(fStat.st_uid)->pw_name);
    for (int i = l; i < column[2]; i++)
        printf(" ");
    
    printf("%s ", getgrgid(fStat.st_gid)->gr_name);
    l = strlen(getgrgid(fStat.st_gid)->gr_name);
    for (int i = l; i < column[3]; i++)
        printf(" ");


    l = 1;
    x = fStat.st_size;
    while(x)
    {
        x/=10;
        if(x == 0)
        {
            break;
        }
        l++;
    }
    for (int i = l; i < column[4]; i++)
        printf(" ");
    printf("%lu ", fStat.st_size);

    char* f_time = (char*)malloc(100);
    strcpy(f_time,asctime(localtime(&(fStat.st_mtime))));
    f_time[strlen(f_time) - 1] = '\0';
    printf("%s ",f_time);


    printf("%s\n",name);
}


void ls(char* tokens[1024],int outFlag)
{
    int n = 0;
    while(tokens[n] != NULL)
    {
        n++;
    }
    char dir_path[256];
    char fPath[512];
    int aFlag =0 ;
    int lFlag = 0;
    int totalFlag = 0;
    for(int i=0;i<n;i++)
    {
        if(tokens[i][0] == '-')
        {
            totalFlag++;
            for(int j = 1;j < strlen(tokens[i]);j++)
            {
                if(tokens[i][j] == 'a')
                {
                    aFlag=1;
                }
                else if(tokens[i][j] == 'l')
                {
                    lFlag = 1;
                }
                else
                {
                    printf("Invalid flag\n");
                    return;
                }
            }
        }
    }
    int no_dir = n - 1 - totalFlag;
    int st = 1;
    if(no_dir == 0)
    {
        strcpy(tokens[0],".");
        st = 0;
    }
    for(int i=st;i<n;i++)
    {
        if(tokens[i][0] != '-')
        {
            int column[7] = {0,0,0,0,0,0,0};
            DIR* lsDir;
            if(tokens[i][0] == '~')
            {
                strcpy(dir_path, def);
                strcat(dir_path, tokens[i]+1);
            }
            else if(tokens[i][0] == '.')
            {
                char* cwd = (char*)malloc(1024);
                getcwd(cwd,1024);
                strcpy(dir_path,cwd);
            }
            else
            {
                strcpy(dir_path,tokens[i]);
            }
            lsDir = opendir(dir_path);
            if(no_dir > 1)
            {
                printf("%s:\n",dir_path);
            }
            int size = 0;
            struct dirent* lsCheck = readdir(lsDir);
            char** lsArr = (char**)malloc(1024*sizeof(char*));
            int s = 0;
            while(lsCheck != NULL)
            {
                lsArr[s] = (char*)malloc(1024);
                strcpy(lsArr[s],lsCheck->d_name);
                s++;
                lsCheck = readdir(lsDir);
            }
            closedir(lsDir);
            qsort(lsArr,s,sizeof(char*),cmpr);
            
            for(int j=0;j<s;j++)
            {
                if (lFlag)
                {
                    sprintf(fPath, "%s/%s", dir_path, lsArr[j]);
                    struct stat fStat;
                    lstat(fPath,&fStat);
                    int hFlag = (lsArr[j][0] == '.');
                    if ((aFlag && hFlag) || (!hFlag))
                    {
                        column[0] = 10;
                        int k = 0;
                        long long x = (long long)fStat.st_nlink;
                        while(x)
                        {
                            x/=10;
                            k++;
                        }
                        column[1] = max(column[1], k);
                        column[2] = max(column[2], strlen(getpwuid(fStat.st_uid)->pw_name));
                        column[3] = max(column[3], strlen(getgrgid(fStat.st_gid)->gr_name));
                        k = 0;
                        x = (long long)fStat.st_size;
                        while(x)
                        {
                            x /= 10;
                            k++;
                        }

                        column[4] = max(column[4], k);
                        column[5] = -1;
                        column[6] = max(column[6], strlen(lsArr[j]));
                        size += (fStat.st_blocks * 512 + 1023) / 1024;
                    }
                }
            }
            
            if(lFlag)
            {
                printf("total %d\n",size);
            }
            DIR* lsPrintDir = opendir(dir_path);
            struct dirent* lsPrint = readdir(lsPrintDir);
            char** lsPrintArr = (char**)malloc(1024*sizeof(char*));
            int m = 0;
            while(lsPrint != NULL)
            {
                lsPrintArr[m] = (char*)malloc(1024);
                strcpy(lsPrintArr[m],lsPrint->d_name);
                m++;
                lsPrint = readdir(lsPrintDir);
            }
            closedir(lsPrintDir);
            qsort(lsPrintArr,m,sizeof(char*),cmpr);

            for(int k=0;k<m;k++)
            {
                if (!lFlag)
                {
                    char hiddenFlag = (lsPrintArr[k][0] == '.');
                    if ((hiddenFlag && aFlag) || (!hiddenFlag)){
                        char pat[1024];
                        sprintf(pat,"%s/%s",dir_path,lsArr[k]);
                        struct stat f;
                        lstat(pat,&f);
                        printf("%s\n", lsPrintArr[k]);
                    }
                }
                else
                {
                    sprintf(fPath, "%s/%s", dir_path, lsArr[k]);
                    struct stat fileStat;
                    lstat(fPath,&fileStat);
                    char hiddenFlag = (lsPrintArr[k][0] == '.');
                    if ((hiddenFlag && aFlag) || (!hiddenFlag))
                        ls_fileDetails(lsPrintArr[k], fileStat, column);
                }
            }
            if (lFlag)
                printf("\x1B[A\b");
            printf("%s\n", no_dir > 1 ? "\n" : "");
        }
    }
    if (no_dir > 1 && outFlag == 0){
        printf("\x1B[A\b");
    }
    
    return;
}