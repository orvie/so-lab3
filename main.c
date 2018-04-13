
/* 
 * File:   main.c
 * Author: orvie
 *
 * Created on April 8, 2018, 10:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Para getcwd
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>

#include "parser.h"



//Declarations section
#define TAM 100
#define BUF_SIZE 128
#define FLAG_EXIT -1
#define FLAG_STAY 0
#define MY_PWD "mypwd"
#define MY_EXIT "myexit"
#define MY_PS_INFO "psinfo"
#define MY_CP "mycp"
#define MY_KILL "mykill"
#define MY_ECHO "myecho"
#define MY_CLEAR "myclear"
#define MY_PAUSE "mypause"
#define MY_PS "myps"
#define MY_GREP "mygrep"
#define MY_TIME "mytime"


void processMyExit(int *flag);
void processMyPWD(int background, int *flag, pid_t *pid);
void processMyCp(int background, int *flag, pid_t *pid, char *origin, char *des);
void processMyKill(int background, int *flag, int signal, char *pids[], int initial, int count, pid_t *pid);
void myKill(pid_t pid, int signal);
void processMyGrep(int background, int *flag, pid_t *pid, char **items);
void processMyEcho(int background, int *flag, pid_t *pid, int num, char **items);
void myTime();
void copy_file(char *f_org, char *f_dest);

/*
 * 
 */
int main() {

    char ** items;
    int i, num, background;
    char expresion[TAM];
    //Variables for pipes
    char buffer[BUF_SIZE];

    pid_t pid;
    int flag;
    flag = FLAG_STAY;

    do {

        //Impresion del prompt en la cosola standard
        printf("%s@my-cli:$ ", getenv("USER"));
        //Esperamos las peticiones del usuario.
        fgets(expresion, TAM, stdin);
        //Procesamos la peticion
        num = separaItems(expresion, &items, &background);

        //Se valida que al menos ingreso un comando
        if (num > 0) {
            if (strcmp(items[0], MY_PWD) == 0 && num == 1) {
                processMyPWD(background, &flag, &pid);
            } else if (strcmp(items[0], MY_EXIT) == 0) {
                processMyExit(&flag);
            } else if (strcmp(items[0], MY_PS_INFO) == 0) {
                execv("./bin/psinfo", items);
            } else if (strcmp(items[0], MY_CP) == 0) {
                //Validamos que venga el origen y destino
                if (num > 2) {
                    processMyCp(background, &flag, &pid, items[1], items[2]);
                } else {
                    printf("%s", "Invalid arguments. <orgin> <dest>\n");
                }

            }//
            else if (strcmp(items[0], MY_KILL) == 0) {

                if (num > 2) {
                    processMyKill(background, &flag, atoi(items[1]), items, 2, num, &pid);
                } else {
                    printf("%s", "Invalid arguments. <signal> <pid...>\n");
                }

            } else if (strcmp(items[0], MY_ECHO) == 0) {
                processMyEcho(background, &flag, &pid, num, items);
            } else if (strcmp(items[0], "myclr") == 0) {
                system("clear");
                //printf("\x1b[s\x1b[2J\x1b[10;25H\x1b[u");
                //printf("\033[H\033[J");
            } else if (strcmp(items[0], MY_PAUSE) == 0) {
                while (getchar() != '\n');
            } else if (strcmp(items[0], MY_PS) == 0) {
                pid = fork();
                if (pid == 0) {
                    char *comando[num + 1];
                    comando[0] = "/bin/ps";
                    if (num > 1) {
                        int i;
                        for (i = 1; i < num; i++) {
                            comando[i] = items[i];
                        }
                    }
                    comando[num] = NULL;
                    execv("/bin/ps", comando);
                    flag = FLAG_EXIT;
                } else if (pid != -1) {
                    if (background == 1) {
                        printf("[%d]\n", pid);
                    } else {
                        wait(NULL);
                    }
                }

            } else if (strcmp(items[0], MY_GREP) == 0) {
                pid = fork();
                if (pid == 0) {
                    char *comando[num + 1];
                    comando[0] = "/bin/grep";
                    if (num > 1) {
                        int i;
                        for (i = 1; i < num; i++) {
                            comando[i] = items[i];
                        }
                    }
                    comando[num] = NULL;
                    execv("/bin/grep", comando);
                    flag = FLAG_EXIT;
                } else if (pid != -1) {
                    if (background == 1) {
                        printf("[%d]\n", pid);
                    } else {
                        wait(NULL);
                    }
                }
            } else if (strcmp(items[0], MY_TIME) == 0) {

                pid = fork();
                if (pid == 0) {
                    myTime();
                    flag = FLAG_EXIT;
                } else if (pid != -1) {
                    if (background == 1) {
                        printf("[%d]\n", pid);
                    } else {
                        waitpid(pid, NULL, 0);

                    }
                }

            }
        }


    } while (flag != -1);

    return EXIT_SUCCESS;
}

void processMyExit(int *flag) {
    char input[5];
    printf("%s", "Are you sure to leave?, y/n ");
    fgets(input, 5, stdin);

    if (input != NULL && input[0] == 'y') {
        *flag = FLAG_EXIT;
    }

}

void processMyPWD(int background, int *flag, pid_t * pid) {

    char out_buf[100];
    *pid = fork();
    if (*pid == 0) {
        printf("%s%s\n", "Your CWD is: ", getcwd(out_buf, -1));
        //Esto para que el ciclo del hijo finalice una vez termine la operacion
        *flag = FLAG_EXIT;
    } else if (*pid != -1) {
        if (background == 1) {
            printf("[%d]\n", *pid);
        } else {
            wait(NULL);
        }
    }

}

void processMyCp(int background, int *flag, pid_t *pid, char *origin, char *des) {
    *pid = fork();
    if (*pid == 0) {
        copy_file(origin, des);
        //Esto para que el ciclo del hijo finalice una vez termine la operacion
        *flag = FLAG_EXIT;
    } else if (*pid != -1) {
        if (background == 1) {
            printf("[%d]\n", *pid);
        } else {
            wait(NULL);
        }
    }
}

void copy_file(char* f_org, char* f_dest) {
    FILE *fp_org, *fp_dest;
    char c;

    if (!(fp_org = fopen(f_org, "rt")) || !(fp_dest = fopen(f_dest, "wt"))) {
        printf("Problems with mycp: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while ((c = fgetc(fp_org)) != EOF && !ferror(fp_org) && !ferror(fp_dest))
        fputc(c, fp_dest);

    if (ferror(fp_org) || ferror(fp_org))
        exit(EXIT_FAILURE);
    fclose(fp_org);
    fclose(fp_dest);
}

void processMyKill(int background, int *flag, int signal, char *pids[], int initial, int count, pid_t * pid) {

    pid_t _pid;
    *pid = fork();
    if (*pid == 0) {
        int i;
        for (i = initial; i < count; i++) {
            _pid = atoi(pids[i]);
            myKill(_pid, signal);
        }
        //Esto para que el ciclo del hijo finalice una vez termine la operacion
        *flag = FLAG_EXIT;
    } else if (*pid != -1) {
        if (background == 1) {
            printf("[%d]\n", *pid);
        } else {
            wait(NULL);
        }
    }

}

void myKill(pid_t pid, int signal) {

    if (kill(pid, signal) != 0) {
        printf("Problems with pid:[%d]! %s\n", pid, strerror(errno));

    }
}

void processMyGrep(int background, int *flag, pid_t *pid, char **items) {

    //char *comando[] = {"/bin/grep", items[1], items[2], NULL};
    //execv("/bin/grep", comando);

    *pid = fork();
    if (*pid == 0) {

        //Esto para que el ciclo del hijo finalice una vez termine la operacion
        *flag = FLAG_EXIT;
    } else if (*pid != -1) {
        if (background == 1) {
            printf("[%d]\n", *pid);
        } else {
            wait(NULL);
        }
    }
}

void processMyEcho(int background, int *flag, pid_t *pid, int num, char **items) {

    *pid = fork();
    if (*pid == 0) {
        for (int j = 1; j < num; j++) {
            printf("%s ", items[j]);
        }
        //Esto para que el ciclo del hijo finalice una vez termine la operacion
        *flag = FLAG_EXIT;
    } else if (*pid != -1) {
        if (background == 1) {
            printf("[%d]\n", *pid);
        } else {
            wait(NULL);
        }

    }
}

void myTime() {
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];
    strftime(output, 128, "%d/%m/%y %H:%M:%S", tlocal);
    printf("%s\n", output);
}

int getPipeIndex(){
    
    
    
    return -1;
}
