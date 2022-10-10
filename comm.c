#include "shush.h"
#include "comm.h"
#include "builtins.h"
# include "utils.h"



int getCommands(char* command,char** commands)
{
    int n = 0;
    if(command == NULL){
        printf("\n");
        return 0;
    }
    commands[0] = (char*)malloc(1024);
    commands[0] = strtok(command,";");
    while(commands[n] != NULL)
    {
        n++;
        commands[n] = (char*)malloc(1024);
        commands[n] = strtok(NULL, ";");
    }

    return n;
}

int tokenize(char *command)
{

    int n = 0;
    
    char* com = (char*)malloc(1024);
    strcpy(com,command);
    tokens[0] = (char*)malloc(1024);
    tokens[0] = strtok(com, " \t\n\r\a");

    while(tokens[n] != NULL)
    {
        n++;
        tokens[n] = (char*)malloc(1024);
        tokens[n] = strtok(NULL, " \t\n\r\a");
    }
    return n;
}

void add_history(char* command)
{
    char* his_path = (char*)malloc(1024*sizeof(char));
    sprintf(his_path,"%s/.history",def);
    FILE* fPointer = fopen(his_path,"r");
    if(fPointer == NULL)
    {
        perror("Error");
    }
    char* check = (char*)malloc(1024*sizeof(char));
    strcpy(check,"");
    while(fgets(his_path,1024,fPointer) != NULL)
    {
        strcpy(check,his_path);
    }
    
    fclose(fPointer);
    sprintf(his_path,"%s/.history",def);
    if(strcmp(check,command))
    {
        
        FILE* fPoint = fopen(his_path,"a");
        fputs(command,fPoint);
        fclose(fPoint);
    }
    
}


void history()
{
    char* his_path = (char*)malloc(1024*sizeof(char));
    sprintf(his_path,"%s/.history",def);
    FILE* fPointer = fopen(his_path,"r");
    if(fPointer == NULL)
    {
        perror("Error");
    }
    while(fgets(his_path,1024,fPointer) != NULL)
    {
        printf("%s",his_path);
    }
}

void piping(char* command,int bg)
{
    int ex = 1;
    int no_coms = 1;
    for(int i=0;i<strlen(command);i++)
    {
        if(command[i] == '|')
        {
            no_coms++;
        }
    }
    if(no_coms == 1)
    {
        execute_command(command,bg);
        return;
    }
    char** commands = (char**)malloc((no_coms+1)*sizeof(char*));
    for(int i=0;i<=no_coms;i++)
    {
        commands[i] = (char*)malloc(1024*sizeof(char));
    }
    int n=0;
    commands[n] = strtok(command,"|");
    while(commands[n] != NULL)
    {
        n++;
        commands[n] = strtok(NULL,"|");
    }
    int no_pipes = no_coms - 1;
    int fd_arr[no_pipes*2];
    for(int i=0;i<no_pipes;i++)
    {
        int check = pipe(fd_arr+2*i);
        if(check)
        {
            fprintf(stderr,"Error in creating pipe\n");
            ex = -1;
            return;
        }
    }
    pid_t pipe_pid;
    for(int i=0;i<=no_pipes;i++)
    {
        pipe_pid = fork();
        if(!pipe_pid)
        {
            if(i != no_pipes)
            {
                dup2(fd_arr[2*i+1],1);
            }
            if(i > 0)
            {
                dup2(fd_arr[2*(i-1)],0);
            }
            for(int j=i;j<i+1;j++)
            {
                close(fd_arr[j]);
            }
            char* exec_command = commands[i];
            execute_command(exec_command,bg);
            exit(ex);
        }
    }
    for(int i=0;i<2*no_pipes;i++)
    {
        close(fd_arr[i]);
    }
    for(int i=0;i<=no_pipes;i++)
    {
        if(i == no_pipes)
        {
            wait(&ex);
            ex = WEXITSTATUS(ex);
        }
        else
        {
            wait(NULL);
        }
    }

}

void execute_command(char* command,int and_bit)
{
    int inFlag = 0,outFlag = 0;
    int in_fd_loc = -1;
    int out_fd_loc = -1;
    int no_args = tokenize(command);
    add_history(command);
    if(no_args == 0)
    {
        printf("%s","");
        return;
    }
    for(int i=0;i<no_args;i++)
    {
        if(!strcmp(tokens[i],">"))
        {
            outFlag=1;
            out_fd_loc = i+1;
        }
        else if(!strcmp(tokens[i],"<"))
        {
            inFlag = 1;
            in_fd_loc = i+1;
        }
        else if(!strcmp(tokens[i],">>"))
        {
            outFlag = 2;
            out_fd_loc = i+1;
        }
    }
    int std_in,std_out;
    int inFD,outFD;
    if(inFlag)
    {
        std_in = dup(STDIN);
        inFD = open(tokens[in_fd_loc],O_RDONLY);
        if(inFD < 0)
        {
            close(std_in);
            fprintf(stderr,"Input file not found\n");
            perror("Error");
            return;
        }
        if(dup2(inFD,STDIN) == -1)
        {
            fprintf(stderr,"Unable to redirect input\n");
            perror("Error");
            return;
        }
    }
    if(outFlag != 0)
    {
        std_out = dup(STDOUT);
        int open_method;
        if(outFlag == 1)
        {
            open_method = O_TRUNC;
        }
        else
        {
            open_method = O_APPEND;
        }
        outFD = open(tokens[out_fd_loc],O_CREAT | O_WRONLY | open_method,0644);
        if(outFD < 0)
        {
            close(std_out);
            fprintf(stderr,"Unable to open output file\n");
            perror("Error");
            return;
        }
        if(dup2(outFD,STDOUT) == -1)
        {
            fprintf(stderr,"Unable to redirect output\n");
            perror("Error");
            return;
        }
    }

    if(!strcmp(tokens[0],"cd"))
    {
        cd(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"echo"))
    {
        if(outFlag)
        {
            tokens[out_fd_loc - 1] = NULL;
        }
        if(inFlag)
        {
            tokens[in_fd_loc - 1] = NULL;
        }
        echo(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"pwd"))
    {
        pwd();
    }
    else if(!strcmp(tokens[0],"ls"))
    {
        if(outFlag)
        {
            tokens[out_fd_loc - 1] = NULL;
        }
        if(inFlag)
        {
            tokens[in_fd_loc - 1] = NULL;
        }
        ls(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"bash") || !strcmp(tokens[0],"exit"))
    {
        exit(0);
    }
    else if(!strcmp(tokens[0],"pinfo"))
    {
        pinfo(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"discover"))
    {
        discover(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"history"))
    {
        history();
    }
    else if(!strcmp(tokens[0],"jobs"))
    {
        int r =0,s=0;
        for(int i=0;i<no_args;i++)
        {
            if(!strcmp(tokens[i],"-r"))
            {
                r=1;
            }
            else if(!strcmp(tokens[i],"-s"))
            {
                s=1;
            }
        }
        jobs(r,s);
    }
    else if(!strcmp(tokens[0],"bg"))
    {
        bg_func(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"fg"))
    {
        fg_func(tokens,no_args);
    }
    else if(!strcmp(tokens[0],"sig"))
    {
        sig(tokens,no_args);
    }
    else
    {
        if(outFlag)
        {
            tokens[out_fd_loc - 1] = NULL;
        }
        if(inFlag)
        {
            tokens[in_fd_loc - 1] = NULL;
        }
        if(and_bit == 1)
        {
            bg(tokens);
        }
        else{
            fg(tokens);
        }
    }
    if(inFlag)
    {
        close(inFD);
        dup2(std_in,0);
        
    }
    if(outFlag)
    {
        close(outFD);
        
        dup2(std_out,1);
    }



    return;
}


void check_execute_command(char* command)
{
    int counter = 0;
    char** cmnds = (char**)malloc(1024*sizeof(char*));
    cmnds[0] = (char*)malloc(1024);
    cmnds[0] = strtok(command,"&");
    while(cmnds[counter] != NULL)
    {
        counter++;
        cmnds[counter] = (char*)malloc(1024);
        cmnds[counter] = strtok(NULL,"&");
    }
    for(int i=0;i<counter-1;i++)
    {
        piping(cmnds[i],1);
    }
    if(command[strlen(command) - 1] == '&')
    {
        piping(cmnds[counter-1],1);
    }
    else
    {
        piping(cmnds[counter-1],0);
    }
    return;
}