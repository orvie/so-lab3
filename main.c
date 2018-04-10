
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
#include "mycp.h"
#include "mykill.h"
#include "mytime.h"

//Declarations section
#define TAM 100

/*
 * 
 */
int main() {

    char ** items;
    int i, num, background;
    char expresion[TAM];
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
            flag = -1;
        }else if(strcmp(items[0],"psinfo")==0){
            execv("./bin/psinfo", items);
        }else if(strcmp(items[0],"mycp")==0){
            copy_file(items[1], items[2]);
        }else if(strcmp(items[0],"mykill")==0){
            mykill(items[1], items[2]);
        }else if(strcmp(items[i],"myecho")==0){
            for (int j=1; j<num; j++){
                printf ("%s ", items[j]);
            }
            printf ("\n");
        }else if(strcmp(items[i],"myclr")==0){
            system("clear");
        }else if(strcmp(items[i],"mypause")==0){
            while (getchar() != '\n' );  
        }else if(strcmp(items[i],"myps")==0){
            char *comando[] = { "/bin/ps" , items[1], NULL };
            execv( "/bin/ps" , comando); 
        }else if(strcmp(items[i],"mygrep")==0){
            char *comando[] = { "/bin/grep" , items[1], items[2], NULL };
            execv( "/bin/grep" , comando); 
        }else if(strcmp(items[i],"mytime")==0){
            
            if (background == 1) {
                pid = fork();
                if (pid == 0) {
                   pid_t pid = getpid();
                   printf("\n[%d]\n", pid);
                   mytime();
                } 
            }

            
            
        }
        
    }while (flag != -1);
    
}

char* execBg(){
    
}



