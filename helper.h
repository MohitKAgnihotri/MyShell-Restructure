//
// Created by 310165137 on 03/10/2020.
//

#ifndef MYSHELL_UTILITIES_H
#define MYSHELL_UTILITIES_H

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARG_LENGTH 64
#define MAX_NUM_OF_ARGUMENTS 64
#define MAX_NUM_PARALLEL_COMMANDS 10


typedef struct Command
{
    char** tokenizedCommands;
    char* redirectedInput;
    char* redirectedOutput;
    int isInputRedirected;
    int isOutputRedirected;
    int isOutputTruncated;
    int isExecuteInBackgrnd;
} Command;

typedef struct CommandList
{
    int numParallelCommands;
    int isCommandsPiped;
    Command pCommand[MAX_NUM_PARALLEL_COMMANDS];
}ParallelCommands;



char *ReadCommandLine( int *isvalid, FILE* inputStream );
char** ParseCommand(char *line);
void ExtractCommandInformation(ParallelCommands *cmdList);
void derivefullpath(char *fullpath, const char *shortpath);
void CreateParallelCommands(char**args, ParallelCommands *cmdList);
void errorMessage(void);
void FreeCommandList(ParallelCommands *cmdList);

void getUsername(char*username, int size);
void getHostname(char *hostname, int size);
int getCurWorkDir(char *path, int size);

#endif //MYSHELL_UTILITIES_H
