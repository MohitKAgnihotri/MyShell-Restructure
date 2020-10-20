cmake_minimum_required(VERSION 3.16)
project(Ariel C)

set(CMAKE_C_STANDARD 99)

add_executable(myshell MyBash.c ExternalCommands.c InternalCommands.c Utilities.c MyBash.h ExternalCommands.h InternalCommands.h Utilities.h)

target_link_libraries(myshell)