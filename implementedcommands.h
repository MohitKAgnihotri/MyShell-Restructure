//
// Created by 310165137 on 04/10/2020.
//

#include "helper.h"
#ifndef MYSHELL_INTERNALCOMMANDS_H
#define MYSHELL_INTERNALCOMMANDS_H


typedef int (*funcPtr) (Command *cmd);
typedef struct
{
    char *cmd;
    funcPtr pfunc;
} implemented_command_list;


int CD (Command *cmd);
int CLR (Command *cmd);
int DIRECTORY (Command *cmd);
int ENVIRON (Command *cmd);
int ECHO (Command *cmd);
int HELP (Command *cmd);
int PAUSE (Command *cmd);
int QUIT (Command *cmd);
int PATH (Command *cmd);

int numImplementedCommands();
int ExecImplementedCmd(Command *cmd);



#endif //MYSHELL_INTERNALCOMMANDS_H
