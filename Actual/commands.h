#ifndef COMMANDS_H
#define COMMANDS_H

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
//bool programlunched = false;

#endif
