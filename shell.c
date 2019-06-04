#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h> 

#include "parse.c"

const int max_path = 1000;
const int max_host = 100;

char userpath[100]="/home/";

void 
printPrompt() {
    char *user = getenv("USER");
    int len = strlen(user);
    char path[max_path]; 
    char hostname[max_host];
    // get current working directory
    if (getcwd(path, max_path) == NULL)
        printf("directory cannot be determined");
    // get host name
    gethostname(hostname, max_host);
    // simplify /home/user with ~
    for (int i=0; i<strlen(path); i++) {
        if (path[i]==user[0] && path[i+len-1]==user[len-1]) {
            path[0] = '~';
            for (int k=1; k<strlen(path); k++)
                path[k] = path[k+5+len];
        }
    }
    printf("%s@%s:%s",user, hostname, path);
}

int 
executeBuiltInCommand(char **cmd) {
    
    // Command "cd"
    if (strcmp(cmd[0],Builtin[0]) == 0) {
        // "cd " and "cd ~" change curr directory to /home/username
        if (cmd[1] == NULL || strcmp(cmd[1], "~") == 0){
            char *user = getenv("USER");
            for (int i=0; i<strlen(user);i++)
                userpath[i+6] = user[i];
            //char *path = "/home/jeffxu";
            if (chdir(userpath) == -1)
                printf("No such file or directory\n");
        }
        else if (chdir(cmd[1]) == -1) {
            printf("No such file or directory\n");
        } 
    }
    // Command "exit"
    if (strcmp(cmd[0], Builtin[1]) == 0)
        exit(0);
    
    // Command "jobs"
    if (strcmp(cmd[0], Builtin[2]) == 0)
        return 1;
 
    // Command "kill"
    if (strcmp(cmd[0], Builtin[3]) == 0)
        return 1;

    return 0;
}

int 
main (int argc, char **argv)
{
	while (1) {
        pid_t childPid;
        int stat_loc;
        char *cmdLine, **cmd;
        
        printPrompt();
        cmdLine = readline("$ "); //or GNU readline("");
        info_t parsed;
        init_parseinfo(&parsed);
        cmd = malloc(20*sizeof(char*)); 
        parsed.cmd = cmd;
        parseCommand(cmdLine, &parsed);
        
        if (parsed.isBuiltin)
            executeBuiltInCommand(cmd);
        else {		
            childPid = fork();
            if (childPid == 0) {
                execvp(cmd[0], cmd); //calls execvp  
            } else {
                if (((parsed.flag)&1) == 1) {
                    printf("background jobs\n");
                    //record in list of background jobs
                } else {
                    waitpid (childPid, &stat_loc, WUNTRACED);
                }		
            }
        }
        free(cmd);
    }
}
