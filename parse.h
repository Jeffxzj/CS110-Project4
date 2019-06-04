#ifndef PARSE_H
#define PARSE_H


void allocation_failed() {
    exit(1);
}
// function to parse the commands into tokens
char **parseCommand(char *cmdLine);
// bool functions to determine command type
int isBuiltInCommand(char **cmd);
int isBackgroundJob(char **cmd);


#endif