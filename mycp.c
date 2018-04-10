/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>


int copy_file(char* f_org,char* f_dest)
{
	FILE *fp_org,*fp_dest; 
	char c;
 
	if(!(fp_org=fopen(f_org,"rt")) || !(fp_dest=fopen(f_dest,"wt")))
	{
		perror("Error de apertura de ficheros");
		exit(EXIT_FAILURE);
	}
 
	while((c=fgetc(fp_org)) != EOF && !ferror(fp_org) && !ferror(fp_dest))
		fputc(c,fp_dest);
 
	if(ferror(fp_org) || ferror(fp_org))
		return 1;
 
	fclose(fp_org);
	fclose(fp_dest);
	return 0;
}