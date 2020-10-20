
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "MyBash.h"
#include "Utilities.h"
#include "InternalCommands.h"
#include "ExternalCommands.h"


/* Global Variable : Use to store and execute commands */
CommandList myCommands;

// Function to execute command from terminal
int ExecuteMyShell(CommandList *cmdList)
{
    /*Error Handling*/
    if (cmdList == NULL)
    {
        PrintErrorMessage();
        return FAILURE;
    }

    int ret;
    /*If it's a single command, then first try to execute as Internal command*/
    if (cmdList->numCmds == 1)
    {
        /*Check for the valid commands */
        if (cmdList->pCommand[0].args[0] == NULL)
        {
            PrintErrorMessage();
            return 1;
        }

        /* Execute the internal command*/
        ret = ExecuteInternalCommands(&cmdList->pCommand[0]);
        if (ret != CMD_NOT_FOUND)
        {
            return ret;
        }
    }

    /*Seems the command is not implemented as an internal command, Try to execute as an External command */
    ret = ExecuteExternalCommands(cmdList);
    return ret;
}

/*
 * Function to get the current user name
 * */
void getUsername(char*username, int size) {
    struct passwd* pwd = getpwuid(getuid());
    strncpy(username, pwd->pw_name, size);
}


/*
 * Function to get the current host name
 * */
void getHostname(char *hostname, int size) {
    gethostname(hostname, size);
}

/*
 * Function to get the current working dir
 * */
int getCurWorkDir(char *curPath, int size) {
    curPath = getcwd(curPath, size);
    if (curPath == NULL)
        return FAILURE;
    else return SUCCESS;
}

/*
 * The function is used to print the shell prompt
 * */
void PrintPrompt()
{
    char userName[64] = {0};
    char hostname[64] = {0};
    char currentPath[64] = {0};

    getUsername(userName, 64); //Current user name
    getHostname(hostname, 64); // Host name
    getCurWorkDir(currentPath, 64); // Current Working  Dir

    printf("%s@%s:%s$ ", userName, hostname,currentPath);
    printf("%s>", SHELL_PROMPT);
}

/*
 * The function is invoked during the interactive mode.
 * */

void InteractiveMode()
{
    int isValidRead = 0;
    while (1)
    {
        PrintPrompt();
        memset(&myCommands,0x00, sizeof(myCommands));

        /*Get the user Input from the Stdin*/
        char *command = ReadCommandLine(&isValidRead, stdin);
        if (command == NULL || isValidRead == -1)
        {
            PrintErrorMessage();
            continue;
        }

        /*Parse the command into individual tokens*/
        char **cmdArgs = ParseCommand(command);
        if (*cmdArgs == NULL)
        {
            PrintErrorMessage();
            continue;
        }

        /*Split a long list of token into multiple parallel commands */
        SplitParallelCommands(cmdArgs, &myCommands);

        /*Structure the parallel commands into command structure */
        StructurePassedCommands(&myCommands);

        /*Execute the commands*/
        ExecuteMyShell(&myCommands);

        /*Free memory allocated during the process of parsing and execution */
        free(cmdArgs);
        FreeCommandList(&myCommands);
        free(command);
    }
}

void BatchMode(char *filename)
{
    printf("Batch mode started \n");
    FILE * fInput = fopen(filename, "r");
    int isValidRead = 0;

    if (fInput == NULL)
    {
        PrintErrorMessage();
        return;
    }
    while (isValidRead != -1)
    {
        memset(&myCommands,0x00, sizeof(myCommands));

        /*Get the user Input from the file*/
        char *command = ReadCommandLine(&isValidRead, fInput);
        if (command == NULL)
        {
            PrintErrorMessage();
            continue;
        }

        if (isValidRead == -1)
        {
            free(command);
            continue;
        }

        /*Parse the command into individual tokens*/
        char **cmdArgs = ParseCommand(command);
        if (*cmdArgs == NULL)
        {
            PrintErrorMessage();
            continue;
        }

        /*Split a long list of token into multiple parallel commands */
        SplitParallelCommands(cmdArgs, &myCommands);

        /*Structure the parallel commands into command structure */
        StructurePassedCommands(&myCommands);

        /*Execute the commands*/
        ExecuteMyShell(&myCommands);

        /*Free memory allocated during the process of parsing and execution */
        free(cmdArgs);
        FreeCommandList(&myCommands);
        free(command);

    }
    /*Close the file once we are done processing all the commands*/
    fclose(fInput);
}

int main(int argc, char **argv)
{
    char * defaultPath = "/bin";

    // Set SHELL environment variable:
    char shell_path[MAX_BUFFER]="shell=";
    strcat(shell_path, getenv("PWD"));
    strcat(shell_path,"/myshell");
    putenv(shell_path); //add the working directory  of myshell in the environment variables

    /*
     * Your initial shell path should contain one directory: `/bin'
     * */
    setenv("PATH",defaultPath,1);// add the current working directory  in the "PATH" environment variable to search for the filename specified.


    // Parsing commands Interactive mode or Script Mode
    if (argc == 1)
        InteractiveMode();
    else if (argc == 2)
        BatchMode(argv[1]);
    else
        PrintErrorMessage();

    // Exit the Shell
    return SUCCESS;
}
