//
// Created by 310165137 on 04/10/2020.
//

#include "helper.h"
#ifndef MYSHELL_INTERNALCOMMANDS_H
#define MYSHELL_INTERNALCOMMANDS_H


typedef int (*Handler_t) (Command *cmd);
typedef struct
{
    char *cmd;
    Handler_t handler;
} Command_t;


int Internal_Handler_CD (Command *cmd);
int Internal_Handler_CLR (Command *cmd);
int Internal_Handler_DIR (Command *cmd);
int Internal_Handler_ENVIORN (Command *cmd);
int Internal_Handler_ECHO (Command *cmd);
int Internal_Handler_HELP (Command *cmd);
int Internal_Handler_PAUSE (Command *cmd);
int Internal_Handler_QUIT (Command *cmd);
int Internal_Handler_PATH (Command *cmd);

int numInternalCommandsSupported();
int ExecuteInternalCommands(Command *cmd);



#endif //MYSHELL_INTERNALCOMMANDS_H
