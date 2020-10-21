//
// Created by 310165137 on 03/10/2020.
//

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include "helper.h"
#include "myshell.h"


// Function to read a line from command into the buffer
char *ReadCommandLine( int *isvalid, FILE* inputStream )
{
    // Dynamically Allocate Buffer
    char *commandline = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    memset(commandline,  0x00, sizeof(char) * MAX_COMMAND_LENGTH);

    /*Set the possible size to max command length*/
    size_t len = MAX_COMMAND_LENGTH;

    /*Read teh command from the input stream*/
    *isvalid= getline(&commandline, &len, inputStream);

    if( commandline[ strlen(commandline) - 1] == '\n' )
        commandline[ strlen(commandline) - 1] = '\0';

    return commandline;
}

/*
 * This function process the input commandline and splits into the multiple commands
 * Example Input: "ls", "-lrt", "&", "cat", "test.txt", "&" wc -l"
 * Example output:
 *  CMD1: ls -lrt
 *  CMD2: cat test.txt
 *  CMD3: wc -l
 * */
void CreateParallelCommands(char**args, ParallelCommands *cmdList)
{
    if (args == NULL || *args == NULL || cmdList == NULL)
    {
        errorMessage();
        return;
    }

    int index;
    int cmdCnt = 0u;
    int argumentCnt = 0u;

    /*Iterate over all the arguments */
    for (index = 0; index < MAX_NUM_OF_ARGUMENTS && args[index] != NULL; index++)
    {
        /*Start processing of the first command*/
        if (cmdList->pCommand[cmdCnt].tokenizedCommands == NULL)
        {
            cmdList->pCommand[cmdCnt].tokenizedCommands = (char **)malloc(sizeof(char *) * MAX_NUM_OF_ARGUMENTS);
            memset(cmdList->pCommand[cmdCnt].tokenizedCommands, 0x00, sizeof(char *) * MAX_NUM_OF_ARGUMENTS);
            argumentCnt = 0;
        }
        /* Check if the end of the command is not reached */
        if ((strcmp(args[index], "&") != 0 &&  strcmp(args[index], "|") != 0) || (strcmp(args[index], "&") == 0 &&  args[index+1] == NULL))
        {
            cmdList->pCommand[cmdCnt].tokenizedCommands[argumentCnt] = args[index];
            argumentCnt++;
        }
        else /*Command has ended. Need to Update the command count and also check if the commands are piped. */
        {
            cmdList->pCommand[cmdCnt].tokenizedCommands[argumentCnt] = NULL;
            if (strcmp(args[index], "|") == 0)
            {
                cmdList->isCommandsPiped = 1;
            }
            cmdCnt++;
        }
    }
    /* Finally update the data structure  with the number of processed commands*/
    cmdList->numParallelCommands = cmdCnt + 1;
}

/* This function takes in the commandline read from a stream, and then tokenizes it.
 * Example Input: "ls -lrt & cat test.txt & wc -l"
 * Example Output:"ls", "-lrt", "&", "cat", "test.txt", "&" wc -l"
 * */
char** ParseCommand(char *line)
{
    char **argumentList = (char **)malloc(sizeof(char *) * MAX_NUM_OF_ARGUMENTS);
    memset(argumentList,0x00, sizeof(char *) * MAX_NUM_OF_ARGUMENTS);
    if (argumentList == NULL)
    {
        errorMessage();
        return NULL;
    }

    char *arg;
    char delim[10] = " \t\n\r\a";
    int pos = 0;

    /*Derive the tokes based on the delimiters*/
    arg = strtok(line, delim);
    while (arg != NULL && pos < MAX_NUM_OF_ARGUMENTS)
    {
        argumentList[pos] = arg;
        pos ++;
        arg = strtok(NULL, delim);
    }
    /*Mark the end of the list*/
    argumentList[pos] = NULL;
    return argumentList;
}


/*
 * This function parses individual command and extracts information required for the execution of the commands
 * */

void ExtractCommandInformation(ParallelCommands *cmdList)
{
    if (cmdList == NULL)
    {
        errorMessage();
        return;
    }

    /*Iterate through all the commands*/
    for (int j = 0; j < MAX_NUM_PARALLEL_COMMANDS; j++)
    {
        Command *cmd = &cmdList->pCommand[j];
        if (cmd->tokenizedCommands == NULL )
        {
            return;
        }

        /*Verify the correctness of the command
         * command > ; or command >> ; or command < ; or command | ; are few example of malformed commands*/
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if (strcmp(cmd->tokenizedCommands[i], "<") == 0 || strcmp(cmd->tokenizedCommands[i], ">") == 0 \
 || strcmp(cmd->tokenizedCommands[i], ">>") == 0 || strcmp(cmd->tokenizedCommands[i], "|") == 0)
            {
                if ((cmd->tokenizedCommands[i + 1] == NULL) || (strcmp(cmd->tokenizedCommands[i + 1], "<") == 0 || strcmp(cmd->tokenizedCommands[i + 1], ">") == 0 \
 || strcmp(cmd->tokenizedCommands[i + 1], ">>") == 0 || strcmp(cmd->tokenizedCommands[i + 1], "|") == 0))
                {
                    errorMessage();
                    return;
                }
            }
        }

        /*Get the filename if the input is redirected */
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if (strcmp(cmd->tokenizedCommands[i], "<") == 0 && cmd->tokenizedCommands[i + 1] != NULL )
            {
                cmd->redirectedInput = cmd->tokenizedCommands[i + 1];
                cmd->isInputRedirected = 1;
            }
        }

        /*Get the filename if the output is redirected */
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if (((strcmp(cmd->tokenizedCommands[i], ">") == 0) || (strcmp(cmd->tokenizedCommands[i], ">>") == 0)) && (cmd->tokenizedCommands[i + 1] != NULL ))
            {
                cmd->redirectedOutput = cmd->tokenizedCommands[i + 1];
                cmd->isOutputRedirected = 1;
                if (strcmp(cmd->tokenizedCommands[i], ">>") == 0)
                {
                    /*See if the output file must be truncated or not*/
                    cmd->isOutputTruncated = 1;
                }
            }
        }

        //Check if the pipe is used
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if (((strcmp(cmd->tokenizedCommands[i], "|") == 0)) && (cmd->tokenizedCommands[i + 1] != NULL ))
            {
                cmdList->isCommandsPiped = 1;
            }
        }

        //Check if the command is to be run in background
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if (strcmp(cmd->tokenizedCommands[i], "&") == 0)
            {
                cmd->isExecuteInBackgrnd = 1;
            }
        }

        /*Reset the extra arguments once parsing is comlete as the infirmation is populated
         * in the command structure. */
        for (int i = 0; i < MAX_NUM_OF_ARGUMENTS && cmd->tokenizedCommands[i] != NULL; i++)
        {
            if(strcmp(cmd->tokenizedCommands[i], "<") == 0 || strcmp(cmd->tokenizedCommands[i], ">") == 0 \
 || strcmp(cmd->tokenizedCommands[i], ">>") == 0 || strcmp(cmd->tokenizedCommands[i], "|") == 0 || strcmp(cmd->tokenizedCommands[i], "&") == 0)
            {
                for (int k = i; k < MAX_NUM_OF_ARGUMENTS; k++)
                {
                    cmd->tokenizedCommands[k] = NULL;
                }
                break;
            }
        }
    }
}

void derivefullpath(char *fullpath, const char *shortpath)
{
    int i,j;
    i=j=0;
    fullpath[0]=0;
    char *old_dir, *current_dir;

    if(shortpath[0]=='~')// e.g.  ~/os
    {
        strcpy(fullpath, getenv("HOME"));
        j=strlen(fullpath);
        i=1;
    }

    else  if(shortpath[0]=='.'&&shortpath[1]=='.')// e.g.  ../os
    {
        old_dir=getenv("PWD");
        chdir("..");
        current_dir=(char *)malloc(MAX_BUFFER);//allocate memory space
        if(!current_dir)        //if allocate fails
            exit(1);


        getcwd(current_dir,MAX_BUFFER);  //get current working directory
        strcpy(fullpath, current_dir);
        j=strlen(fullpath);
        i=2;
        chdir(old_dir);
    }
    else   if(shortpath[0]=='.')// e.g.
    {
        strcpy(fullpath, getenv("PWD"));
        j=strlen(fullpath);
        i=1;
    }
    else if(shortpath[0]!='/')// e.g.
    {
        strcpy(fullpath, getenv("PWD"));
        strcat(fullpath, "/");
        j=strlen(fullpath);
        i=0;
    }
    strcat(fullpath+j,shortpath+i);
}

void errorMessage(void)
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void FreeCommandList(ParallelCommands *cmdList)
{
    for (int i = 0; i < MAX_NUM_PARALLEL_COMMANDS; i++)
    {
        Command *cmd = &cmdList->pCommand[i];
        if (cmd)
        {
            free(cmd->tokenizedCommands);
        }
    }
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
int getCurWorkDir(char *path, int size) {
    path = getcwd(path, size);
    if (path == NULL)
        return FAILURE;
    else return SUCCESS;
}