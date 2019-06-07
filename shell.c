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
char Running[] = "Running";
char Done[] = "Done";
char Terminated[] = "Terminated";


typedef struct job_node
{
    int job_id;//id [1]
    int process_id;
    char* job_src;//copy the command;
    char* status;//[running] usually
    struct job_node* next; 
    struct job_node* prev; 
    int j;// = 0, if last one, = 1; last second = 2;
    int need_to_be_del;
}job_node; 

typedef struct jobs_list 
{
    int size;
    int cap;
    job_node head;
    job_node tail;
    char* status[3];
}jobs_list;
jobs_list jobl;

void ini_jobl(){
	
	//delete the first several lines in main function
	jobl.size = 0;
	jobl.cap = 1;
	jobl.head.next = &jobl.tail;
	jobl.tail.prev = &jobl.head;
	jobl.status[0] = Running;
	jobl.status[1] = Done;
	jobl.status[2] = Terminated;

	return;
}


void maintainjobl_j(){
	if (jobl.head.next == &jobl.tail)
	{//no job
		return;
	}
	if (jobl.head.next->next == &jobl.tail)
	{//only one job;
		jobl.head.next->j = 1;
		return;		
	}
	if (jobl.head.next->next->next == &jobl.tail)
	{//2 jobs
		jobl.head.next->next->j = 1;
		jobl.head.next->j = 2;
		return;
	}
	jobl.tail.prev->j = 1;
	jobl.tail.prev->prev->j = 2;
	jobl.tail.prev->prev->prev->j = 0;
	return;
}

void addjob(pid_t id, char* command){
	jobl.size ++;
	jobl.cap ++;
	job_node* new_job = (job_node*)malloc(sizeof(job_node));
	new_job->job_id = jobl.cap;
	new_job->process_id = id;
	new_job->prev = jobl.tail.prev;
	new_job->status = jobl.status[0];
	jobl.tail.prev->next = new_job;
	new_job->next = &jobl.tail;
	new_job->job_src = command;
	new_job->need_to_be_del = 0;
	jobl.tail.prev = new_job;
	new_job->j = 0;
	maintainjobl_j();
	return;
}

void jobs_maintain()
{//check every job if they have been done.
	job_node* temp;
	temp = jobl.head.next;
	if (jobl.size == 0)
	{
		return;
	}
	while(temp != &jobl.tail){
		pid_t pid = temp->process_id;
		if(temp->need_to_be_del)
		{//child process ended and need to be deleted
			job_node* tempprev = temp->prev;
			jobl.size--;
			job_node* tempnext = temp->next;
			tempprev->next = tempnext;
			tempnext->prev = tempprev;
			free(temp);
			maintainjobl_j();
		}
		else if (!waitpid(pid, NULL, WNOHANG)&& (temp->need_to_be_del == 0))
		{
			temp->need_to_be_del = 1;
		}
		temp = temp->next;
	}
	
    return;

}


void my_jobs()
{//first check every pid and maintain the jobl(free)
	// then maintain j
	//then print
	jobs_maintain();
//printf:
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
    return;
}
int
check_process(pid_t processid){
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


void kill_process(pid_t processid){
    //kill 6666
    job_node* temp;
    temp = jobl.head.next;
    if(!check_process(processid)){
    	return;
    }
    while(temp != &jobl.tail){
        if (temp->process_id == processid)
    	{
        	if (!strcmp(Running,temp->status))
        	{
        		//kill it;
        		kill(temp->process_id,SIGKILL);
        		temp->status = Terminated;
        	}
            return;
        }
        temp = temp->next;
    }
    return;
}

void
kill_num(int num){
    //kill %num 
    //previous to this, it has been checked that if this process exists
    if (!check_job(num))
    {
    	return;
    }        
    job_node* temp;
    temp = jobl.head.next;
    while(temp != &jobl.tail){
        if (temp->job_id == num)
    	{
        	if (!strcmp(Running,temp->status))
        	{
        		//kill it;
        		kill(temp->process_id,SIGKILL);
        		temp->status = Terminated;
        	}
            return;
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
    /*
    // Command "exit"
    if (strcmp(cmd[0], Builtin[1]) == 0)
        exit(0);
    */
    // Command "jobs"
    if (strcmp(cmd[0], Builtin[2]) == 0){
        printf("my_jobs\n");
    	my_jobs();
        printf("my_jobsend\n");
        return 1;
    }
 
    // Command "kill"
    if (strcmp(cmd[0], Builtin[3]) == 0){

        return 1;
    }

    return 0;
}

void 
executePipes(char **cmd, int *pipe_idx, int cmd_cnt) 
{
    // pipe number is always 1 less than cmd number
    int pipe_cnt = cmd_cnt - 1; 
    int pipefd[2*pipe_cnt]; 
    pid_t childpid;

    // initialize all the read/write file descriptions
    for (int i=0; i<pipe_cnt; i++) {
        pipe(pipefd+2*i);
    }
    int j = 0;
    for (int i=0; i<cmd_cnt; i++) {
        childpid = fork();
        if (childpid == 0) {
            // if not last command
            if (i != pipe_cnt) {
                // redirect stdout with input of 
                dup2(pipefd[j+1], STDOUT_FILENO);
            }
            // if not first command
            if (i != 0) {
                // redirect stdin with output of prev cmd
                dup2(pipefd[j-2], STDIN_FILENO);
            }

            for (int k=0; k<2*pipe_cnt; k++)
                close(pipefd[k]);
            
            execvp(cmd[pipe_idx[i]], cmd+pipe_idx[i]);
        }            
        j += 2; // go to next group of FO
    }
    // close all the file descriptions
    for (int i=0; i<2*pipe_cnt; i++)
        close(pipefd[i]);
    for (int i=0; i<cmd_cnt; i++)
        waitpid(childpid, NULL, WUNTRACED);

}

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
    FILE *fp;
    fp = fopen(argv[1], "r"); // open the .sh shell script
    ini_jobl();
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
                executePipes(cmd, pipe_idx, parsed.num);
                continue;
            }
            execvp(cmd[0], cmd); //calls execvp() to execute the cmd
        }
        else {
            // if the command is background
            
            if (parsed.flag & Bg) {
                //printf("background jobs\n");
                //record in list of background jobs
                
                addjob(childPid,cmdLine);
                execvp(cmd[0], cmd);
            } else {
            
            waitpid(childPid, NULL, WUNTRACED);
        }		
    }
    free(cmd);
    free(cmdLine);
    free(pipe_idx);
    fclose(fp);
    return 0;
}
}