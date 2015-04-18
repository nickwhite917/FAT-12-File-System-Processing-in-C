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

//Loads the actual data from the floppy into the corresponding struct
void load();
void unload();
void mount();
void unmount();
void structure(int l);
void traverse(int l);
void showfat();
void showsector(int sector);
void showfile(char *file);
int equals(char *str1, char* str2);
char *getFile();
int isMounted();
void help();
//bool programlunched = false;

#endif
