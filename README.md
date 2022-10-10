## Compilation

To compile - run the command "make all"
To clean all the executables - run "make clean"
I have assumed all commands, paths to be at a maximum of 1024 characters long

## Explanation of files

Contents of the following files are :-
    1. shush.c/shush.h - This contains the driver code for the shell along with a few global variables for all the files
    2. console.c/console.h - This contains the code necessary to print the prompt onto the terminal screen
    3. comm.c/comm.h - This contains the code that takes in commands, tokenizes them and runs them based on the command keyword
    4. builtins.c/builtins.h - This contains the code that can run the builtin commands such as ls,cd,echo,pwd
    5. utils.c/utils.h - This contains the code to execute non-built in commands and background/foreground processes


## Explanation of new commands 
The new commands are :-
    1."jobs" lists out all the stopped or running background jobs with pid number.
    2."sig" sends a certain signal number to the desired background process.
    3."fg" brings a background process to the foreground and executes it on the terminal.
    4."bg" causes a stopped background process to start running again(in the background).

## New Utilities
There are new functionalities of the shell :-
    1.Signal Handling - i)CTRL-Z pushes any foreground process to background and stops it.
                        ii)CTRL-C sends the SIGINT signal to any currently running foreground process.
                        iii)CTRL-D exits the terminal.
    2.Input-output redirection - Using '>','<' or '>>', the input or output streams can be redirected into files.
    3.Piping - Using '|', commands can be piped together, where the output of one command is the input of another command.
    4.Autocompletion - Pressing 'Tab' reveals a list of options for files in the current directory.
