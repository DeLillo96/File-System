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
    void *childs[MAXCHILDS];
    int nChilds;
} directory;

int isAlphanumeric(char *);
int getCommand(char *);
char* substr(char *, int, int);
char* getNeedle(char *);
char* getText(char *);
directory * createFile(directory *, char *);
directory * createDirectory(directory *, char *);
void readFile(directory *, char *);
void writeFile(directory *, char *);

int main() {
    directory * root;
    char command[INPUTMAX];
    char * path;
    int ex = 0, nCommand;
    while (ex == 0) {
        /***DEBUG*****/
        file * f;
        for(int i=0; i<root->nChilds; i++) {
            f = (file *) root->childs[i];
            printf("%s | %s\n", f->name, f->text);
        }
        /*************/
        printf("$>");
        fgets (command, INPUTMAX, stdin);
        if( 1 == isAlphanumeric(command) ) {
            nCommand = getCommand(command);
            switch (nCommand) {
                case 1:
                    root = createDirectory(root, command);
                break;
                case 2:
                    root = createFile(root, command);
                break;
                case 3:
                    readFile(root, command);
                break;
                case 4:
                    writeFile(root, command);
                break;
                case 8: ex = 1;
            }
        } else {
            printf("no\n");
        }
    }
    return 0;
}

int isAlphanumeric(char * string) {
    int i;
    for (i = 0; i < INPUTMAX; i ++) {
        if (!(
            (string[i] >= 'A' && string[i] <= 'Z') ||
            (string[i] >= 'a' && string[i] <= 'z') ||
            (string[i] >= '0' && string[i] <= '9') ||
            string[i] == '_' ||
            string[i] == '/' ||
            string[i] == '"' ||
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

int getCommand(char * command) {
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

char* substr(char * string, int startIndex, int endIndex) {
    char * newString;
    int i,c = 0;

    newString = (char *)malloc((endIndex - startIndex)*sizeof(char));

    for(i = startIndex; i < endIndex; i ++) {
        newString[c] = string[i];
        c++;
    }

    return newString;
}

char* getNeedle(char * path) {
    char * needle;
    int i, c = 0;

    needle = (char *)malloc(MAXNAME*sizeof(char));

    if( path[0] == '/' ) {
        for (i = 1; i < MAXNAME; i ++) {
            if(path[i] != '/' && path[i] != '\0' && path[i] != '\n' && path[i] != ' ') {
                needle[c] = path[i];
                c++;
            } else break;
        }
    }
    return needle;
}

char* getText(char * path) {
    char * text;
    int i, c = 0, startRecord = 0;

    text = (char *)malloc(20*sizeof(char));

    for (i = 0; i < 20; i ++) {
        if(path[i] == '"') {
            if(startRecord == 1){
                break;
            } else {
                startRecord = 1;
                continue;
            }
        }
        if(startRecord == 1){
            text[c] = path[i];
            c++;
        }
    }
    return text;
}

directory* createFile(directory * fs, char * command) {
    directory * probeDir, *dirLine[MAXHEIGHT];
    file * newFile;
    char* needle;
    int commandLength, length = 7, i, dirIndex = 0, childIndexes[MAXHEIGHT];

    newFile = (file *) malloc(sizeof(file));
    probeDir = fs;
    dirLine[dirIndex] = fs;
    dirIndex++;

    commandLength = (strlen(command) - 1);
    needle = getNeedle(substr(command, length, strlen(command)));
    length += strlen(needle) + 1;

    while(length != commandLength) {
        for(i = 0; i < probeDir->nChilds; i++) {
            probeDir = (directory *) &probeDir->childs[i];
            if( 0 == strcmp(probeDir->name, needle)) {
                break;
            }
        }
        needle = getNeedle(substr(command, length, strlen(command)));
        length += strlen(needle) + 1;
    }

    strcpy(newFile->name, needle);
    strcpy(newFile->text, " ");

    probeDir->childs[ probeDir->nChilds ] = (void *) newFile;
    probeDir->nChilds++;

    for(i = (dirIndex - 1); i >= 0; i--) {
        dirLine[i] = probeDir;
    }

    return dirLine[0];
}

directory* createDirectory(directory * fs, char * command) {
    directory * probeDir, * dirLine[MAXHEIGHT];
    directory * newDir = (directory *) malloc(sizeof(directory));
    char* needle;
    int commandLength, length = 11, i, dirIndex = 0, childIndexes[MAXHEIGHT];

    probeDir = fs;
    dirLine[dirIndex] = fs;
    dirIndex++;

    commandLength = (strlen(command) - 1);
    needle = getNeedle(substr(command, length, strlen(command)));
    length += strlen(needle) + 1;

    while(length != commandLength) {
        for(i = 0; i < probeDir->nChilds; i++) {
            probeDir = (directory *) &probeDir->childs[i];
            if( 0 == strcmp(probeDir->name, needle)) {
                break;
            }
        }
        needle = getNeedle(substr(command, length, strlen(command)));
        length += strlen(needle) + 1;
    }

    strcpy(newDir->name, needle);

    probeDir->childs[ probeDir->nChilds ] = (void *) newDir;
    probeDir->nChilds++;

    for(i = (dirIndex - 1); i >= 0; i--) {
        dirLine[i] = probeDir;
    }

    return dirLine[0];
}

void readFile(directory * fs, char * command) {
    directory * probeDir;
    file * f;
    char* needle;
    int commandLength, length = 5, i;

    probeDir = fs;

    commandLength = (strlen(command) - 1);
    needle = getNeedle(substr(command, length, strlen(command)));
    length += strlen(needle) + 1;

    while(length != commandLength) {
        for(i = 0; i < probeDir->nChilds; i++) {
            probeDir = (directory *) &probeDir->childs[i];
            if( 0 == strncmp(probeDir->name, needle, 0)) {
                break;
            }
        }
        needle = getNeedle(substr(command, length, strlen(command)));
        length += strlen(needle) + 1;
    }

    for(i = 0; i < probeDir->nChilds; i++) {
        f = (file *) probeDir->childs[i];
        if( 0 == strcmp(f->name, needle)) {
            printf("%s\n", f->text);
            return NULL;
        }
    }
    printf("no\n");
}

void writeFile(directory * fs, char * command) {
    directory * probeDir;
    file * f;
    char* needle;
    char* text;
    int commandLength, length = 6, i;

    probeDir = fs;

    text = getText(command);

    commandLength = (strlen(command) - strlen(text) - 4);

    needle = getNeedle(substr(command, length, strlen(command)));
    length += strlen(needle) + 1;

    while(length != commandLength) {
        for(i = 0; i < probeDir->nChilds; i++) {
            probeDir = (directory *) &probeDir->childs[i];
            if( 0 == strncmp(probeDir->name, needle, 0)) {
                break;
            }
        }
        needle = getNeedle(substr(command, length, strlen(command)));
        length += strlen(needle) + 1;
    }

    for(i = 0; i < probeDir->nChilds; i++) {
        f = (file *) probeDir->childs[i];
        if( 0 == strcmp(f->name, needle)) {
            strcpy(f->text, text);
            printf("si\n");
            return NULL;
        }
    }
    printf("no\n");
}
