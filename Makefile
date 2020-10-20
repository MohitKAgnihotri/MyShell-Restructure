#Joe Citizen, s1234567 - Operating Systems Project 1
# CompLab1/01 tutor: Fred Bloggs
myshell: helper.c  implementedcommands.c  myshell.c  systemcommands.c helper.h  implementedcommands.h  myshell.h  systemcommands.h 
	gcc -g3 -Werror -Wall -Wpedantic helper.c  implementedcommands.c  myshell.c  systemcommands.c -o myshell

clean:
	rm -rf *.o && rm -rf myshell
