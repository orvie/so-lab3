
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

#include "parser.h"
#include "mycp.h"
#include "mytime.h"


//Declarations section
#define TAM 100
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

/*
 * 
 */
int main() {

    char ** items;
    int i, num, background;
    char expresion[TAM];

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
        if(num > 0){
            if(strcmp(items[0], MY_PWD) == 0 && num == 1) {
                processMyPWD(background, &flag, &pid);
            }else if (strcmp(items[0],MY_EXIT)==0) {
                processMyExit(&flag);
            }else if(strcmp(items[0],MY_PS_INFO)==0){
                execv("./bin/psinfo", items);
            }else if(strcmp(items[0],MY_CP)==0){
                //Validamos que venga el origen y destino
                if (num > 2) {
                    processMyCp(background, &flag, &pid, items[1], items[2]);
                }else{
                    printf("%s", "Invalid arguments. <orgin> <dest>\n");
                }

            }//
            else if (strcmp(items[0], MY_KILL) == 0) {
                
                if (num > 2) {
                    processMyKill(background, &flag, atoi(items[1]), items, 2, num, &pid);
                }else{
                    printf("%s", "Invalid arguments. <signal> <pid...>\n");
                }
                
            } else if (strcmp(items[0], "myecho") == 0) {
                for (int j = 1; j < num; j++) {
                    printf("%s ", items[j]);
                }
                printf("\n");
            } else if (strcmp(items[0], "myclr") == 0) {
                system("clear");
            } else if (strcmp(items[0], "mypause") == 0) {
                while (getchar() != '\n');
            } else if (strcmp(items[0], "myps") == 0) {
                pid = fork();
                if (pid == 0) {
                    char *comando[] = {"/bin/ps", items[1], NULL};
                    execv("/bin/ps", comando);
                    flag = -1;
                }else{
                    wait(NULL);
                }


            } else if (strcmp(items[0], "mygrep") == 0) {
                char *comando[] = {"/bin/grep", items[1], items[2], NULL};
                execv("/bin/grep", comando);
            } else if (strcmp(items[0], "mytime") == 0) {

                if (background == 1) {
                    pid_t _pd = fork();
                    if (_pd == 0) {
                        pid_t _pid = getpid();
                        printf("\n[%d]\n", _pid);
                        mytime();
                        flag = -1;
                    }
                }

            }
        }
       
        
    }while (flag != -1);
    
}

void processMyExit(int *flag){
    char input[5];
    printf("%s", "Are you sure to leave?, y/n ");
    fgets(input, 5, stdin);
    
    if(input != NULL && input[0] == 'y'){
        *flag = FLAG_EXIT;
    }
    
}

void processMyPWD(int background, int *flag, pid_t *pid) {
    
    char out_buf[100];
    printf("%s%s\n", "Your CWD is: ", getcwd(out_buf, -1));
/*
 * Sample with background.
    if (background == 1) {
        *pid = fork();
        if (*pid == 0) {
            printf("\n[%d]\n", getpid());
            printf("%s%s\n", "Your CWD is: ", getcwd(out_buf, -1));
            //Esto para que el ciclo del hijo finalice una vez termine la operacion
            *flag = -1;
        }

    }  else {
        printf("%s%s\n", "Your CWD is: ", getcwd(out_buf, -1));
    }
*/

}

void processMyCp(int background, int *flag, pid_t *pid, char *origin, char *des) {
    
    if (background == 1) {
        *pid = fork();
        if (*pid == 0) {
            copy_file(origin, des);
            //Esto para que el ciclo del hijo finalice una vez termine la operacion
            *flag = FLAG_EXIT;
        }else if(*pid != -1){
            printf("[%d]\n", *pid);
        }
    } else {
        copy_file(origin, des);
    }
    
}

void processMyKill(int background, int *flag, int signal, char *pids[], int initial, int count, pid_t *pid){
    
    pid_t _pid;
    if (background == 1) {
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
            printf("[%d]\n", *pid);
        }
    } else {
        int i;
        for (i = initial; i < count; i++) {
            _pid = atoi(pids[i]);
            myKill(_pid, signal);
        }
    }
    
}


void myKill(pid_t pid, int signal){
    
    if (kill(pid, signal) != 0) {
        printf("Problems with pid:[%d]! %s\n", pid, strerror(errno));
 
    }    
}

