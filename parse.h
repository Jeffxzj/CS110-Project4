#ifndef PARSE_H
#define PARSE_H

const int Bg = 1;    // Flag for if Background
const int Pp = 2;    // Flag for if Piped
const int Ri = 4;    // Flag for if in-Redirection
const int Ro = 8;    // Flag for if out-Redirection (cover mode)
const int Roo = 16;  // Flag for if out-Redirection (append mode)
char *Builtin[4] = {"cd", "exit", "jobs", "kill"};    


void allocation_failed() {
    exit(1);
}

typedef struct Parse_info {
    int flag;         // 0b(>>)(>)(<)(|)(&) 5-bits representing on/off of these symbols 
    int isRedirect;   // if cmdline includes redirections
    int isBuiltin;    // if cmd is built-in command
    int num;          // number of commands, default 1
    char **cmd;
    char *input;
    char *output;
} info_t;

void init_parseinfo(info_t *info);
/* Parsing function to parse the commands into tokens and set the parse_info
return 0 if success, -1 if cmdLine is empty */
int parseCommand(char *cmdLine, char *cmd_cpy, int *pipe_cmd, info_t *parsed);

#endif