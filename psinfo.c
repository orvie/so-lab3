#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//Declarations section
#define EXEC_INFO "-- Información recolectada!!!\n"
#define P_ID "Pid: "
#define P_NAME  "Nombre del proceso: "
#define P_STATUS "Estado: "
#define TOT_MEM_SIZE "Tamaño total de la imagen de memoria: "
#define TXT_MEM_SIZE "	Tamaño de la memoria en la region TEXT: "
#define DAT_MEM_SIZE "	Tamaño de la memoria en la region DATA: "
#define STK_MEM_SIZE "	Tamaño de la memoria en la region STACK: "
#define CTXT_CHANGES "Numero de cambios de contexto realizados (voluntarios - no voluntarios): "
#define F_NOT_FOUND "Identificador de proceso no valido o archivo de informacion protegido o no encontrado. "
#define ERROR_PROC "Error: "
#define ERROR_NO_ARG "Argumentos incompletos"
#define ERROR_PROC_SAMPLE "Valid arguments [-l] [-r] pid [pids]"
#define FILE_CREATED "Archivo de salida generado: "
#define ERROR_FILE_CREATED "Problemas generando el archivo de reporte."
#define PROPERTY_NOT_FOUND "Propiedad no encontrada.\n"

#define NEW_LINE "\n"
#define PATH_PROC_DIR "/proc/"
#define STATUS_FILE "/status"
#define MAX_LINE 100

/**
 * Structure to save process info
 */
typedef struct {
	char p_id[25];
	char p_name[50];
	char p_status[50];
	char total_mem_size[50];
	char data_mem_size[50];
	char text_mem_size[50];
	char stack_mem_size[50];
	int ctext_change_v;
	int ctext_change_nv;
	char p_not_found_msg[255];
	int p_not_found_id;
} proc_info_t;

//Function declaration section
proc_info_t getProcessinfo(char pid[]);

void processLine(proc_info_t* p_info_t, char* lineptr);

void writeProcessToFile(proc_info_t* p_info_t, FILE *file_to_write);

void showProcessInfo(proc_info_t* p_info_t);

int processError();

int main(int argc, char *argv[]) {
	

	if (argc == 2) { //call with PID expected

		if (strcmp("-l", argv[1]) != 0 && strcmp("-r", argv[1]) != 0) {

			proc_info_t my_proc_t = getProcessinfo(argv[1]);
			showProcessInfo(&my_proc_t);

		}else { //Bad argument
			return processError();
		}

	}else if (argc > 2) { //Call with -l or -r expected

		//List on console the processes info
		if (strcmp("-l", argv[1]) == 0) {

			printf("%s", EXEC_INFO);
			for (int i = 2; i < argc; ++i) {
				proc_info_t my_proc_t = getProcessinfo(argv[i]);
				showProcessInfo(&my_proc_t);
			}

		}else if (strcmp("-r", argv[1]) == 0) { // Report process to a file

			proc_info_t p_info_list_t[argc - 2];
			char f_name[255];
			strcpy(f_name, "psinfo-report");

			/**
			 * Get each process info into an array and creates the file name
			 *
			 */
			for (int i = 2; i < argc; ++i) {
				p_info_list_t[i-2] = getProcessinfo(argv[i]);
				strcat(f_name, "-");
				strcat(f_name, argv[i]);
			}

			//Add extension to the output file name
			strcat(f_name, ".info");

			//Creates my file stream
			FILE *file_to_write;

			printf("%s %s \n", "Attempting to create file: ", f_name);
			file_to_write = fopen(f_name, "w");

			if (file_to_write != NULL) {
				printf("%s %s \n", "File created: ", f_name);
				fputs(EXEC_INFO, file_to_write);
				int counter = argc - 2;

				/**
				 * Writes each process info into the file stream
				 */
				for (int i = 0; i < counter; ++i) {
					writeProcessToFile(&p_info_list_t[i], file_to_write);
				}

				//Close our file
				fclose(file_to_write);
				printf("%s %s \n", FILE_CREATED, f_name);
			}else{
				printf("%s \n", ERROR_FILE_CREATED);
			}

		}else {
			return processError();
		}

	}else {
		return processError();
	}

	return 0;
}

/**
 * Function to look for a process id, and return a defined structure for process
 */
proc_info_t getProcessinfo(char pid[]){

	//Create a local variable to save process info
	proc_info_t local_proc_t;
        strcpy(local_proc_t.p_status, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.data_mem_size, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.p_id, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.p_name, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.stack_mem_size, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.text_mem_size, PROPERTY_NOT_FOUND);
        strcpy(local_proc_t.total_mem_size, PROPERTY_NOT_FOUND);
        local_proc_t.ctext_change_nv = -1;
        local_proc_t.ctext_change_v = -1;
        

	//Create an other variable to get the file
	FILE *file_to_read;

	//Get memory dynamically
	char* lineptr = (char *) malloc(MAX_LINE * sizeof(char));

	//Try to read the given pid file status
	char file_p[50];
	strcpy(file_p, "");
	strcat(file_p, PATH_PROC_DIR);
	strcat(file_p, pid);
	strcat(file_p, STATUS_FILE);

	file_to_read = fopen(file_p, "r");

	if (file_to_read != NULL) {

		local_proc_t.p_not_found_id = 0;

		size_t size_l_t = 0;
		while(getline(&lineptr,&size_l_t,file_to_read) != -1){
			//Fetch for a line
			//printf("%s",lineptr);
			processLine(&local_proc_t, lineptr);

		}
		//Close file
		fclose(file_to_read);

	} else {
		//When file not found write simple process info
		local_proc_t.p_not_found_id = -1;
		strcpy(local_proc_t.p_id, pid);
		strcpy(local_proc_t.p_not_found_msg,F_NOT_FOUND);
	}

	//and free memory
	free(lineptr);

	return local_proc_t;
}

/**
 * Function to process a line of the file and get info needed to my
 * process info structure and these info is set to my pointer of process info
 */
void processLine(proc_info_t* p_info_t, char* lineptr){

	char* token = strtok(lineptr, ":");

	if (token != NULL) {

		if (strcmp(token,"Pid") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->p_id,token);

		}else if (strcmp(token,"Name") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->p_name,token);

		}else if (strcmp(token,"State") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->p_status,token);

		}else if (strcmp(token,"VmSize") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->total_mem_size,token);

		}else if (strcmp(token,"VmData") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->data_mem_size,token);

		}else if (strcmp(token,"VmStk") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->stack_mem_size,token);

		}else if (strcmp(token,"VmExe") == 0) {
			token = strtok(NULL, ":");
			strcpy(p_info_t->text_mem_size,token);

		}else if (strcmp(token,"voluntary_ctxt_switches") == 0) {
			token = strtok(NULL, ":");
			p_info_t->ctext_change_v = atoi(token);

		}else if (strcmp(token,"nonvoluntary_ctxt_switches") == 0) {
			token = strtok(NULL, ":");
			p_info_t->ctext_change_nv = atoi(token);

		}else{
                    
                }

	}

}

/**
 * Function to print process info on console
 */
void showProcessInfo(proc_info_t* p_info_t){

	if (p_info_t->p_not_found_id == 0) {

		printf("%s %s", P_ID, p_info_t->p_id);
		printf("%s %s", P_NAME, p_info_t->p_name);
		printf("%s %s", P_STATUS, p_info_t->p_status);
		printf("%s %s", TOT_MEM_SIZE, p_info_t->total_mem_size);
		printf("%s %s", TXT_MEM_SIZE, p_info_t->text_mem_size);
		printf("%s %s", DAT_MEM_SIZE, p_info_t->data_mem_size);
		printf("%s %s", STK_MEM_SIZE, p_info_t->stack_mem_size);
		printf("%s %d %s %d \n\n", CTXT_CHANGES, p_info_t->ctext_change_v, "-", p_info_t->ctext_change_nv);
	}else {
		printf("%s %s \n", P_ID, p_info_t->p_id);
		printf("%s %s \n\n", ERROR_PROC, p_info_t->p_not_found_msg);
	}

}

/**
 * Function to write into a file stream the process info
 */
void writeProcessToFile(proc_info_t* p_info_t, FILE *file_to_write){

	if (p_info_t->p_not_found_id == 0) {

		fprintf(file_to_write, "%s %s", P_ID, p_info_t->p_id);
		fprintf(file_to_write, "%s %s", P_NAME, p_info_t->p_name);
		fprintf(file_to_write, "%s %s", P_STATUS, p_info_t->p_status);
		fprintf(file_to_write, "%s %s", TOT_MEM_SIZE, p_info_t->total_mem_size);
		fprintf(file_to_write, "%s %s", TXT_MEM_SIZE, p_info_t->text_mem_size);
		fprintf(file_to_write, "%s %s", DAT_MEM_SIZE, p_info_t->data_mem_size);
		fprintf(file_to_write, "%s %s", STK_MEM_SIZE, p_info_t->stack_mem_size);
		fprintf(file_to_write, "%s %d %s %d \n\n", CTXT_CHANGES, p_info_t->ctext_change_v, "-", p_info_t->ctext_change_nv);

	}else {

		fprintf(file_to_write, "%s %s \n", P_ID, p_info_t->p_id);
		fprintf(file_to_write, "%s %s \n\n", ERROR_PROC, p_info_t->p_not_found_msg);

	}
}

/**
 * Handle errors
 */
int processError(){
	printf("%s%s\n",ERROR_PROC, ERROR_NO_ARG);
	printf("%s\n", ERROR_PROC_SAMPLE);
	return -1;
}
