#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUTMAX 65036
#define MAXNAME 255
#define MAXFILE 8192
#define MAXCHILDS 1024
#define MAXHEIGHT 255

enum type_of_element {is_dir = 0, is_file = 1};

typedef struct {
    enum type_of_element type;
    char name[MAXNAME];
    void *childs[MAXCHILDS];
    int nChilds;
    char text[MAXFILE];
} element;

int isAlphanumeric(char *);
int getCommand(char *);
char * substr(char *, int, int);
char * getNeedle(char *, int);
char * getText(char *);
element * getLastElement(element *, char *);
void * createFile(element *, char *);
void * createDirectory(element *, char *);
void * readFile(element *, char *);
void * writeFile(element *, char *);
void * delete_r(element *, char *);

int main() {
    element root;
    char command[INPUTMAX];
    char * path;
    int ex = 0, nCommand;
    root.type = is_dir;
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
                    delete_r(&root, substr(command, 7, strlen(command)-1));
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

element * getLastElement(element * fs, char * path) {
    element * probeDir;
    char * needle;
    int length , i;

    needle = getNeedle(path, 0);
    length = strlen(needle) + 1;

    if(length != strlen(path)) {
        for(i = 0; i < fs->nChilds; i++) {
            probeDir = (element *) fs->childs[i];
            if( 0 == strcmp(probeDir->name, needle)) {
                return getLastElement(probeDir, substr(path, length, strlen(path)));
            }
        }
    } else {
        return fs;
    }
}

void * createFile(element * fs, char * command) {
    element * last = getLastElement(fs, command);
    element * newFile;
    char * needle;

    needle = getNeedle(command, 1);
    for(int i = 0; i < last->nChilds; i++) {
        newFile = (element *) last->childs[i];
        if(newFile->type == is_file) {
            if( 0 == strcmp(newFile->name, needle)) {
                return NULL;
            }
        }
    }
    newFile = (element *) malloc(sizeof(element));
    newFile->type = is_file;
    strcpy(newFile->name, needle);
    strcpy(newFile->text, " ");
    last->childs[ last->nChilds ] = (void *) newFile;
    last->nChilds++;
}

void * createDirectory(element * fs, char * command) {
    element * last = getLastElement(fs, command);
    element * newDir;
    char * needle = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        newDir = (element *) last->childs[i];
        if(newDir->type = is_dir) {
            if( 0 == strcmp(newDir->name, needle)) {
                return NULL;
            }
        }
    }
    newDir = (element *) malloc(sizeof(element));
    newDir->type = is_dir;
    strcpy(newDir->name, needle);
    last->childs[ last->nChilds ] = (void *) newDir;
    last->nChilds++;
}

void * readFile(element * fs, char * command) {
    element * last = getLastElement(fs, substr(command, 0, strlen(command)));
    element * file;
    char* needle  = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        file = (element *) last->childs[i];
        if(file->type == is_file) {
            if(0 == strcmp(file->name, needle)) {
                printf("%s\n", file->text);
                return NULL;
            }
        }
    }
    printf("no\n");
}

void * writeFile(element * fs, char * command) {
    element * file;
    char * needle, * text = getText(command);
    element * last = getLastElement(fs, substr(command, 0, strlen(command) - strlen(text) - 4));

    needle = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        file = (element *) last->childs[i];
        if(file->type == is_file){
            if( 0 == strcmp(file->name, needle)) {
                strcpy(file->text, text);
                printf("si\n");
                return NULL;
            }
        }
    }
    printf("no\n");
}

void * delete_r(element * fs, char * command) {
    element * last = getLastElement(fs, command);
    char * needle = getNeedle(command, 1);
    element * el;

    for(int i = 0; i < last->nChilds; i++) {
        el = (element *) last->childs[i];
        if(el->type = is_dir) {

        }
    }
}
