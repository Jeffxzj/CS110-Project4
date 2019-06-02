#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <sys/wait.h>

#include "parse.c"
/*
int 
isBuiltInCommand(){

}

int 
executeBuiltInCommand(){

}
*/
void 
printPrompt() {
    char *user = getenv("USER");
    char *pwd = getenv("PWD");
    char *pwd_modified;
    char hostname[58];
    int len = strlen(user);
    gethostname(hostname, sizeof(hostname));

    for (int i=0; i<strlen(pwd); i++) {
        if (pwd[i]==user[0] && pwd[i+len-1]==user[len-1]) {
            pwd[0] = '~';
            for (int k=1; k<strlen(pwd); k++)
                pwd[k] = pwd[k+5+len];
        }
    }
    printf("%s@%s:%s$",user, hostname, pwd);
}



int 
main (int argc, char **argv)
{
	while (1) {
        pid_t childPid;
        char * cmdLine;
        printPrompt();

        cmdLine= readCommandLine(); //or GNU readline("");
        //cmd = parseCommand(cmdLine);

        //record command in history list (GNU readline history ?)

        if ( isBuiltInCommand(cmd)){
            executeBuiltInCommand(cmd);
        } else {		
            childPid = fork();
            if (childPid == 0){
            executeCommand(cmd); //calls execvp  

            } else {
                if (isBackgroundJob(cmd)){
                    //record in list of background jobs
                } else {
                    waitpid (childPid);
                }		
            }
        }
}
