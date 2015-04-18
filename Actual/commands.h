/**
*   Name:       Immanuel I George, Nicholas White, Jason Hindulak
*   Group:      ?
*   Date:       19-APR-2015
*   Project 3:  Floppy Shell Implementation
*   Goal:       Add additional UNIX commands to the Floppy Shell project, add inter-process communication functionality.
*
*   UPDATED:    19-APR-2015
**/
#ifndef COMMANDS_H
#define COMMANDS_H

//List of commands (according to instructions) which should be part of the child process:
//help
//traverse [-l]
//showsector [sector number]
//showfat

void help();	//Required
void traverse(int l);	//Required
void structure(int l);	//Required
void showfat();	//Required
void showsector(int sector);	//Required

//Floppy I/O
void load();
void unload();
void mount();
void unmount();

//Aux functions
int equals(char *str1, char* str2);
char *getFile();
int isMounted();

//bool programlunched = false;

#endif
