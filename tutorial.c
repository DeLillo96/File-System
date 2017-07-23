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
char * substr(char *, int, int);
char * getNeedle(char *, int);
char * getText(char *);
directory * getLastElement(directory *, char *);
void * createFile(directory *, char *);
void * createDirectory(directory *, char *);
void * readFile(directory *, char *);
void * writeFile(directory *, char *);

int main() {
    directory root;
    char command[INPUTMAX];
    char * path;
    int ex = 0, nCommand;
    while (ex == 0) {
        printf("$>");
        fgets (command, INPUTMAX, stdin);
        if( 1 == isAlphanumeric(command) ) {
            nCommand = getCommand(command);
            switch (nCommand) {
                case 1:
                    createDirectory(&root, substr(command, 11, strlen(command)-1));
                break;
                case 2:
                    createFile(&root, substr(command, 7, strlen(command)-1));
                break;
                case 3:
                    readFile(&root, substr(command, 5, strlen(command)-1));
                break;
                case 4:
                    writeFile(&root, substr(command, 6, strlen(command)-1));
                break;
                case 5:
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

char* getNeedle(char * path, int reverse) {
    char * needle;
    int i, c = 0, startIndex = 0;

    needle = (char *)malloc(MAXNAME*sizeof(char));
    if(reverse == 1) {
        for(i = strlen(path); i>0; i--){
            if(path[i] == '/') {
                startIndex = i;
                break;
            }
        }
    }

    if( path[startIndex] == '/' ) {
        for (i = startIndex+1; i < MAXNAME; i ++) {
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

directory * getLastElement(directory * fs, char * path) {
    directory * probeDir;
    char * needle;
    int length , i;

    needle = getNeedle(path, 0);
    length = strlen(needle) + 1;

    if(length != strlen(path)) {
        for(i = 0; i < fs->nChilds; i++) {
            probeDir = (directory *) fs->childs[i];
            if( 0 == strcmp(probeDir->name, needle)) {
                return getLastElement(probeDir, substr(path, length, strlen(path)));
            }
        }
    } else {
        return (void *) fs;
    }
}

void * createFile(directory * fs, char * command) {
    directory * last = getLastElement(fs, command);
    file * newFile;
    char * needle;

    needle = getNeedle(command, 1);
    for(int i = 0; i < last->nChilds; i++) {
        newFile = (file *) last->childs[i];
        if( 0 == strcmp(newFile->name, needle)) {
            return NULL;
        }
    }
    newFile = (file *) malloc(sizeof(file));
    strcpy(newFile->name, needle);
    strcpy(newFile->text, " ");
    last->childs[ last->nChilds ] = (void *) newFile;
    last->nChilds++;
}

void * createDirectory(directory * fs, char * command) {
    directory * last = getLastElement(fs, command);
    directory * newDir;
    char * needle = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        newDir = (directory *) last->childs[i];
        if( 0 == strcmp(newDir->name, needle)) {
            return NULL;
        }
    }
    newDir = (directory *) malloc(sizeof(directory));
    strcpy(newDir->name, needle);
    last->childs[ last->nChilds ] = (void *) newDir;
    last->nChilds++;
}

void * readFile(directory * fs, char * command) {
    directory * last = getLastElement(fs, substr(command, 0, strlen(command)));
    file * wFile;
    char* needle  = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        wFile = (file *) last->childs[i];
        if( 0 == strcmp(wFile->name, needle)) {
            printf("%s\n", wFile->text);
            return NULL;
        }
    }
    printf("no\n");
}

void * writeFile(directory * fs, char * command) {
    file * wFile;
    char* needle, * text = getText(command);
    directory * last = getLastElement(fs, substr(command, 0, strlen(command) - strlen(text) - 4));

    needle = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        wFile = (file *) last->childs[i];
        if( 0 == strcmp(wFile->name, needle)) {
            strcpy(wFile->text, text);
            printf("si\n");
            return NULL;
        }
    }
    printf("no\n");
}

void * delete_r(directory * fs, char * command) {
    directory * last = getLastElement(fs, command);

}
