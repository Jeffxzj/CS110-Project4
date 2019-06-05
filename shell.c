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

typedef struct job_node
{
	int job_id;//id [1]
	int process_id;
	char* job_src;//copy the command;
	char* status;//[running] usually
	struct job_node* next; 
	struct job_node* prev; 
	int j;// = 0, if last one, = 1; last second = 2;
}job_node;

typedef struct jobs_list 
{
	int size;
	int cap;
	job_node head;
	job_node tail;
}jobs_list;

jobs_list jobl;


int
check_process(int processid){
	job_node* temp;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return 0;
	}
	while(temp != &jobl.tail){
		if (temp->process_id == processid)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}


int  
check_job(int num){
	job_node* temp;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return 0;
	}
	while(temp != &jobl.tail){
		if (temp->job_id == num)
		{
			return 1;
		}
		temp = temp->next;
	}
	return 0;
}



void kill_process(int processid){
	//kill 6666
    job_node* temp;
	job_node* temp_prev;
	job_node* temp_next;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return;
	}
	while(temp != &jobl.tail){
		if (temp->process_id == processid)
		{
			temp_prev = temp->prev;
			temp_next = temp->next;
			temp_prev->next = temp_next;
			temp_next->prev = temp_prev;
			jobl.size--;
			free(temp);
			return;
		}
		temp = temp->next;
	}
}

void
kill_num(int num){
	//kill %num 
	//previous to this, it has been checked that if this process exists
	job_node* temp;
	job_node* temp_prev;
	job_node* temp_next;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return;
	}
	while(temp != &jobl.tail){
		if (temp->job_id == num)
		{
			temp_prev = temp->prev;
			temp_next = temp->next;
			temp_prev->next = temp_next;
			temp_next->prev = temp_prev;
			jobl.size--;
			free(temp);
			return;
		}
		temp = temp->next;
	}
}


void 
my_jobs(){
	job_node* temp;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return;
	}
	while(temp != &jobl.tail){
		if (temp->j == 0)
		{
			printf("[%d]   %s                 %s\n", temp->job_id,temp->status,temp->job_src);
		}
		else if(temp->j == 1)//+
		{
			printf("[%d]-  %s                 %s\n", temp->job_id,temp->status,temp->job_src);
		}
		else if(temp->j == 2)//-
		{
			printf("[%d]+  %s                 %s\n", temp->job_id,temp->status,temp->job_src);
		}
		temp = temp->next;
	}
}

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
    if (strcmp(cmd[0], Builtin[2]) == 0){
    	my_jobs();
        return 1;
    }
 
    // Command "kill"
    if (strcmp(cmd[0], Builtin[3]) == 0){

        return 1;
    }

    return 0;
}

int 
main (int argc, char **argv)
{
	jobl.size = 0;



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
