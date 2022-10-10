#include "utils.h"
int cur = 0;
extern int tt;
void relative(char *shush_path){
    if(strlen(shush_path) < strlen(def) || strncmp(shush_path, def, strlen(def)))
    {
        return;
    }
    sprintf(shush_path, "~%s", shush_path + strlen(def));
    return;
}

void rmproc(int pid)
{
    int found = 0;
    for (int i = 0; i < cur; i++)
    {
        if (bgids[i] == pid)
        {
            found = 1;
        }
        if (found)
        {
            if (i == cur - 1)
                bgids[i] = -1;
            else
            {
                bgids[i] = bgids[i + 1];
                strcpy(bgnames[i], bgnames[i + 1]);
            }
        }
    }
    cur--;
}

void addproc(int pid, char* name)
{
    if(cur == 100)
    {
        fprintf(stderr,"Child pool full:Insertion unsuccessful");
        
    }
    bgids[cur] = pid;
    strcpy(bgnames[cur],name);
    cur++;
    return;
}

int fg(char* tokens[1024])
{
    int pid;
    int status;
    time_t start,end;
    int time_taken;
    pid = fork();
    fgRunning = 1;
    if(pid == 0){
        execvp(tokens[0],tokens); 
        fprintf(stderr,"Command does not exist\n");
        return -1;
    }
    else if(pid < 0)
    {
        fprintf(stderr,"Error\n");
        return -1;
    }
    else{
        start = time(NULL);
        waitpid(pid, &status, WUNTRACED);
        if(WIFSTOPPED(status))
        {
            addproc(pid,tokens[0]);
        }
        
        end = time(NULL);
        time_taken = (end-start);
        if(time_taken > 1)
        {
            tt=time_taken;
        }
        fgRunning = 0;
    }
    return 0;
}


int bg(char* tokens[1024])
{
    
    int pid;
    int status;
    int child_pid = fork();
    int ex=0;
    if(child_pid == 0)
    {
        setpgid(0, 0);
        execvp(tokens[0],tokens);
        ex=1;
        perror("Error");
        
    }
    else if(child_pid < 0)
    {
        printf("Error\n");
        return -1;
    }
    else{
        if(!ex){
            addproc(child_pid,tokens[0]);
            printf("[%d] %d\n",cur,child_pid);
        }
    }
    return 0;
}


int pinfo(char* tokens[1024],int n)
{
    int pid = 0;
    if (tokens[1] != NULL)
    {
        pid = (int)atoi(tokens[1]);
    }
    if (!pid )
    {
        pid = getpid();
    }
    char fPath[20];
    sprintf(fPath, "/proc/%d/stat", pid);
    char fStr[1025];
    for(int i=0;i<1025;i++)
    {
        fStr[i] = '\0';
    }

    FILE *procStat;
    procStat = fopen(fPath, "r");
    if (procStat == NULL)
    {
        perror("Error");
        return -1;
    }
    fread(fStr, 1024, 1, procStat);
    fclose(procStat);

    char *attrs[1024];
    attrs[0] = (char*)malloc(1024*sizeof(char*));
    attrs[0] = strtok(fStr, " ");
    for (int i = 1; attrs[i - 1] != NULL; i++)
    {
        attrs[i] = (char*)malloc(1024*sizeof(char*));
        attrs[i] = strtok(NULL, " ");
    }
    printf("Pid -> %d\n", (int)pid);
    printf("Process Status -> %s\n", attrs[2]);
    printf("Memory -> %s\n", attrs[22]);
    sprintf(fPath, "/proc/%d/exe", pid);
    char* execPath;
    execPath = (char*)malloc(1025*sizeof(char));
    strcpy(execPath,"");
    int readStat = readlink(fPath, execPath, 1024);
    execPath[readStat] = '\0';
    relative(execPath);
    printf("Executable Path -> %s\n", execPath);
    return 0;
    
}

void discover(char* tokens[1024],int no_args)
{
    int no_dir;
    int flags = 0;
    int dFlag=0;
    int fFlag=0;
    int st = 1;
    int file_present = 0;
    char* dir_path = (char*)malloc(1024*sizeof(char));
    for(int i=1;i<no_args;i++)
    {
        if(tokens[i][0] == '-')
        {
            flags++;
            if(tokens[i][1] == 'd')
            {
                dFlag = 1;
            }
            else if(tokens[i][1] == 'f')
            {
                fFlag = 1;
            }
            else
            {
                printf("Invalid flag\n");
                return;
            }
        }
        else if(tokens[i][0] == '"')
        {
            file_present = 1;
        }
    }
    no_dir = no_args - 1 - flags - file_present;
    if(no_dir == 0)
    {
        strcpy(tokens[0],".");
        st = 0;
    }
    
    if(tokens[st][0] == '~')
    {
        strcpy(dir_path, def);
        strcat(dir_path, tokens[st]+1);
    }
    else
    {
        strcpy(dir_path,tokens[st]);
    }  
    
    if(!dFlag && !fFlag)
    {
        dFlag = fFlag = 1;
    }
    if(!file_present && dFlag)
        printf("%s \n",dir_path);
    char* fName = (char*)malloc(1024*sizeof(char));
    if(file_present)
    {
        strncpy(fName,tokens[no_args-1]+1,strlen(tokens[no_args-1])-2);
    }
    else
    {
        strcpy(fName,"");
    }
    
    disc_rec(dir_path,dFlag,fFlag,fName);
    return;
}


void disc_rec(char* dir_path,int dFlag,int fFlag, char* fName)
{
    DIR* disc_dir;
    disc_dir = opendir(dir_path);
    struct dirent* disc_list = readdir(disc_dir);
    if(!strcmp(fName,""))
    {
        while(disc_list != NULL)
        {
            if(disc_list->d_name[0] != '.')
            {
                struct stat fStat;
                char* p = (char*)malloc(1024*sizeof(char));
                sprintf(p,"%s/%s",dir_path,disc_list->d_name);
                lstat(p,&fStat);
                int bits = fStat.st_mode;
                if(S_ISDIR(bits))
                {
                    if(dFlag)
                        printf("%s\n",p);
                    disc_rec(p,dFlag,fFlag,"");
                }
                else if(S_ISREG(bits) && fFlag)
                {
                    printf("%s\n",p);
                }
            }
            disc_list = readdir(disc_dir);
        }
        closedir(disc_dir);
        return;
    }
    else
    {
        while(disc_list != NULL)
        {
            if(disc_list->d_name[0] != '.'){
                struct stat fStat;
                char* p = (char*)malloc(1024*sizeof(char));
                sprintf(p,"%s/%s",dir_path,disc_list->d_name);
                lstat(p,&fStat);
                int bits = fStat.st_mode;
                if(S_ISDIR(bits))
                {
                    disc_rec(p,dFlag,fFlag,fName);
                }
                else if(S_ISREG(bits))
                {
                    if(!strcmp(disc_list->d_name,fName)){
                        printf("%s\n",p);
                        return;
                    }
                }
            }

            disc_list = readdir(disc_dir);
        }
        closedir(disc_dir);
        return;
    }
}


int jobs(int r,int s)
{
    if(r == s)
    {
        r = 1;
        s = 1;
    }
    char* status = (char*)malloc(1024);
    char fPath[20];
    FILE* pStat;
    // job* job_list = (job*)malloc(sizeof(job)*cur);
    for(int i=0;i<cur;i++)
    {
        sprintf(fPath,"/proc/%d/stat",bgids[i]);
        pStat = fopen(fPath,"r");
        if(pStat == NULL)
        {
            continue;
        }
        char sRead[100];
        fread(sRead,100,1,pStat);
        char* tmp = strtok(sRead," ");
        tmp = strtok(NULL," ");
        // tmp = strtok(NULL," ");
        if(tmp[0] == 'S')
        {
            strcpy(status,"Running");
            if(!r)
            {
                continue;
            }
        }
        else
        {
            strcpy(status,"Stopped");
            if(!s)
            {
                continue;
            }
        }
        printf("[%d] %s %s [%d]\n",i+1,status,bgnames[i],bgids[i]);
    }
    return 0;
}


int bg_func(char* tokens[1024],int no_args)
{
    if(no_args != 2)
    {
        fprintf(stderr,"Incorrect usage\n");
        return -1;
    }
    int ind = atoi(tokens[1]);
    if(ind > cur)
    {
        perror("No such process");
        return 0;
    }
    return kill(bgids[ind - 1], SIGCONT);
}

void fg_func(char* tokens[1024],int no_args)
{
    if(no_args != 2)
    {
        fprintf(stderr,"Incorrect usage\n");
        return;
    }
    int status;
    int in = atoi(tokens[1]);
    if (in > cur)
    {
        fprintf(stderr, "No such process\n");
        return;
    }
    int pid = bgids[in - 1];
    fgRunning = 1;
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    if(tcsetpgrp(STDIN, getpgid(pid)))
    {
        fprintf(stderr, "Could not give terminal control to job %d\n", in);
    }
    else if(kill(pid, SIGCONT))
    {
        fprintf(stderr, "Unable to resume job %d\n", in);
    }
    else
    {
        if(waitpid(pid, &status, WUNTRACED) > 0)
        {
            if(!WIFSTOPPED(status))
            {
                rmproc(pid);
            }
        }
        fgRunning = 0;
        if(tcsetpgrp(STDIN, getpgid(0)))
        {
            fprintf(stderr, "Could not return terminal controll to the shell. Exiting the shell\n");
            exit(1);
        }
    }
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    return;
}

void sig(char* tokens[1024],int no_args)
{
    if(no_args != 3)
    {
        fprintf(stderr,"Wrong syntax\n");
        return;
    }
    int job_number = atoi(tokens[1]);
    if(job_number > cur)
    {
        fprintf(stderr,"Job number does not exist\n");
        return;
    }
    int signal_no = atoi(tokens[2]);
    int pid = bgids[job_number-1];
    if(kill(pid,signal_no))
    {
        fprintf(stderr,"Incorrect signal number given\n");
    }
    return;
}
