#ifndef PROCESSHANDLING_H
#define PROCESSHANDLING_H

int process_floppy_command(char *);

typedef struct {
				char* name;
				char** argv;
				char* input_file;
				char* output_file;
				int argc;
} command;

command* new_command(char*);
void execute_command(command*);
void delete_command(command*);

typedef struct {
				command** commands;
				int num_commands;
} process;

process* new_process(char*);
void execute_process(process*);
void delete_process(process*);

#endif