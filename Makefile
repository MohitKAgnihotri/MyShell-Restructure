#Joe Citizen, s1234567 - Operating Systems Project 1
# CompLab1/01 tutor: Fred Bloggs
myshell: ExternalCommands.c  InternalCommands.c  MyBash.c  Utilities.c ExternalCommands.h  InternalCommands.h  MyBash.h  Utilities.h
	gcc -g3 -Werror -Wall -Wpedantic ExternalCommands.c  InternalCommands.c  MyBash.c  Utilities.c -o myshell

clean:
	rm -rf *.o && rm -rf myshell
