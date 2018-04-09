/*
    parser.h
*/

#ifndef _SSOO_TMA_PARSER_H_
#define _SSOO_TMA_PARSER_H_

int separaItems (char * expresion,   // Palabras a separar
                 char *** items,     // Resultado
                 int * background);  // 1 si hay un & al final

void liberaItems (char ** items);

#endif


