#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <syscall.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

//#include <readline/readline.h>
//#include <readline/history.h> 

#include "parse.c"

const int max_path = 1000;
const int max_host = 100;
const int max_line = 100;

/*
typedef struct job_node
{
	int job_id;//id [1]
	int process_id;
	char* job_src;//copy the command;
	char* status;//[running] usually
	job_node* next; 
	job_node* prev; 
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
*/

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
    /*
    // Command "exit"
    if (strcmp(cmd[0], Builtin[1]) == 0)
        exit(0);
    */
    // Command "jobs"
    if (strcmp(cmd[0], Builtin[2]) == 0){
    	//my_jobs();
        return 1;
    }
 
    // Command "kill"
    if (strcmp(cmd[0], Builtin[3]) == 0){

        return 1;
    }

    return 0;
}
/*
void 
executePipes(char **cmd, int *pipe_idx, int pipe_cnt) {
    int pipe[2*pipe_cnt];
    pid_t childpid;
    
    
}
*/

void 
executeRedirections(info_t *parsed) 
{
    // "<" redirection
    if (parsed->flag & Ri) {
        // open file in ReadOnly Mode, 
        // if not exist create it with read/write permission
        int fo = open(parsed->input, O_RDONLY|O_CREAT, 00666);
        dup2(fo, STDIN_FILENO); // redirect stdin to input file fo
        close(fo);
    }
    // ">" redirection
    if (parsed->flag & Ro) {
        int fo = open(parsed->output, O_CREAT|O_WRONLY|O_TRUNC, 00666);
        dup2(fo, STDOUT_FILENO); // redirect stout to output file fo;
        close(fo);
    }
    // ">>" redirection
    if (parsed->flag & Roo) {
        int fo = open(parsed->output, O_CREAT|O_WRONLY|O_APPEND, 00666);
        dup2(fo, STDOUT_FILENO); // redirect stout to output file fo;
        close(fo);
    }
    execvp(parsed->cmd[0], parsed->cmd); //calls execvp() to execute the cmd
}

void 
cmdPrinter(char **cmd) 
{
    int i=0;
    printf("cmd start with:   ");
    while(cmd[i] != 0) {
        printf("%s ", cmd[i]);
        i++;
    }
    printf("\n");
}

int 
main (int argc, char **argv)
{
	//int flag = 0;
    //jobl.size = 0;        
    FILE *fp;
    //if (argc > 1) {
    //printf("%s",argv[1]);
    fp = fopen(argv[1], "r"); // open the .sh shell script
    
    char *cmdLine, **cmd;
    int *pipe_idx;
    // cmd tokens after parsing
    cmd = malloc(max_line * sizeof(char*)); 
    // piping commands indexs in cmd
    pipe_idx = malloc(max_line * sizeof(int));
    
    while (!feof(fp)){
        pid_t childPid;
        // initialize the parse_info
        info_t parsed;
        init_parseinfo(&parsed);
        memset(cmd, 0, max_line*sizeof(char*));
        memset(pipe_idx, 0, max_line*sizeof(int));
        // cmdLine recieved by fgets()    
        cmdLine = malloc(max_line * sizeof(char));
        fgets(cmdLine, max_line, fp);
        parsed.cmd = cmd;
        // if the line is empty continue
        if (parseCommand(cmdLine, pipe_idx, &parsed) == -1)
            continue;
        //mdPrinter(cmd);
        
        // if cmd is Built-in command
        if (parsed.isBuiltin) {
            if (strcmp(cmd[0], Builtin[1]) == 0)
                break;
            executeBuiltInCommand(cmd);
            continue;
        }
        childPid = fork();
        // for the new process
        if (childPid == 0){
            // Only redirection, no pipes
            if (parsed.isRedirect && !(parsed.flag & Pp)) {
                //printf("fuck\n");
                executeRedirections(&parsed);
                continue;
            }
            // if piped
            if (parsed.flag & Pp) {
                /*
                printf("%d\n",parsed.num);
                for (int k=0; k<parsed.num; k++){
                    printf("%d ",pipe_idx[k]);
                    printf("\n");
                }
                */
                continue;
            }
            // if cmd only includes redirection
            execvp(cmd[0], cmd); //calls execvp() to execute the cmd
        }
        else {
            // if the command is background
            /*
            if (parsed.flag & Bg) {
                //printf("background jobs\n");
                //record in list of background jobs
            } else {
            */
            waitpid(childPid, NULL, WUNTRACED);
        }		
    }
    free(cmd);
    free(cmdLine);
    free(pipe_idx);
    fclose(fp);
    return 0;
}

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