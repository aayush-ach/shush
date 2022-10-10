#include "shush.h"
#include "console.h"
#include "comm.h"

void die(const char *s) {
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
         die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
         die("tcsetattr");
}







char* path = "~";
char prev[1024] = "NULL";
int tt = 0;
void sig_chld_handler()
{
    int status,pid;
    int flag = 0;
    while((pid = waitpid(-1,&status,WNOHANG)) > 0)
    {
        status = WIFEXITED(status);
        for(int i=0;i<100;i++)
        {
            if(bgids[i] == pid)
            {
                char* out = (char*)malloc(100*sizeof(char));
                sprintf(out,"\n%s with pid %d exited %s", bgnames[i], pid, status == 0 ? "abnormally" : "normally");
                write(1,out,strlen(out));
                break;
            }
        }
        flag = 1;
    }
    if(flag)
    {
        char* ps = (char*)malloc(1024);
        strcpy(ps,"\n");
        write(1,ps,strlen(ps));
        getPrompt(ps,tt);
        write(1,ps,strlen(ps));
    }
}

void sig_handler()
{
    char* ps = (char*)malloc(1024);
    getPrompt(ps,tt);
    write(1,"\n",1);
    if(!fgRunning)
    {
        write(STDOUT, ps, strlen(ps));
    }
    else
    {
        fgRunning = 0;
    }
}


int autocomplete(char inp[100]){
    char* cwd = (char*)malloc(1024);
    getcwd(cwd,1024);

}



int main()
{
    fgRunning = 0;
    tt=0;
    signal(SIGCHLD,sig_chld_handler);
    signal(SIGINT,sig_handler);
    signal(SIGTSTP,sig_handler);
    def = (char*)malloc(1024);
    for(int i=0;i<100;i++)
    {
        bgnames[i] = (char*)malloc(1024*sizeof(char));
    }
    char** commands = (char**)malloc(20*sizeof(char*)) ;
    tokens = (char**)malloc(1024*sizeof(char*));
    char* input_string = (char*)malloc(1024*sizeof(char));
    getcwd(def,1024);
    char* ps = (char*)malloc(1024);
    getPrompt(ps,tt);
    char c;
    while(1)
    {
        
        tt=0;
        char cur_word[100];
        for(int i=0;i<100;i++)
        {
            cur_word[i] = ' ';
        }
        int cwordptr=0;
        printf("%s",ps);
        // setbuf(stdout,NULL);
        // enableRawMode();
        // memset(input_string,'\0',100);
        // int pt=0;
        // while(read(STDIN_FILENO,&c,1) == 1)
        // {
        //     if(c == 10)
        //     {
        //         input_string[pt++] = '\n';
        //         input_string[pt] = '\0';
        //         printf("\n");
        //         break;
        //     }
        //     else if(c == 127)
        //     {
        //         if(pt > 0){
        //             input_string[--pt] = '\0';
        //             printf("\b \b");
        //             cur_word[cwordptr] = '\0';
        //             cur_word[cwordptr--] = ' ';
        //         }
        //     }
        //     else if (c == 9) { 
        //         // Tab Handling
        //         autocomplete(cur_word);

        //     } else if (c == 4) {
        //         printf("\n");
        //         exit(0);
        //     } else {
        //         input_string[pt++] = c;
        //         input_string[pt] = '\0';
        //         if(c == ' ' || c == '|' || c == ';' || c == '&')
        //         {
        //             strcpy(cur_word,"");
        //             cwordptr=0;
        //         }
        //         else
        //         {
        //             cur_word[cwordptr++] = c;
        //             cur_word[cwordptr] = '\0';
        //         }
        //         printf("%c", c);
        //     }
        //     fflush(stdout);
        // } 
        // disableRawMode();
        // input_string[pt] = '\0';
        if(!(fgets(input_string,1024,stdin)))
        {
            printf("\n");
            break;
        }
        int comms = getCommands(input_string,commands);
        for(int i=0;i<comms;i++)
        {
            check_execute_command(commands[i]);
        }
        getPrompt(ps,tt);
    }
    return 0;
}