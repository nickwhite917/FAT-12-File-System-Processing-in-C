#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "Fat12BootEntry.h"
#include <unistd.h>  
#include <fcntl.h>

#include <errno.h>

//Define global variables
int img ;

int mounted = 0;
char *filename = '\0';

//Define FAT12Boot Structure
Fat12Boot boot;
Fat12Entry *entry;

/*
*   Loads all FAT12 boot information into 'boot' and FAT12 entries into 'entry'
*/
void load() {
    lseek(img, 11, SEEK_SET); //skip 11 bytes
    //lseek();

    //BYTES_PER_SECTOR (2 bytes)
    //fread(&boot.BYTES_PER_SECTOR, 2, 1, img);
    read(img,&boot.BYTES_PER_SECTOR,2);
    //printf("BYTES_PER_SECTOR: %d\n", boot.BYTES_PER_SECTOR);

    //SECTORS_PER_CLUSTER (2 bytes)
    //fread(&boot.SECTORS_PER_CLUSTER, 1, 1, img);
    read(img,&boot.SECTORS_PER_CLUSTER,1);
    //printf("SECTORS_PER_CLUSTER: %d\n", boot.SECTORS_PER_CLUSTER);
    //RESERVED_SECTORS (2 bytes)
    //fread(&boot.RESERVED_SECTORS, 2, 1, img);
    read(img,&boot.RESERVED_SECTORS,2);
    //printf("RESERVED_SECTORS: %d\n", boot.RESERVED_SECTORS);
    //NUM_OF_FATS (1 byte)
    //fread(&boot.NUM_OF_FATS, 1, 1, img);
    read(img,&boot.NUM_OF_FATS,1);
    //printf("NUM_OF_FATS: %d\n", boot.NUM_OF_FATS);
    //MAX_ROOT_DIRS (2 bytes)
    //fread(&boot.MAX_ROOT_DIRS, 2, 1, img);
    read(img,&boot.MAX_ROOT_DIRS,2);
    //printf("MAX_ROOT_DIRS: %d\n", boot.MAX_ROOT_DIRS);
    //Initialize 'entry'
    entry = (Fat12Entry *)malloc(sizeof(Fat12Entry) * boot.MAX_ROOT_DIRS);

    //TOTAL_SECTORS (2 bytes)
    //fread(&boot.TOTAL_SECTORS, 2, 1, img);
    read(img,&boot.TOTAL_SECTORS,2);
    lseek(img, 1, SEEK_CUR); //skip 1 byte

    //SECTORS_PER_FAT (2 bytes)
    //fread(&boot.SECTORS_PER_FAT, 2, 1, img);
    read(img,&boot.SECTORS_PER_FAT,2);
    //SECTORS_PER_TRACK (2 bytes)
    //fread(&boot.SECTORS_PER_TRACK, 2, 1, img);
    read(img,&boot.SECTORS_PER_TRACK,2);
    //NUM_OF_HEADS (2 bytes)
    //fread(&boot.NUM_OF_HEADS, 2, 1, img);
    read(img,&boot.NUM_OF_HEADS,2);
    lseek(img, 11, SEEK_CUR); //skip 11 bytes

    //VOUME_ID (4 bytes)
   // fread(&boot.VOLUME_ID, 4, 1, img);
    read(img,&boot.VOLUME_ID,4);
    //VOLUME_LABEL
    //fread(&boot.VOLUME_LABEL, 11, 1, img);
    read(img,&boot.VOLUME_LABEL,11);
    //printf("RET: %d: %s\n", ret, strerror(errno));
    //printf("IMG: %d\n", img);

    //Move to beginning of ROOT DIRECTORY
    lseek(img, ((boot.NUM_OF_FATS * boot.SECTORS_PER_FAT) + 1) * boot.BYTES_PER_SECTOR, SEEK_SET);
    //printf("RET: %d: %s\n", ret, strerror(errno));
    //printf("IMG: %d\n", img);
    //printf("OFFSET: %d\n", ((boot.NUM_OF_FATS * boot.SECTORS_PER_FAT) + 1) * boot.BYTES_PER_SECTOR);

    for (int i = 0; i < boot.MAX_ROOT_DIRS; i++) {

        //FILENAME (8 bytes)
        //fread(&entry[i].FILENAME, 8, 1, img);
        read(img,&entry[i].FILENAME,8);

        //EXT (3 bytes)
        //fread(&entry[i].EXT, 3, 1, img);
        read(img,&entry[i].EXT,3);
        //ATTRIBUTES (1 byte)
        //fread(&entry[i].ATTRIBUTES, 1, 1, img);
        read(img,&entry[i].ATTRIBUTES,1);

        //RESERVED (10 bytes)
       // fread(&entry[i].RESERVED, 2, 1, img);
        read(img,&entry[i].RESERVED,2);
        //CREATION_TIME (2 bytes)
        //fread(&entry[i].CREATION_TIME, 2, 1, img);
        read(img,&entry[i].CREATION_TIME,2);
        //CREATION_DATE (2 bytes)
        //fread(&entry[i].CREATION_DATE, 2, 1, img);
        read(img,&entry[i].CREATION_DATE,2);
        //LAST_ACCESS_DATE (2 bytes)
        //fread(&entry[i].LAST_ACCESS_DATE, 2, 1, img);
        read(img,&entry[i].LAST_ACCESS_DATE,2);
        lseek(img, 2, SEEK_CUR); //skip 2 bytes
        //MODIFY_TIME (2 bytes)
        //fread(&entry[i].MODIFY_TIME, 2, 1, img);
        read(img,&entry[i].MODIFY_TIME,2);
        //MODIFY_DATE (2 bytes)
        //fread(&entry[i].MODIFY_DATE, 2, 1, img);
        read(img,&entry[i].MODIFY_DATE,2);
        //START_CLUSTER (2 bytes)
        //fread(&entry[i].START_CLUSTER, 2, 1, img);
        read(img,&entry[i].START_CLUSTER,2);
        //FILE_SIZE (4 bytes)
        //fread(&entry[i].FILE_SIZE, 4, 1, img);
        read(img,&entry[i].FILE_SIZE,4);
    }

   // printf("IMG: %d\n", img);
}

/*
*   Clears 'boot'
*/
void unload() {
    boot = (Fat12Boot){0};
    entry = (Fat12Entry *){0};
}

/*
*   Saves file information and opens the file
*/
void mount(char *file) {
    
    filename = (char *)malloc(strlen(file) + 1);
    strcpy(filename, file);
    img = open(file, O_RDONLY);
    mounted = 1;
    load();
    //printf("Done!\n");
}

/*
*   Clears  file information and closes the file
*/
void unmount() {
    printf("    Unmounting `%s`... ", filename);
    filename = '\0';
    close(img);
    mounted = 0;
    unload();
    printf("Done!\n");
}

/*
*   Prints FAT12 image structure
*/
void structure(int l) {
    printf("    Mounted\n");
    printf("        number of FAT:                      %d\n", boot.NUM_OF_FATS);
    printf("        number of sectors used by FAT:      %d\n", boot.SECTORS_PER_FAT);
    printf("        number of sectors per cluster:      %d\n", boot.SECTORS_PER_CLUSTER);
    printf("        number of ROOT Entries:             %d\n", boot.MAX_ROOT_DIRS);
    printf("        number of bytes per sector          %d\n", boot.BYTES_PER_SECTOR);
    if (l) {
        printf("        ---Sector #---      ---Sector Types---\n");
        printf("             0                    BOOT\n");

        for(int i = 0; i < boot.NUM_OF_FATS; i++)
                printf("          %02d -- %02d                FAT%d\n", (boot.SECTORS_PER_FAT * i) + 1, boot.SECTORS_PER_FAT * (i + 1), i);

        printf("          %02d -- %02d                ROOT DIRECTORY\n", boot.SECTORS_PER_FAT * boot.NUM_OF_FATS, (boot.MAX_ROOT_DIRS / 16) + (boot.SECTORS_PER_FAT * boot.NUM_OF_FATS));
    }
}

/*
*   Lists all files within the FAT12 image
*/
void traverse(int l) {
    if (l) {
        printf("    Mounted\n");
        printf("    *****************************\n");
        printf("    ** FILE ATTRIBUTE NOTATION **\n");
        printf("    **                         **\n");
        printf("    ** R ------ READ ONLY FILE **\n");
        printf("    ** S ------ SYSTEM FILE    **\n");
        printf("    ** H ------ HIDDEN FILE    **\n");
        printf("    ** A ------ ARCHIVE FILE   **\n");
        printf("    *****************************\n");
        printf("\n");
        //printf("jsrjkfsksbsf%d\n", boot.MAX_ROOT_DIRS);
        for (int i = 0; i < boot.MAX_ROOT_DIRS; i++) {
            if (entry[i].FILENAME[0] != 0x00 && entry[i].START_CLUSTER != 0) {
                char attr[6] = {'-', '-', '-', '-', '-'};
                unsigned char a = entry[i].ATTRIBUTES[0];
                if (a == 0x01)
                    attr[0] = 'R';
                if (a == 0x02)
                    attr[1] = 'H';
                if (a == 0x04)
                    attr[2] = 'S';
                if (a == 0x20)
                    attr[5] = 'A';
                if (a == 0x10) {
                    for (int j = 0; j < 6; j++)
                        attr[j] = '-';
                }

                if (entry[i].ATTRIBUTES[0] == 0x10) {
                    printf("%.6s    %d %d       < DIR >      /%.8s                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/.                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, entry[i].START_CLUSTER);
                    printf("%.6s    %d %d       < DIR >      /%.8s/..                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILENAME, 0);
                } else {
                    printf("%.6s    %d %d       %lu      /%.8s.%.3s                 %d\n", attr, entry[i].CREATION_DATE, entry[i].CREATION_TIME, entry[i].FILE_SIZE, entry[i].FILENAME, entry[i].EXT, entry[i].START_CLUSTER);
                }
            }
        }

    } else {
       // printf("jsrjkfsksbsf%d\n", boot.MAX_ROOT_DIRS);
        for (int i = 0; i < boot.MAX_ROOT_DIRS; i++) {
            if (entry[i].FILENAME[0] != 0x00 && entry[i].START_CLUSTER != 0) {
                if (entry[i].ATTRIBUTES[0] == 0x10) {
                    printf("/%.8s                       < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/.                     < DIR >\n", entry[i].FILENAME);
                    printf("/%.8s/..                    < DIR >\n", entry[i].FILENAME);
                } else {
                    printf("/%.8s.%.3s\n", entry[i].FILENAME, entry[i].EXT);
                    
                }
            }
        }
    }
}

/*
*   Outputs a hex dump of all FAT tables
*/
void showfat() {
    printf("    Mounted\n");
    unsigned char in;
    char * free = "FREE";

    lseek(img, boot.BYTES_PER_SECTOR, SEEK_SET);

    printf("        0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\n");
    for (int i = 0; i < (boot.NUM_OF_FATS * boot.SECTORS_PER_FAT * boot.BYTES_PER_SECTOR); i++) {
        if (i % 16 == 0 || i == 0) {
            printf("\n");
            printf("%4x", i);
        }
        //fread(&in, 1, 1, img);
        read(img,&in,1);
       if(in == 0){
printf("%5s",free);
}
else{
    printf("%5x", in);
}
    }

    printf("\n");
}

/*
*   Outputs a hex dump of a specific sector (512 bytes)
*/
void showsector(int sector) {
    printf("    Mounted\n");
    unsigned char in;

    lseek(img, boot.BYTES_PER_SECTOR * sector, SEEK_SET);

    printf("        0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\n");
    for (int i = 0; i < boot.BYTES_PER_SECTOR; i++) {
        if (i % 16 == 0 || i == 0) {
            printf("\n");
            printf("%4x", i);
        }
        //fread(&in, 1, 1, img);
        read(img,&in,1);
        printf("%5x", in);
    }

    printf("\n");
}

/*
*   Outputs a hex dump of a specific file
*/
// void showfile(char *file) {
//     printf("    Mounting `%s`... ", file);
//     char *filename = '\0';
//     char *ext = '\0';
//     char *full_filename;
//     char *p;

//     //Parse filename
//     p = strtok(file, ".");
//     if (p)
//         filename = p;

//     //Parse extension
//     p = strtok(NULL, ".");
//     if (p)
//         ext = p;

//     full_filename = (char *)malloc(strlen(filename) + strlen(ext) + 1);
//     strcpy(full_filename, filename);
//     strcat(full_filename, ext);

//     for (int i = 0; i < boot.MAX_ROOT_DIRS; i++) {
//         if (entry[i].FILENAME != 0x00 && entry[i].START_CLUSTER != 0) { //Ignore invalid files
//             if(equals(full_filename, (char *)entry[i].FILENAME)) {
//                 unsigned char in;

//                 //Move to the first byte of the file
//                 lseek(img, ((boot.MAX_ROOT_DIRS / 16) + (boot.SECTORS_PER_FAT * boot.NUM_OF_FATS) - 1) + (boot.BYTES_PER_SECTOR * entry[i].START_CLUSTER), SEEK_SET);

//                 printf("        0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f\n");
//                 for (int j = 0; j < entry[i].FILE_SIZE; j++) {
//                     if (j % 16 == 0 || j == 0) {
//                         printf("\n");
//                         printf("%4x", j);
//                     }
//                     //fread(&in, 1, 1, img);
//                     read(img,&in,1);
//                     printf("%5x", in);
//                 }

//                 printf("\n");
//             }
//         }
//     }
// }

/*
*   Compares two char*'s exactly
*   Used for comparing FILENAME's
*/
int equals(char * str1, char *str2) {
    for (int i = 0; i < strlen(str1); i++) {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

/*
*   Returns the filename
*/
char *getFile() {
    return filename;
}

/*
*   Returns 1 if the file is mounted, 0 if it is not.
*/
int isMounted() {
    return mounted;
}
