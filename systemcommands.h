//
// Created by 310165137 on 04/10/2020.
//
#include "helper.h"
#ifndef MYSHELL_EXTERNALCOMMANDS_H
#define MYSHELL_EXTERNALCOMMANDS_H

int ExecuteExternalCommands(ParallelCommands *cmdList);
int ExecuteSingleCommand(Command *cmd);
int ExecuteMultipleCommandWithoutWait(ParallelCommands *cmdList);
int ExecuteMultipleCommandWithPipe(ParallelCommands *cmdList);

#endif //MYSHELL_EXTERNALCOMMANDS_H
