/*
    parser.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int separaItems (char * expresion,   // Palabras a separar
                 char *** items,     // Resultado
                 int * background)   // 1 si hay un & al final
{
  int i, j, num, ult;
  char ** pp;
                    // En principio:
  *items = NULL;    //   cero parametros
  *background = 0;  //   ejecucion en primer plano

  for (i=0; expresion[i]!='\0'; i++)  // Cambiar saltos de
    if (expresion[i]=='\n' ||         // linea y tabuladores
        expresion[i]=='\t')           // por espacios
      expresion[i] = ' ';

  while (*expresion==' ')   // Quitar espacios del principio
    expresion ++;

  if (*expresion=='\0')     // Si cadena vacia ...
    return 0;               // ... cero parametros

  for (i=1, num=1, ult=0; expresion[i]!='\0'; i++)
    if (expresion[i]!=' ' &&
        expresion[i-1]==' ')            // Contar palabras
    {                                   // (parametros)
      num ++;
      ult = i;     // Recordar posicion de la ultima palabra
    }

  i --;
  while (i>=0 && expresion[i]==' ')   // Quitar espacios
    expresion[i--] = '\0';            // del final

                                      // Mirar si la ultima
  if (strcmp(expresion+ult,"&")==0)   // palabra es un &
  {
    *background = 1;  // Marcar ejecucion en segundo plano

    expresion[ult] = ' ';               // Quitar &

    while (i>=0 && expresion[i]==' ')   // Quitar espacios
      expresion[i--] = '\0';            // del final
                                        // (otra vez)
    num --;           // Descontar &

    if (*expresion=='\0')  // Si solo estaba el & ...
      return 0;            // ... cero parametros
  }

  pp = malloc ((num+1)*sizeof(char*));  // Pedir array
  if (pp==NULL) return -1;              // de punteros

  pp[0] = expresion;  // El primer parametro es facil

  for (i=1, j=1; expresion[i]!='\0'; i++)  // Localizar los
    if (expresion[i]!=' ' &&             // demas parametros,
        expresion[i-1]==' ')           // apuntar a ellos con
    {                                // los punteros del
      expresion[i-1] = '\0';       // array, y poner \0 en
      pp[j++] = expresion + i;   // lugar de espacios
    }
                  // Anyadir un puntero a NULL al final del
  pp[j] = NULL;   // array de punteros (asi es mas facil
                  // trabajar con el)
  *items = pp;
  return num;     // Devolver array y numero de palabras
}

void liberaItems (char ** items)
{
  free (items);
}
