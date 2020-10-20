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
    char** args;
    char* inFileName;
    char* outFileName;
    int isInputRedirected;
    int isOutputRedirected;
    int isOutputTruncated;
    int isExecuteInBackgrnd;
} Command;

typedef struct CommandList
{
    int numCmds;
    int isCommandsPiped;
    Command pCommand[MAX_NUM_PARALLEL_COMMANDS];
}ParallelCommands;



char *ReadCommandLine( int *isvalid, FILE* inputStream );
char** ParseCommand(char *line);
void ExtractCommandInformation(ParallelCommands *cmdList);
void get_fullpath(char *fullpath, const char *shortpath);
void CreateParallelCommands(char**args, ParallelCommands *cmdList);
void PrintErrorMessage(void);
void FreeCommandList(ParallelCommands *cmdList);

#endif //MYSHELL_UTILITIES_H
