//
// Created by 310165137 on 04/10/2020.
//


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "systemcommands.h"


/**
 * This function start to process the external commands based on the number of commands
 * and the related flags.
 * */

int ExecuteSystemCommands(ParallelCommands *cmdList)
{
    if (cmdList->numCmds == 1)
    {
        /* Execute a single external command */
        ExecuteSingleSystemCommand(&cmdList->pCommand[0]);
    }
    else if (cmdList->numCmds > 1 && cmdList->isCommandsPiped != 1)
    {
        /* When we have multiple commands, try to execute each using the ExecuteSingleSystemCommand*/
        for (int i = 0; i < cmdList->numCmds; i++)
            ExecuteSingleSystemCommand(&cmdList->pCommand[i]);
    }
    else
    {
        /* If the commands are piped, invoke the commands with the piped function*/
        ExecuteMultipleCommandWithPipe(cmdList);
    }
    return EXIT_SUCCESS;
}

/** This function executes a single command.
 * The passed structure has information about the arguments
 * Based on the flags; either the input or output or both are redirected
 * Also, based on the flags, either the task is executes in the background or foreground.
 * */
int ExecuteSingleSystemCommand(Command *cmd)
{
    pid_t pid;
    int status;
    pid = fork();

    char max_file_path[100] = {0};
    char parent[100] = {0};
    strcpy(parent,"parent=") ;
    strcat(parent, getenv("shell"));
    FILE * outFileFp=NULL,* inFileFp;
    /*Check if the output is redirected, and if yes, use the right mode to open the file
     * If the file is truncated, open it for writing else open for append */
    if (cmd->isOutputRedirected)
    {
        /*Find the full path name for the file where output is redirected*/
        derivefullpath(max_file_path, cmd->outFileName);
        /* Reopen the stdout as a filename */
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    if (pid == 0)
    {
        // The Child Process
        // Set the enviornment for the child process
        putenv( parent);
        printf("%s, %s", cmd->args[0], cmd->args[1]);

        /*Check if  the inout is reqirected, if yes, open the stdin as filename*/
        if (cmd->isInputRedirected)
        {
            derivefullpath(max_file_path, cmd->inFileName);
            inFileFp= freopen(max_file_path,"r",stdin); // open file
        }
        /*Finally execute  the command using the arguments using the args*/
        if (execvp(cmd->args[0], cmd->args) == -1)
        {
            PrintErrorMessage();
            exit(EXIT_FAILURE);
        }

        /*Finally close the input file*/
        if (inFileFp)
            fclose(inFileFp);

    }
    else if (pid < 0)
    {
        //Forking Error
        PrintErrorMessage();
    }
    else
    {
        // The Parent Process
        if (!cmd->isExecuteInBackgrnd)
        {
            do
            {
                (void)waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        /*If the output was re-directed, re-direct the stdout to /dev/tty*/
        if(outFileFp)
        {
            fclose(outFileFp);
            freopen("/dev/tty","w",stdout);
        }
    }
    return 1;
}

/*
 * This function is used to execute multiple commands which are piped together.
 * */
int ExecuteMultipleCommandWithPipe(ParallelCommands *cmdList)
{
        size_t i, n;
        int prev_pipe, pfds[2];
        n = cmdList->numCmds;
        prev_pipe = STDIN_FILENO;

        for (i = 0; i < n - 1; i++)
        {
            pipe(pfds);
            if (fork() == 0)
            {
                // Redirect previous pipe to stdin
                if (prev_pipe != STDIN_FILENO) {
                    dup2(prev_pipe, STDIN_FILENO);
                    close(prev_pipe);
                }

                // Redirect stdout to current pipe
                dup2(pfds[1], STDOUT_FILENO);
                close(pfds[1]);

                // Start command
                execvp(cmdList->pCommand[i].args[0], cmdList->pCommand[i].args);
                PrintErrorMessage();
                exit(1);
            }

            // Close read end of previous pipe (not needed in the parent)
            close(prev_pipe);
            // Close write end of current pipe (not needed in the parent)
            close(pfds[1]);
            // Save read end of current pipe to use in next iteration
            prev_pipe = pfds[0];
        }

        // Get stdin from last pipe
        if (prev_pipe != STDIN_FILENO) {
            dup2(prev_pipe, STDIN_FILENO);
            close(prev_pipe);
        }

        // Start last command
        execvp(cmdList->pCommand[i].args[0], cmdList->pCommand[i].args);
        PrintErrorMessage();
        exit(1);
}
