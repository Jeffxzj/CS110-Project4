#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

void
init_parseinfo(info_t *info) {
    info->isBuiltin = 0;
    info->flag = 0;
    info->cmd = NULL;
    info->input = NULL;
    info->output = NULL;
}

int
parseCommand(char *cmdLine, info_t *parsed) {
    
    char *ignore = " "; // seperate by space
    char *token = strtok(cmdLine, ignore);
    int cnt = 0, i = 0 ;
    while (token) {
        parsed->cmd[cnt] = token;
        token = strtok(NULL, ignore);
        cnt++;
    }
    parsed->cmd[cnt] = NULL;
    
    char **cmd = parsed->cmd;
    if ((strcmp(cmd[0],Builtin[0])==0) || (strcmp(cmd[0],Builtin[1])==0)
    || (strcmp(cmd[0],Builtin[2])==0) || (strcmp(cmd[0],Builtin[3]))==0)
        parsed->isBuiltin = 1;
    
    if (strcmp(cmd[cnt-1], "&") == 0)
        parsed->flag |= Bg;

    while (i < cnt) {
        if (strcmp(cmd[i], "<") == 0) {
            parsed->flag |= Ri;
            parsed->input = cmd[i+1];
            i += 2;
        }
        if (strcmp(cmd[i], ">") == 0) { 
            parsed->flag |= Ro;
            parsed->output = cmd[i+1];
            i += 2;
        }
        if (strcmp(cmd[i], ">>") == 0) {
            parsed->flag |= Roo;
            parsed->output = cmd[i+1];
            i += 2;
        }
        if (strcmp(cmd[i], "|") == 0) {
            parsed->flag |= Pp;
            i++;
        }
        else i++;
    }
    return 0;
}