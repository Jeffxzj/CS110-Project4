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
    int isBuiltin;
    int flag; //0b(>>)(>)(<)(|)(&) 5-bits representing on/off of these symbols 
    char **cmd;
    char *input;
    char *output;
} info_t;

// function to parse the commands into tokens and set the parse_info
int parseCommand(char *cmdLine, info_t *parsed);

#endif