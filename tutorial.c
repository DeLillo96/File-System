#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUTMAX 65036
#define MAXNAME 255
#define MAXFILE 8192
#define MAXCHILDS 1024
#define MAXHEIGHT 255

typedef struct{
    char name[MAXNAME];
    char text[MAXFILE];
} file;

typedef struct {
    char name[MAXNAME];
    void * childs[MAXCHILDS];
    int nChilds;
} directory;

int isAlphanumeric(char *);
int getCommand(char *);
directory createFile(directory, char *);
char* getNeedle(char *, int);

int main() {
    directory root;
    char command[INPUTMAX];
    char * path;
    int exit = 0, nCommand;
    while (exit == 0) {
        printf("$>");
        fgets (command, INPUTMAX, stdin);
        if( 1 == isAlphanumeric(command) ) {
            nCommand = getCommand(command);
            switch (nCommand) {
                case 2:
                    root = createFile(root, command);
                break;
                case 8: exit = 1;
            }
        } else {
            printf("no\n");
        }
    }
    return 0;
}

int isAlphanumeric( char * string ) {
    int i;
    for (i = 0; i < INPUTMAX; i ++) {
        if (!(
            (string[i] >= 'A' && string[i] <= 'Z') ||
            (string[i] >= 'a' && string[i] <= 'z') ||
            (string[i] >= '0' && string[i] <= '9') ||
            string[i] == '_' ||
            string[i] == '/' ||
            string[i] == ' '
        )) {
            if(string[i] == '\0' || string[i] == '\n')
                return 1;
            else
                return 0;
         }
    }
    return 1;
}

int getCommand( char * command ) {
    if( 0 == strncmp(command, "create_dir", 10) ) return 1;
    if( 0 == strncmp(command, "create", 6) ) return 2;
    if( 0 == strncmp(command, "read", 4) ) return 3;
    if( 0 == strncmp(command, "write", 5) ) return 4;
    if( 0 == strncmp(command, "delete_r", 8) ) return 5;
    if( 0 == strncmp(command, "delete", 6) ) return 6;
    if( 0 == strncmp(command, "find", 4) ) return 7;
    if( 0 == strncmp(command, "exit", 4) ) return 8;
    return 0;
}

directory createFile(directory fs, char * command ) {
    directory probeDir, dirLine[MAXHEIGHT];
    file newFile = *((file *) malloc(sizeof(file)));
    char* needle;
    int commandLength, length = 7, i, dirIndex = 0, childIndexes[MAXHEIGHT];

    probeDir = fs;
    dirLine[dirIndex] = fs;
    dirIndex++;

    commandLength = (strlen(command) - 1);
    needle = getNeedle(command, length);
    length += strlen(needle) + 1;

    while(length != commandLength) {
        for(i = 0; i < probeDir.nChilds; i++) {
            probeDir = *((directory *) &probeDir.childs[i]);
            if( 0 == strncmp(probeDir.name, needle, 0)) {
                break;
            }
        }
        needle = getNeedle(command, length);
        length += strlen(needle) + 1;
    }

    strcpy(newFile.name, needle);

    probeDir.childs[ probeDir.nChilds ] = (void *) & newFile;
    probeDir.nChilds++;

    for(i = (dirIndex - 1); i >= 0; i--) {
        dirLine[i] = probeDir;
    }

    return dirLine[0];
}

char* getNeedle(char * path, int startIndex) {
    char * needle;
    int i, c = 0;

    needle = (char *)malloc(MAXNAME*sizeof(char));

    if( path[startIndex] == '/' ) {
        startIndex++;
        for (i = startIndex; i < MAXNAME; i ++) {
            if(path[i] != '/' && path[i] != '\0' && path[i] != '\n') {
                needle[c] = path[i];
                c++;
            } else break;
        }
    }
    return needle;
}
