#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include "myshell.h"
#include "implementedcommands.h"


// Definitions
static const implemented_command_list implemented_commands[] =
        {
                { "cd",      CD },
                { "clr",     CLR },
                { "dir",     DIRECTORY },
                { "environ", ENVIRON },
                { "echo",    ECHO },
                { "help",    HELP },
                { "pause",   PAUSE },
                { "exit",    QUIT },
                {"path",     PATH},
                { 0,         0 }
        };


int ArgsCount(char *const *args);

int numImplementedCommands()
{
    return sizeof(implemented_commands) / sizeof(implemented_commands[0]);
}

int ExecImplementedCmd(Command *cmd)
{
    // Loop to check for builtin functions
    for (int i=0; ((i < numImplementedCommands()) && (implemented_commands[i].cmd != NULL)); i++) // numBuiltin() returns the number of builtin functions
    {
        if(strcmp(cmd->tokenizedCommands[0], implemented_commands[i].cmd) == 0) // Check if user function matches builtin function name
            return (implemented_commands[i].handler)(cmd); // Call respective builtin function with arguments
    }
    return CMD_NOT_FOUND;
}

int ArgsCount(char *const *args)
{
    int num_of_args;
    for (num_of_args = 0; ((num_of_args < MAX_NUM_OF_ARGUMENTS) && (args[num_of_args+1] != NULL)); num_of_args++)
    {

    }
    return num_of_args;
}

int CD (Command *cmd)
{
    int numArgs = ArgsCount(cmd->tokenizedCommands);

    if (numArgs == 0)
    {
        PrintErrorMessage();
        return TOO_FEW_PARAMETERS;
    }

    if (numArgs > 1)
    {
        PrintErrorMessage();
        return TOO_MANY_PARAMETERS;
    }

    int ret = chdir(cmd->tokenizedCommands[numArgs]);
    if (ret)
    {
        PrintErrorMessage();
        return FAILURE;
    }
    return SUCCESS;
}

int CLR (Command *cmd)
{
    pid_t pid;
    pid = fork();
    if (pid == 0)
    {
        execvp("clear", cmd->tokenizedCommands);
        PrintErrorMessage();
    }
    sleep(1);
    return EXIT_SUCCESS;
}

int DIRECTORY (Command *cmd)
{
    FILE * fp=NULL;
    int numArgs = ArgsCount(cmd->tokenizedCommands);

    struct dirent *parentDir;  // Pointer for directory entry

    if (cmd->isOutputRedirected)
    {
        char filePath[100] = {0};
        derivefullpath(filePath, cmd->redirectedOutput);
        fp=freopen(filePath, cmd->isOutputTruncated == 1 ? "w" : "a", stdout);
    }

    // opendir() returns a pointer of DIR type.
    DIR *dir;
    if (numArgs == 0)
    {
        dir = opendir(".");
    }
    else
    {
        dir = opendir(cmd->tokenizedCommands[numArgs]);
    }

    if (dir == NULL)  // opendir returns NULL if couldn't open directory
    {
        PrintErrorMessage();
        return FAILURE;
    }

    while ((parentDir = readdir(dir)) != NULL)
        printf("%s\n", parentDir->d_name);

    closedir(dir);

    if(fp)
    {
        fclose(fp);
        freopen("/dev/tty","w",stdout);
    }

    return SUCCESS;
}

int ENVIRON (Command *cmd)
{
    FILE * fp=NULL;
    int i = 0;

    if (cmd->isOutputRedirected)
    {
        char filePath[100] = {0};
        derivefullpath(filePath, cmd->redirectedOutput);
        fp=freopen(filePath, cmd->isOutputTruncated == 1 ? "w" : "a", stdout);
    }

    while(__environ[i]) {
        printf("%s\n", __environ[i++]); // prints in form of "variable=value"
    }

    if(fp)
    {
        fclose(fp);
        freopen("/dev/tty","w",stdout);
    }
    return SUCCESS;
}
int ECHO (Command *cmd)
{
    int numArgs;
    FILE * fp=NULL;

    if (cmd->isOutputRedirected)
    {
        char filepath[100] = {0};
        derivefullpath(filepath, cmd->redirectedOutput);
        fp=freopen(filepath, cmd->isOutputTruncated == 1 ? "w" : "a", stdout);
    }

    for (numArgs = 1; ((numArgs < MAX_NUM_OF_ARGUMENTS) && (cmd->tokenizedCommands[numArgs] != NULL)); numArgs++)
    {
        printf("%s \t",cmd->tokenizedCommands[numArgs]);
    }
    printf("\n");

    if(fp)
    {
        fclose(fp);
        freopen("/dev/tty","w",stdout);
    }
    return FAILURE;
}
int HELP (Command *cmd)
{
    FILE * fp=NULL;
    if (cmd->isOutputRedirected)
    {
        char filePath[100] = {0};
        derivefullpath(filePath, cmd->redirectedOutput);
        fp=freopen(filePath, cmd->isOutputTruncated == 1 ? "w" : "a", stdout);
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
    if(fp)
    {
        fclose(fp);
        freopen("/dev/tty","w",stdout);
    }

    return EXIT_SUCCESS;
}
int PAUSE (Command *cmd)
{
    getpass("Paused \n <ENTER> key to continue");
    return EXIT_SUCCESS;
}

int QUIT (Command *cmd)
{
    exit(0);
}

int PATH (Command *cmd)
{
    char modifiedPath[MAX_NUM_OF_ARGUMENTS * MAX_ARG_LENGTH] = {0};
    int num_of_args = ArgsCount(cmd->tokenizedCommands);
    while (num_of_args > 0)
    {
        strcat(modifiedPath, cmd->tokenizedCommands[num_of_args]);
        strcat(modifiedPath, ":");
        num_of_args--;
    }
    setenv("PATH", modifiedPath, 1);// add the current working directory  in the "PATH" environment variable to search for the filename specified.
    return EXIT_SUCCESS;
}
