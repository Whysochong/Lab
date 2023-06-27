/**
 * CS2106 AY22/23 Semester 2 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void proc_update_status(pid_t pid, int status, int exitCode);

struct PCBTable *processes;
struct PCBTable* pointer;
int num_processes;

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void signal_handler(int signo)
{

    // Use the signo to identy ctrl-Z or ctrl-C and print “[PID] stopped or print “[PID] interrupted accordingly.
    // Update the status of the process in the PCB table

    //printf("im in signal_handler");

    int exit_code;
    if (signo == SIGTSTP) { // CTRL-Z
        printf("[%d] stopped.\n", pointer->pid);
        pointer->status = 4;
        // pointer->exitCode = 1;
        pointer++;
        
    } 
    if (signo == SIGINT) { // CTRL-C
        if (processes->status == 2) {
            printf("[%d] interrupted.\n", pointer->pid);
            proc_update_status(pointer->pid, 1, WEXITSTATUS(exit_code));
            pointer++;
        }
    }
        
}



static void proc_update_status(pid_t pid, int status, int exitCode)
{
    /******* FILL IN THE CODE *******/

    // Call everytime you need to update status and exit code of a process in PCBTable

    // May use WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG, WIFSTOPPED

    for (int i = 0; i < num_processes; i++)
    {
        if (processes[i].pid == pid)
        {
            processes[i].exitCode = exitCode;
            processes[i].status = status;
        }
    }
}

static void check_status()
{
    for (int i = 0; i < num_processes; i++)
    {
        int exit_status;
        pid_t updated = waitpid(processes[i].pid, &exit_status, WNOHANG);

        if (updated != processes[i].pid)
        {
            continue;
        }
        //printf("Updating status pid %d, status %d", updated, exit_status);
        if (WIFEXITED(exit_status))
        {
            processes[i].exitCode = WEXITSTATUS(exit_status);
            processes[i].status = 1;
        }
        else if (WIFSIGNALED(exit_status))
        {
            processes[i].exitCode = WTERMSIG(exit_status);
            processes[i].status = 1;
        }
    }
}

/*******************************************************************************
 * Built-in Commands
 ******************************************************************************/

static void command_info(int option)
{

    /******* FILL IN THE CODE *******/
    int running = 0, exited = 0, terminating = 0, stopped = 0;
    // If option is 0
    // Print details of all processes in the order in which they were run. You will need to print their process IDs, their current status (Exited, Running, Terminating, Stopped)
    // For Exited processes, print their exit codes.

    for (int i = 0; i < num_processes; i++)
    {
        struct PCBTable curr_process = processes[i];

        // printf("Process: %d. Current status: %d\n", i, curr_process.status);
        if (curr_process.status == 1)
        {
            if (option == 0)
            {
                printf("[%d] Exited %d\n", curr_process.pid, curr_process.exitCode);
            }
            exited++;
        }
        if (curr_process.status == 2)
        {
            if (option == 0)
            {
                printf("[%d] Running\n", curr_process.pid);
            }
            running++;
        }
        if (curr_process.status == 3)
        {
            if (option == 0)
            {
                printf("[%d] Terminating\n", curr_process.pid);
            }
            terminating++;
        }
        if (curr_process.status == 4)
        {
            if (option == 0)
            {
                printf("[%d] Stopped\n", curr_process.pid);
            }
            stopped++;
        }
    }

    // If option is 1
    // Print the number of exited process.
    if (option == 0)
    {
    }
    else if (option == 1)
    {
        printf("Total exited process: %d\n", exited);
    }
    // If option is 2
    // Print the number of running process.
    else if (option == 2)
    {
        printf("Total running process: %d\n", running);
    }
    // If option is 3
    // Print the number of terminating process.
    else if (option == 3)
    {
        printf("Total terminating process: %d\n", terminating);
    }
    // If option is 4
    // Print the number of stopped process.
    else if (option == 4)
    {
        printf("Total stopped process: %d\n", stopped);
    }
    // For all other cases print “Wrong command” to stderr.
    else
    {
        fprintf(stderr, "Wrong Command\n");
        return;
    }
}

static void command_wait(pid_t pid)
{

    /******* FILL IN THE CODE *******/

    // Find the {PID} in the PCBTable
    int exit_status_code;
    for (int i = 0; i < num_processes; i++)
    {
        // If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
        // After the process terminate, update status and exit code (call proc_update_status())
        if (processes[i].pid == pid && processes[i].status == 2)
        {
            pid_t updated = waitpid(pid, &exit_status_code, WUNTRACED);
            if (updated != processes[i].pid)
            {
                continue;
            }
            if (WIFEXITED(exit_status_code))
            {
                processes[i].exitCode = WEXITSTATUS(exit_status_code);
                processes[i].status = 1;
            }
            else if (WIFSIGNALED(exit_status_code))
            {
                processes[i].exitCode = WTERMSIG(exit_status_code);
                processes[i].status = 1;
            }
        }
    }

    // Else, continue accepting user commands.
}

static void command_terminate(pid_t pid)
{

    /******* FILL IN THE CODE *******/

    // Find the pid in the PCBTable
    for (int i = 0; i < num_processes; i++)
    {
        if (processes[i].pid == pid)
        {
            if (processes[i].status == 2)
            {
                processes[i].status = 3;
                kill(processes[i].pid, SIGTERM);
            }
        }
    }
    // If {PID} is RUNNING:
    // Terminate it by using kill() to send SIGTERM
    // The state of {PID} should be “Terminating” until {PID} exits
}


static void command_fg(pid_t pid) // i think no parameters are needed
{

    /******* FILL IN THE CODE *******/
    // decrement pointer as you want to retreive the previous pid if we CTRL-Z again (cause of the fact we incremented the pointer in signal handler after running it)
    pointer--;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].pid == pid) {
            int exit_code;
            if (processes[i].status == 4) 
            {
                printf("[%d] resumed\n", processes[i].pid);
                // resume running
                kill(processes[i].pid, SIGCONT);
                processes[i].status = 2;
                // wait
                // WUNTRACED flag causes waitpid() to return for stopped children as well as terminated children
                pid_t updated = waitpid(processes[i].pid, &exit_code, WUNTRACED);
                
                // update exit status after its done
                if (updated != processes[i].pid)
                {
                    continue;
                }
                if (WIFEXITED(exit_code))
                {
                    processes[i].exitCode = WEXITSTATUS(exit_code);
                    processes[i].status = 1;
                }
                else if (WIFSIGNALED(exit_code))
                {
                    processes[i].exitCode = WTERMSIG(exit_code);
                    processes[i].status = 1;
                }
            }
        }
        
    }
    // if the {PID} status is stopped
    // Print “[PID] resumed”
    // Use kill() to send SIGCONT to {PID} to get it continue and wait for it
    // After the process terminate, update status and exit code (call proc_update_status())
}

/*******************************************************************************
 * Program Execution
 ******************************************************************************/
static void open_writefd(int destination, const char *path)
{
    if (path == NULL)
    {
        fprintf(stderr, "Expected file for redirection\n");
        exit(EXIT_FAILURE);
    }

    int fd = openat(AT_FDCWD, path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1)
    {
        fprintf(stderr, "Cannot write to file %s\n", path);
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, destination) == -1)
    {
        fprintf(stderr, "dup2() failed to open file descriptor to another descriptor\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
}

static void command_exec(char **tokens, size_t size)
{

    /******* FILL IN THE CODE *******/
    // printf("I'm in command exec\n");

    // check if program exists and is executable : use access()
    if (access(tokens[0], X_OK) != 0)
    {
        fprintf(stderr, "%s not found\n", tokens[0]);
        return;
    }

    bool is_ampersand = false;
    if (strcmp(tokens[size - 2], "&") == 0)
    {
        is_ampersand = true;
        tokens[size - 2] = NULL;
    }
    bool is_input = false, is_output = false, is_error = false;
    char *input_file;
    char *output_file;
    char *error_file;
    for (size_t i = 0; i < size - 2; i++)
    {
        // printf("Token %zu : %s", i, tokens[i]);
        if (tokens[i] != NULL)
        {
            if (strcmp(tokens[i], "<") == 0)
            {
                is_input = true;
                input_file = tokens[i + 1];
                tokens[i] = NULL;
            }
            else if (strcmp(tokens[i], ">") == 0)
            {
                is_output = true;
                output_file = tokens[i + 1];
                tokens[i] = NULL;
            }
            else if (strcmp(tokens[i], "2>") == 0)
            {
                is_error = true;
                error_file = tokens[i + 1];
                tokens[i] = NULL;
            }
        }
    }
    // fork a subprocess and execute the program

    pid_t pid = fork();
    // printf("%d\n", pid);
    if (pid == 0)
    {
        // CHILD PROCESS

        // check file redirection operation is present : ex3
        if (is_input || is_output || is_error)
        {
            // int first_redirection = 0;
            if (is_input)
            {
                if (access(input_file, R_OK) != 0)
                {
                    fprintf(stderr, "%s does not exist\n", input_file);
                    exit(EXIT_FAILURE);
                }

                int fd = openat(AT_FDCWD, input_file, O_RDONLY);

                if (fd == -1)
                {
                    fprintf(stderr, "Cannot read from file: %s\n", input_file);
                    exit(EXIT_FAILURE);
                }

                if (dup2(fd, STDIN_FILENO) == -1)
                {
                    fprintf(stderr, "dup2() failed to open file descriptor to another descriptor\n");
                    exit(EXIT_FAILURE);
                }
                // first_redirection = input_i - 1;
                close(fd);
            }
            if (is_output)
            {
                open_writefd(STDOUT_FILENO, output_file);
                // if (first_redirection == 0)
                // {
                //     first_redirection = output_i - 1;
                // }
            }
            if (is_error)
            {
                open_writefd(STDERR_FILENO, error_file);
                // if (first_redirection == 0)
                // {
                //     first_redirection = output_i - 1;
                // }
            }
            // tokens[first_redirection] = NULL;
        }
        // if < or > or 2> present:
        // use fopen/open file to open the file for reading/writing with  permission O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_SYNC and 0644
        // use dup2 to redirect the stdin, stdout and stderr to the files
        // call execv() to execute the command in the child process

        // else : ex1, ex2
        // call execv() to execute the command in the child process
        char *command = tokens[0];

        if (execv(command, tokens) != 0)
        {
            perror("execv\n");
            exit(EXIT_FAILURE);
        }

        // Exit the child
    }
    else if (pid == -1)
    {
        fprintf(stderr, "Unable to create child process.\n");
        return;
    }
    else
    {
        // PARENT PROCESS
        // register the process in process table
        processes[num_processes].pid = pid;
        processes[num_processes].status = 2;
        processes[num_processes].exitCode = -1;

        // If  child process need to execute in the background  (if & is present at the end )
        int exit_code_status;
        if (is_ampersand)
        {
            // pid_t child_pid;
            // print Child [PID] in background
            printf("Child [%d] in background\n", pid);
        }
        else
        {
            // else wait for the child process to exit
            pid_t child_pid = waitpid(pid, &exit_code_status, WUNTRACED);

            if (processes[num_processes].status != 4) {                
                processes[num_processes].status = 1;
            }
            
            processes[num_processes].exitCode = exit_code_status;

            if (WIFEXITED(exit_code_status))
            {
                processes[num_processes].exitCode = WEXITSTATUS(exit_code_status);
                processes[num_processes].status = 1;
            }
            else if (WIFSIGNALED(exit_code_status))
            {
                processes[num_processes].exitCode = WTERMSIG(exit_code_status);
                processes[num_processes].status = 1;
            }
            // Use waitpid() with WNOHANG when not blocking during wait and  waitpid() with WUNTRACED when parent needs to block due to wait
        }
        num_processes++;
    }
}

/*******************************************************************************
 * Command Processor
 ******************************************************************************/

static void command(char **tokens, size_t size)
{

    /******* FILL IN THE CODE *******/

    // printf("YEET COMMAND. First token in command: %s\n", tokens[0]);
    // if command is "info" call command_info()             : ex1
    if (strcmp(tokens[0], "info") == 0)
    {
        if (size <= 2)
        {
            fprintf(stderr, "Wrong Command\n");
            return;
        }
        command_info(atoi(tokens[1]));
    }
    // if command is "wait" call command_wait()             : ex2
    else if (strcmp(tokens[0], "wait") == 0)
    {
        command_wait(atoi(tokens[1]));
    }
    // if command is "terminate" call command_terminate()   : ex2
    else if (strcmp(tokens[0], "terminate") == 0)
    {
        command_terminate(atoi(tokens[1]));
    }
    // if command is "fg" call command_fg()                 : ex4
    else if (strcmp(tokens[0], "fg") == 0) {
         command_fg(atoi(tokens[1]));
    }
    // call command_exec() for all other commands           : ex1, ex2, ex3
    else
    {
        command_exec(tokens, size);
    }
}

/*******************************************************************************
 * High-level Procedure
 ******************************************************************************/

void my_init(void)
{

    /******* FILL IN THE CODE *******/

    // anything else you require
    size_t size = sizeof(struct PCBTable *) * MAX_PROCESSES;
    processes = malloc(size);
    num_processes = 0;
    pointer = malloc(size + 1);
    pointer = processes;

   
    // use signal() with SIGTSTP to setup a signalhandler for ctrl+z : ex4
    signal(SIGTSTP, signal_handler);
    // use signal() with SIGINT to setup a signalhandler for ctrl+c  : ex4
    signal(SIGINT, signal_handler);
}

void my_process_command(size_t num_tokens, char **tokens)
{
    check_status();

    bool is_terminated = false;
    size_t curr_token_i = 0;

    while (!is_terminated)
    {
        char **curr_command = (char **)malloc(100 * sizeof(char *));
        size_t command_len = 0;
        int command_token_i = 0;

        while (curr_token_i < num_tokens)
        {
            // terminate when NULL is encountered
            char *curr_token = tokens[curr_token_i];
            curr_token_i++;
            // printf("%s, %d, %zu\n", curr_token, curr_token == NULL, curr_token_i);
            if ((curr_token == NULL) == 1)
            {
                curr_command[command_token_i] = curr_token;
                command_len++;
                command(curr_command, command_len);
                free(curr_command);
                is_terminated = true;
                break;
            }
            else if (strcmp(curr_token, ";") == 0)
            {
                // printf("I'm in ; ! Command len: %zu\n", command_len);
                curr_command[command_token_i] = NULL;
                command_len++;
                command(curr_command, command_len);
                curr_command = (char **)realloc(curr_command, 100 * sizeof(char *));
                command_len = 0;
                command_token_i = 0;
            }
            else
            {
                // dynamically reallocate memory for curr_command
                curr_command[command_token_i] = curr_token;
                command_token_i++;
                command_len++;
            }
        }
    }

    // for example :  /bin/ls ; /bin/sleep 5 ; /bin/pwd
    // split the above line as first command : /bin/ls , second command: /bin/pwd  and third command:  /bin/pwd
    // Call command() and pass each individual command as arguements
}

void my_quit(void)
{

    /******* FILL IN THE CODE *******/
    // Kill every process in the PCB that is either stopped or running
    for (int i = 0; i < num_processes; i++)
    {
        struct PCBTable process = processes[i];
        if (process.status == 2)
        {
            printf("Killing [%d]\n", process.pid);
            kill(process.pid, SIGTERM);
        }
    }
    free(processes);

    printf("\nGoodbye\n");
}
