//
// Created by 310165137 on 04/10/2020.
//
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include "myshell.h"
#include "implementedcommands.h"


// Definitions
static const Command_t internal_command[] =
        {
                { "cd",         Internal_Handler_CD },
                { "clr",        Internal_Handler_CLR },
                { "dir",        Internal_Handler_DIR },
                { "environ",    Internal_Handler_ENVIORN },
                { "echo",       Internal_Handler_ECHO },
                { "help",       Internal_Handler_HELP },
                { "pause",      Internal_Handler_PAUSE },
                { "exit",       Internal_Handler_QUIT },
                {"path",        Internal_Handler_PATH},
                { 0, 0 }
        };


int GetArgsCount(char *const *args);

int numInternalCommandsSupported()
{
    return sizeof(internal_command)/sizeof(internal_command[0]);
}

int ExecuteInternalCommands(Command *cmd)
{
    // Loop to check for builtin functions
    for (int i=0; ((i< numInternalCommandsSupported()) &&  (internal_command[i].cmd != NULL)); i++) // numBuiltin() returns the number of builtin functions
    {
        if(strcmp(cmd->args[0], internal_command[i].cmd)==0) // Check if user function matches builtin function name
            return (internal_command[i].handler)(cmd); // Call respective builtin function with arguments
    }
    return CMD_NOT_FOUND;
}

int GetArgsCount(char *const *args)
{
    int num_of_args;
    for (num_of_args = 0; ((num_of_args < MAX_NUM_OF_ARGUMENTS) && (args[num_of_args+1] != NULL)); num_of_args++)
    {

    }
    return num_of_args;
}

int Internal_Handler_CD (Command *cmd)
{
    int num_of_args = GetArgsCount(cmd->args);

    if (num_of_args == 0)
    {
        PrintErrorMessage();
        return TOO_FEW_PARAMETERS;
    }

    if (num_of_args  > 1)
    {
        PrintErrorMessage();
        return TOO_MANY_PARAMETERS;
    }

    int ret = chdir(cmd->args[num_of_args]);
    if (ret)
    {
        PrintErrorMessage();
        return FAILURE;
    }
    return SUCCESS;
}

int Internal_Handler_CLR (Command *cmd)
{
    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        execvp("clear", cmd->args);
        PrintErrorMessage();
    }
    sleep(1);
    return EXIT_SUCCESS;
    //Note: Can't use System
}

int Internal_Handler_DIR (Command *cmd)
{
    FILE * outFileFp=NULL;
    int num_of_args = GetArgsCount(cmd->args);

    struct dirent *de;  // Pointer for directory entry

    if (cmd->isOutputRedirected)
    {
        char max_file_path[100] = {0};
        get_fullpath(max_file_path, cmd->outFileName);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    // opendir() returns a pointer of DIR type.
    DIR *dr;
    if (num_of_args == 0)
    {
        dr = opendir(".");
    }
    else
    {
        dr = opendir(cmd->args[num_of_args]);
    }

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        PrintErrorMessage();
        return FAILURE;
    }

    while ((de = readdir(dr)) != NULL)
        printf("%s\n", de->d_name);

    closedir(dr);

    if(outFileFp)
    {
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }

    return SUCCESS;
}

int Internal_Handler_ENVIORN (Command *cmd)
{
    FILE * outFileFp=NULL;
    int i = 0;

    if (cmd->isOutputRedirected)
    {
        char max_file_path[100] = {0};
        get_fullpath(max_file_path, cmd->outFileName);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    while(__environ[i]) {
        printf("%s\n", __environ[i++]); // prints in form of "variable=value"
    }

    if(outFileFp)
    {
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }
    return SUCCESS;
}
int Internal_Handler_ECHO (Command *cmd)
{
    int num_of_args;
    FILE * outFileFp=NULL;

    if (cmd->isOutputRedirected)
    {
        char max_file_path[100] = {0};
        get_fullpath(max_file_path, cmd->outFileName);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    for (num_of_args = 1; ((num_of_args < MAX_NUM_OF_ARGUMENTS) && (cmd->args[num_of_args] != NULL)); num_of_args++)
    {
        printf("%s \t",cmd->args[num_of_args]);
    }
    printf("\n");

    if(outFileFp)
    {
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }
    return FAILURE;
}
int Internal_Handler_HELP (Command *cmd)
{
    FILE * outFileFp=NULL;
    if (cmd->isOutputRedirected)
    {
        char max_file_path[100] = {0};
        get_fullpath(max_file_path, cmd->outFileName);
        outFileFp=freopen(max_file_path, cmd->isOutputTruncated ==  1 ? "w": "a",stdout);
    }

    if (fork() == 0)
    {
        char * const help[] = { "more", "readme_doc", NULL };
        if (execvp(help[0], help) == -1)
        {
            PrintErrorMessage();
            exit(EXIT_FAILURE);
        }
    }
    if(outFileFp)
    {
        fclose(outFileFp);
        freopen("/dev/tty","w",stdout);
    }

    return EXIT_SUCCESS;
}
int Internal_Handler_PAUSE (Command *cmd)
{
    getpass("Paused \n <ENTER> key to continue");
    return EXIT_SUCCESS;
}

int Internal_Handler_QUIT (Command *cmd)
{
    exit(0);
}

int Internal_Handler_PATH (Command *cmd)
{
    char newpath[MAX_NUM_OF_ARGUMENTS * MAX_ARG_LENGTH] = {0};
    int num_of_args = GetArgsCount(cmd->args);
    while (num_of_args > 0)
    {
        strcat(newpath,cmd->args[num_of_args]);
        strcat(newpath,":");
        num_of_args--;
    }
    setenv("PATH",newpath,1);// add the current working directory  in the "PATH" environment variable to search for the filename specified.
    return EXIT_SUCCESS;
}
