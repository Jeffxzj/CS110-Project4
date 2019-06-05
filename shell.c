#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <syscall.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h> 

#include "parse.c"

const int max_path = 1000;
const int max_host = 100;
const int max_line = 100;

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
/*
void 
printPrompt() {
    struct passwd *pwuid;

    char path[max_path]; 
    char hostname[max_host];
    pwuid = getpwuid(getuid());
    int len = strlen(pwuid->pw_dir);
    // get current working directory
    if (getcwd(path, max_path) == NULL)
        printf("directory cannot be determined");
    // get host name
    gethostname(hostname, max_host);
    if (strncmp(path, pwuid->pw_dir, len) == 0)
        printf("%s@%s:~%s", pwuid->pw_name, hostname, path+len);
    else
        printf("%s@%s:%s", pwuid->pw_name, hostname, path);
}
*/

int 
executeBuiltInCommand(char **cmd) {
    struct passwd *pwuid;
    // Command "cd"
    if (strcmp(cmd[0],Builtin[0]) == 0) {
        // "cd " and "cd ~" change curr directory to /home/username
        if (cmd[1] == NULL || strcmp(cmd[1], "~") == 0){
            pwuid = getpwuid(getuid());
            chdir(pwuid->pw_dir);
        } 
        else chdir(cmd[1]);
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
	int flag = 0;
    //jobl.size = 0;        
    FILE *fp;
    //if (argc > 1) {
    //printf("%s",argv[1]);
    fp = fopen(argv[1], "r");

    char *cmdLine, **cmd;
    int i=0; 
    while (!feof(fp)){
        pid_t childPid;
        int stat_loc;
        info_t parsed;
        init_parseinfo(&parsed);
        cmd = malloc(50 * sizeof(char*)); 
        cmdLine = malloc(max_line * sizeof(char));
        fgets(cmdLine, max_line, fp);
        parsed.cmd = cmd;
        parseCommand(cmdLine, &parsed);
        
        if (parsed.isBuiltin)
            executeBuiltInCommand(cmd);
        else {
            childPid = fork();
            if (childPid == 0)
                execvp(cmd[0], cmd); //calls execvp()
            else {
                if (((parsed.flag)&1) == 1) {
                    printf("background jobs\n");
                    //record in list of background jobs
                } else {
                    waitpid(childPid, &stat_loc, WUNTRACED);
                }		
            }
        }
    }
    free(cmd);
    free(cmdLine);
    fclose(fp);
/*
	while (1) {
        pid_t childPid;
        int stat_loc;
        char *cmdLine, **cmd; 
        if (flag == 1) {     
            cmdLine = malloc(max_line * sizeof(char));
            
        }
        else {
            printPrompt();
            cmdLine = readline("$ ");
        }
        
        
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
        free(cmdLine);
    }
*/
    fclose(fp);
    return 0;
}
