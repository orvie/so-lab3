/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

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

#include "parser.h"

//Declarations section
#define TAM 100

void processCommand(int* flag, char *command);

/*
 * 
 */
int main() {

    char ** items;
    int i, num, background;
    char expresion[TAM];
    char command[10];
    char out_buf[100];
    pid_t pid;
    int flag = 0;

    
    do {
        printf("%s@my-cli:%s$ ", getenv("USER"),getenv("PWD"));
        fgets(expresion, TAM, stdin);
        num = separaItems(expresion, &items, &background);
        if(strcmp(items[0],"mypwd")==0){
            pid = fork();
            
            if (pid == 0) {
                printf( "%s%s\n","Your CWD is: ",getcwd( out_buf, -1 ) );
            }else{
                wait(NULL);
            } 
        }else if (strcmp(items[0],"myexit")==0) {
            processCommand(&flag, &command);
        }
        
    }while (flag != -1);
    
}

void processCommand(int *flag, char command[]){
    printf( "%s","my-cli will exit. Do you want to continue.? y/n ");
    fgets(command, 10, stdin);

    if (strcmp(command,"y") == 0 || strcmp(command,"Y") == 0) {
        int res = -1;
        flag = &res;
    }
    
}

