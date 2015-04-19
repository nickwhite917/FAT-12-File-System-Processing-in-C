/**
*   Name:       Immanuel I George, Nicholas White, Jason Hindulak
*   Group:      ?
*   Date:       19-APR-2015
*   Project 3:  Floppy Shell Implementation
*   Goal:       Add additional UNIX commands to the Floppy Shell project, add inter-process communication functionality.
*
*   UPDATED:    19-APR-2015
**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "commands.h"
#include "processhandling.h"

#define COMMAND_EXECUTE 0
#define FLOP_MOUNT 1
#define FLOP_UMOUNT 2
#define FLOP_STRUCTURE 3
#define FLOP_SHOWSECTOR 4
#define FLOP_TRAVERSE 5
#define FLOP_SHOWFAT 6
#define FLOP_SHOWFILE 7
#define FLOP_HELP 8

#define copy_of_str(s) (strcpy((char*)malloc(strlen(s) + 1),s))

/*
* Scan for arguments and return them if found.
*/
static char* copy_scanned_argument(char** pointer, char* end_of_cmd) {
	char* scanner = *pointer;
	//Skip spaces
	while (*scanner == ' ' && scanner < end_of_cmd)
		scanner++;

	if (scanner == end_of_cmd) {
		*pointer = scanner;
		//No arguments supplied
		return NULL;
	} else {
		//Parse arguments
		char* arg_start = scanner;

		//Skip spaces
		while (*scanner != ' '&&scanner<end_of_cmd)
			scanner++;

		char* arg_end = scanner;
		*pointer = scanner;
		int argsize = arg_end - arg_start;
		char* narg = (char*)malloc(argsize + 1);
		strncpy(narg, arg_start, argsize);
		narg[argsize] = '\0';
		return narg;
	}
}

/*
* Command constructor method.
*/
command* new_command(char* cmdstr) {
	//Defaults
	command* cmd = (command*)malloc(sizeof(command));
	cmd->name = NULL;
	cmd->argv = NULL;
	cmd->output_file = NULL;
	cmd->input_file = NULL;
	cmd->argc = 0;
	char* first_space = strchr(cmdstr, ' ');

	if (first_space == NULL){ //No arguments
		cmd->name = copy_of_str(cmdstr);
		cmd->argv = (char**)malloc(sizeof(char*) * 2);
		cmd->argv[0] = copy_of_str(cmd->name);
		cmd->argv[1] = NULL;
		cmd->argc = 1;
	} else { //At least one argument
		int cmdnamelen = first_space - cmdstr;
		cmd->name = (char*)malloc(cmdnamelen + 1);
		strncpy(cmd->name, cmdstr, cmdnamelen);
		cmd->name[cmdnamelen] = '\0';
		cmd->argv = (char**)malloc(sizeof(char*) * 2);
		cmd->argv[0] = copy_of_str(cmd->name);
		cmd->argv[1] = NULL;
		cmd->argc = 1;
		char* scanner = first_space;
		char* end_of_cmd = cmdstr + strlen(cmdstr);

		//Scan for information
		while (scanner<end_of_cmd && *(++scanner)) {
			if (*scanner == '>') {
				scanner++;
				cmd->output_file = copy_scanned_argument(&scanner, end_of_cmd);
			} else if (*scanner == '<') {
				scanner++;
				cmd->input_file = copy_scanned_argument(&scanner, end_of_cmd);
			} else {
				char* narg = copy_scanned_argument(&scanner, end_of_cmd);

				if (narg != NULL){
					char** newargv = (char**)malloc(sizeof(char*) * (cmd->argc + 2));
					memcpy(newargv, cmd->argv, sizeof(char*) * (cmd->argc));
					newargv[cmd->argc++] = narg;
					newargv[cmd->argc] = NULL;

					if (cmd->argv){
						free(cmd->argv);
					}
					cmd->argv = newargv;
				}
			}
		}
	}
	return cmd;
}

/*
* Destructor method.
* Used to free commands from memory.
*/
void delete_command(command* cmd) {
	for (int i = 0; i < cmd->argc; i++){
		free(cmd->argv[i]);
	}
	if (cmd->argv){
		free(cmd->argv);
	}
	if (cmd->input_file){
		free(cmd->input_file);
	}
	if (cmd->output_file) {
		free(cmd->output_file);
	}
}

/*
* Process constructor method.
*/
process* new_process(char* procstr) {
	//Defaults
	process* ptr = (process*)malloc(sizeof(process));
	ptr->num_commands = 0;
	ptr->commands = NULL;
	char* scanner = procstr;
	char* end = procstr + strlen(procstr);

	//Scan for information
	while (scanner < end) {
		while (*scanner == ' ') {
			scanner++;
        }

		char* end_of_cmd = strchr(scanner, '|'); //Search for pipes '|'

		if (end_of_cmd == NULL){
			end_of_cmd = end;
		}

		char* cmd = strncpy((char*)malloc(end_of_cmd - scanner + 1), scanner, end_of_cmd - scanner);
		cmd[end_of_cmd - scanner] = '\0';
		command** ncmds = (command**)malloc((sizeof(command*)) * (ptr->num_commands + 1));
		memcpy(ncmds, ptr->commands, (sizeof(command*)) * ptr->num_commands);
		ncmds[ptr->num_commands++] = new_command(cmd);

		if (ptr->commands) {
			free(ptr->commands);
		}

		ptr->commands = ncmds;
		free(cmd);
		scanner = end_of_cmd + 1;
	}
	return ptr;
}

/*
* Executes a command.
* Will search for a built in Floppy command first before
* defaulting to a Linux command.
*/
void execute_command(command* cmd) {
	/* Check for Floppy commands */
	switch(process_floppy_command(cmd->name)) {
		/*
		* STRUCTURE
		*/
		case FLOP_STRUCTURE: {
            if (!isMounted()) {
                printf("    ERROR: no floppy mounted.\n");
                break;
            }

            if (cmd->argc == 1) {
                structure(0);
            } else if (cmd->argc == 2) {
                if (strcmp(cmd->argv[1], "-l") == 0) {
                    structure(1);
                } else {
                    printf("Usage: structure [-l]\n");
                }
            } else {
                printf("Usage: structure [-l]\n");
            }
			break;
		}

		/*
		* SHOWSECTOR
		*/
		case FLOP_SHOWSECTOR: {
            if (!isMounted()) {
                printf("    ERROR: no floppy mounted.\n");
                break;
            }

            if (cmd->argc == 2) {
                int sector = atoi(cmd->argv[1]);
                showsector(sector);
            } else {
                printf("Usage: showsector <sector>\n");
            }
			break;
		}

		/*
		* TRAVERSE
		*/
		case FLOP_TRAVERSE: {
            if (!isMounted()) {
                printf("    ERROR: no floppy mounted.\n");
                break;
            }

            if (cmd->argc == 1) {
                traverse(0);
            } else if (cmd->argc == 2) {
                if (strcmp(cmd->argv[1], "-l") == 0) {
                    traverse(1);
                } else {
                    printf("Usage: traverse [-l]\n");
                }
            } else {
                printf("Usage: traverse [-l]\n");
            }
			break;
		}

		/*
		* SHOWFAT
		*/
		case FLOP_SHOWFAT: {
            if (!isMounted()) {
                printf("    ERROR: no floppy mounted.\n");
                break;
            }

            if (cmd->argc == 1) {
                showfat();
            } else {
                printf("Usage: showfat\n");
            }
			break;
		}
		case FLOP_HELP: {
			help();
		}

		/*
		* DEFAULT: Execute command
		*/
		default: {
			execvp(cmd->name, cmd->argv);
            printf("%s: command not found\n", cmd->name);
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}

/*
* Execute a process (runs a command)
* Will pipe each command into each other if necessary.
*/
void execute_process(process* proc) {
	/* pipe() -> read_from, write_to */
	int original_stdout = dup(STDOUT_FILENO); //STDOUT
	int original_stdin = dup(STDIN_FILENO); //STDIN
	int inode[2];

	//Create two pipes (inode)
	pipe(inode);

	//Execute each command
	for (int i = 0; i < proc->num_commands; i++){
		int nnode[2];
		pipe(nnode);
		command* cmd = proc->commands[i];
		int pid = fork();

		if ( pid == 0 ) {
			/* Child process */
			if(i==0&&cmd->input_file){
				/*
					If the first command has
					an input file, use it instead
					of stdin. Ignore input files
					on commands that aren't the first in the chain.
				*/
				int fnum = open(cmd->input_file, O_RDONLY);
				dup2(fnum,STDIN_FILENO);
				close(fnum);
			} else {
				dup2(inode[0],STDIN_FILENO);
			}

			close(nnode[0]);

			if((i + 1) == proc->num_commands){
				//Last process gets STDOUT again
				if(cmd->output_file) {
					/*
						If this one has an output
						file specified, use it.
					*/
					int fnum = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(fnum,STDOUT_FILENO);
					close(fnum);
				} else {
					dup2(original_stdout,STDOUT_FILENO);
					close(original_stdout);
				}
			} else {
				dup2(nnode[1],STDOUT_FILENO);
				close(nnode[1]);
			}
			execute_command(cmd);
		} else if ( pid < 0 ) {
			/* Error */
			perror("fork");
			exit(EXIT_FAILURE);
		} else {
			/* Parent process */
			waitpid(pid,NULL,0);
			close(inode[0]);
			close(inode[1]);
			close(nnode[1]);
			inode[0] = nnode[0];
			inode[1] = nnode[1];
		}
	}

	// Restore original STDIN/OUT
	dup2(original_stdin,STDIN_FILENO);
	close(original_stdin);
}

/*
* Destructor method.
* Used to free process from memory
*/
void delete_process(process* proc) {
	for (int i = 0; i < proc->num_commands; i++){
		delete_command(proc->commands[i]);
	}
}

/*
* Floppy command checker
*/
int process_floppy_command(char *cmd) {
    if (strstr(cmd, "structure") == cmd) {
        return FLOP_STRUCTURE;
    }
    if (strstr(cmd, "showsector") == cmd) {
        return FLOP_SHOWSECTOR;
    }
    if (strstr(cmd, "traverse") == cmd) {
        return FLOP_TRAVERSE;
    }
    if (strstr(cmd, "showfat") == cmd) {
        return FLOP_SHOWFAT;
    }
    if (strstr(cmd, "help") == cmd) {
        return FLOP_help;
    }
    return COMMAND_EXECUTE;
}
