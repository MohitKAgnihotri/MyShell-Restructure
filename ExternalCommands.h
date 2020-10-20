//
// Created by 310165137 on 04/10/2020.
//
#include "Utilities.h"
#ifndef MYSHELL_EXTERNALCOMMANDS_H
#define MYSHELL_EXTERNALCOMMANDS_H

int ExecuteExternalCommands(CommandList *cmdList);
int ExecuteSingleCommand(Command *cmd);
int ExecuteMultipleCommandWithoutWait(CommandList *cmdList);
int ExecuteMultipleCommandWithPipe(CommandList *cmdList);

#endif //MYSHELL_EXTERNALCOMMANDS_H
