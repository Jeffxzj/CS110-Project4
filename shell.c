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
    char *cmdList[4] = {"cd", "jobs", "exit","kill"};
    
    if (strcmp(cmd[0],cmdList[0]) == 0) {
        if (cmd[1] == NULL || strcmp(cmd[1], "~") == 0){
            char *user = getenv("USER");

        }
        else if (chdir(cmd[1]) == -1)
            printf("bash: cd: %s No such file or directory\n", cmd[1]);
    }
    if (strcmp(cmd[0],cmdList[1]) == 0)
        return 1;
    if (strcmp(cmd[0],cmdList[2]) == 0)
        exit(0);
    if (strcmp(cmd[0],cmdList[3]) == 0)
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
        cmd = parseCommand(cmdLine);
        //add_history(cmd);

        //record command in history list (GNU readline history ?)
        
        if (isBuiltInCommand(cmd))
            executeBuiltInCommand(cmd);
        else {		
            childPid = fork();
            if (childPid == 0) {
                execvp(cmd[0], cmd); //calls execvp  
            } else {
                if (isBackgroundJob(cmd)) {
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
