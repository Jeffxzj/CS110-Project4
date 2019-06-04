#ifndef PARSE_H
#define PARSE_H


//bool functions to determine command type
int isBuiltInCommand(char *cmd);
int isBackgroundJob(char *cmd);
char* parseCommand(char *cmdLine);


#endif