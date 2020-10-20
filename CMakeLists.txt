cmake_minimum_required(VERSION 3.16)
project(Ariel C)

set(CMAKE_C_STANDARD 99)

add_executable(myshell helper.c  implementedcommands.c  myshell.c  systemcommands.c  helper.h  implementedcommands.h  myshell.h  systemcommands.h)

target_link_libraries(myshell)