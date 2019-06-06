#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

void
init_parseinfo(info_t *info) {
    info->flag = 0;
    info->isBuiltin = 0;
    info->isRedirect = 0;
    info->num = 1;
    info->cmd = NULL;
    info->input = NULL;
    info->output = NULL;
}

int
parseCommand(char *cmdLine, int *pipe_cmd, info_t *parsed) {
    
    char *ignore = " ";                    // seperate cmds by space
    cmdLine = strtok(cmdLine, "\n");       // get rid of "\n" at the end
    char *token = strtok(cmdLine, ignore);
    int cnt = 0, i = 0;
    while (token) {
        parsed->cmd[cnt] = token;
        token = strtok(NULL, ignore);
        cnt++;
    }
    if (cnt == 0) {
        parsed->num = 0;
        return -1;
    }
    char **cmd = parsed->cmd;
    if ((strcmp(cmd[0],Builtin[0])==0) || (strcmp(cmd[0],Builtin[1])==0)
    || (strcmp(cmd[0],Builtin[2])==0) || (strcmp(cmd[0],Builtin[3]))==0)
        parsed->isBuiltin = 1;
    
    if (strcmp(cmd[cnt-1], "&") == 0)
        parsed->flag |= Bg;

    while (i < cnt) {
        
        if (strcmp(cmd[i], "|") == 0) {
            parsed->flag |= Pp;          // set the "pipe" flag
            pipe_cmd[parsed->num] = i+1; // record the idx of cmd next to "|" in **cmd
            parsed->num++;               // increase the command number
            i += 2;
            continue;
        }
        if (strcmp(cmd[i], "<") == 0) {
            parsed->flag |= Ri;
            parsed->input = cmd[i+1];
            parsed->isRedirect = 1;
            i += 2;
            continue;
        }
        if (strcmp(cmd[i], ">") == 0) { 
            parsed->flag |= Ro;
            parsed->output = cmd[i+1];
            parsed->isRedirect = 1;
            i += 2;
            continue;
        }
        if (strcmp(cmd[i], ">>") == 0) {
            parsed->flag |= Roo;
            parsed->output = cmd[i+1];
            parsed->isRedirect = 1;
            i += 2;
            continue;
        }
        else i++;
    }
    return 0;
}