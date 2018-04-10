
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

int mykill(char *arg1, char *arg2) {
    pid_t pid = atoi(arg1);
    int signal = atoi(arg2);
    //printf("%d \"%d\"\n", pid, signal);
    kill(pid, signal);
    return 0;
}