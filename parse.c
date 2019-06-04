#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"


char** 
parseCommand(char *cmdLine) {
    char **cmd = malloc(10 * sizeof(char*));  //parsed cmd to return
    if (cmd == NULL) allocation_failed();
    
    char *ignore = " "; // seperate sign
    char *token = strtok(cmdLine, ignore);
    int i = 0;
    while (token) {
        cmd[i] = token;
        token = strtok(NULL, ignore);
        i++;
    }
    return cmd;
}

int 
isBuiltInCommand(char **cmd) {
    char *cmdList[4] = {"cd", "jobs", "exit","kill"};

    if (strcmp(cmd[0],cmdList[0]) == 0)
        return 1;
    if (strcmp(cmd[0],cmdList[1]) == 0)
        return 1;
    if (strcmp(cmd[0],cmdList[2]) == 0)
        return 1;
    if (strcmp(cmd[0],cmdList[3]) == 0)
        return 1;
    else 
        return 0;
}

int 
isBackgroundJob(char **cmd) {
    
    int l = strlen(*cmd) - 1;
    int ll = strlen(cmd[l]) - 1;
    if (!strcmp(cmd[l], "&") == 0 || strcmp(&cmd[l][ll],"&") == 0)
        return 1;

    return 0;
}




