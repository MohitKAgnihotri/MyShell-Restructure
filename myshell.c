
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "myshell.h"
#include "helper.h"
#include "implementedcommands.h"
#include "systemcommands.h"


/* Global Variable : Use to store and execute commands */
ParallelCommands commandsToBeExecuted;

void ParseExecuteCommand(char *command);

// Function to execute command from terminal
int LaunchShell(ParallelCommands *commandsToBeExecuted)
{
    /*Error Handling*/
    if (commandsToBeExecuted == NULL)
    {
        errorMessage();
        return FAILURE;
    }

    int returnVal;
    /*If it's a single command, then first try to execute as Internal command*/
    if (commandsToBeExecuted->numParallelCommands == 1)
    {
        /*Check for the valid commands */
        if (commandsToBeExecuted->pCommand[0].tokenizedCommands[0] == NULL)
        {
            errorMessage();
            return 1;
        }

        /* Execute the internal command*/
        returnVal = ExecImplementedCmd(&commandsToBeExecuted->pCommand[0]);
        if (returnVal != CMD_NOT_FOUND)
        {
            return returnVal;
        }
    }

    /*Seems the command is not implemented as an internal command, Try to execute as an External command */
    returnVal = ExecuteSystemCommands(commandsToBeExecuted);
    return returnVal;
}

/*
 * The function is used to print the shell prompt
 * */
void setPromptUser()
{
    char username[64] = {0};
    char hostname[64] = {0};
    char path[64] = {0};

    getUsername(username, 64); //Current user name
    getHostname(hostname, 64); // Host name
    getCurWorkDir(path, 64); // Current Working  Dir

    printf("%s@%s:%s$ ", username, hostname, path);
    printf("%s>", SHELL_PROMPT);
}

/*
 * The function is invoked during the interactive mode.
 * */

void InteractiveMode()
{
    int readValidLine = 0;
    while (1)
    {
        setPromptUser();
        memset(&commandsToBeExecuted, 0x00, sizeof(commandsToBeExecuted));

        /*Get the user Input from the Stdin*/
        char *commandInput = ReadCommandLine(&readValidLine, stdin);
        if (commandInput == NULL || readValidLine == -1)
        {
            errorMessage();
            continue;
        }

        ParseExecuteCommand(commandInput);
    }
}

void BatchMode(char *filename)
{
    printf("Batch mode started \n");
    FILE * commandfile = fopen(filename, "r");
    int readValidLine = 0;

    if (commandfile == NULL)
    {
        errorMessage();
        return;
    }
    while (readValidLine != -1)
    {
        memset(&commandsToBeExecuted, 0x00, sizeof(commandsToBeExecuted));

        /*Get the user Input from the file*/
        char *command = ReadCommandLine(&readValidLine, commandfile);
        if (command == NULL)
        {
            errorMessage();
            continue;
        }

        if (readValidLine == -1)
        {
            free(command);
            continue;
        }

        ParseExecuteCommand(command);
    }
    /*Close the file once we are done processing all the commands*/
    fclose(commandfile);
}

/*Parse the command into individual tokens*/
void ParseExecuteCommand(char *command)
{
    char **cmdArgs = ParseCommand(command);
    if (*cmdArgs == NULL)
    {
        errorMessage();
        free(command);
        return;
    }

    /*Split a long list of token into multiple parallel commands */
    CreateParallelCommands(cmdArgs, &commandsToBeExecuted);

    /*Structure the parallel commands into command structure */
    ExtractCommandInformation(&commandsToBeExecuted);

    /*Execute the commands*/
    LaunchShell(&commandsToBeExecuted);

    /*Free memory allocated during the process of parsing and execution */
    free(cmdArgs);
    FreeCommandList(&commandsToBeExecuted);
    free(command);
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
        errorMessage();

    // Exit the Shell
    return SUCCESS;
}
