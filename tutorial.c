#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct element_str {
    char type;
    char *name;
    struct element_str **childs;
    int nChilds;
    char *text;
} element;

typedef struct searchList_str {
	char *text;
    struct searchList_str *next;
    struct searchList_str *prev;
} searchList;

searchList *searchPaths, *probeList;
element root;

char * substr(char * string, int offset) {
    char * newString;
    newString = (char *)malloc(offset + 1);
    memcpy(newString, string, offset);
    newString[offset] = '\0';
    return newString;
}

char * getLastNeedle(char * path) {
    short int i, lenPath = strlen(path);
    for(i = lenPath; path[i] != '/'; i--) {}
    i++;
    return substr(&path[i], (lenPath - i));
}

element * getLastElement(char * path) {
    element * probeDir , * fs = &root;
    char * needle, flag;
    short int length, lenPath = strlen(path), i;

    for(flag = 0;;flag = 0) {
        for (length = 1; path[length] != '\0' && path[length] != '/'; length++) lenPath--;
        lenPath --;
        if(lenPath > 0) {
            if(fs->type == 0) {
                needle = substr(&path[1], length - 1);
                for(i = 0; i < fs->nChilds; i++) {
                    probeDir = fs->childs[i];
                    if(0 == strcmp(probeDir->name, needle)) {
                        free(needle);
                        needle = substr(&path[length], lenPath);
                        free(path);
                        path = needle;
                        fs = probeDir;
                        flag = 1;
                        break;
                    }
                }
                if(flag == 1) continue;
                free(needle);
            }
            return NULL;
        } else {
            return fs;
        }
    }
}

void * create(char * command, char type) {
    char * needle = getLastNeedle(command);
    element * last = getLastElement(command);
    element * new, * elem;
    short int i;

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(i = 0; i < last->nChilds; i++) {
        elem = last->childs[i];
        if(elem->type == type) {
            if(0 == strcmp(elem->name, needle)) {
                printf("no\n");
                return NULL;
            }
        }
    }
    new = (element *)malloc(40);
    new->type = type;
    new->nChilds = 0;
    new->name = needle;
    new->text = NULL;
    if(last->nChilds > 0) {
        last->childs = (element**)realloc(last->childs, (last->nChilds + 1)*8);
    } else {
        last->childs = (element**)malloc(8);
    }
    last->childs[ last->nChilds ] = new;
    last->nChilds++;
    printf("ok\n");
    return NULL;
}

void * readFile(char * command) {
    char* needle  = getLastNeedle(command);
    element * last = getLastElement(command);
    element * el;
    short int i;

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(el->type == 1) {
            if(0 == strcmp(el->name, needle)) {
                if(el->text != NULL) printf("contenuto %s\n", el->text);
                else printf("contenuto \n");
                free(needle);
                return NULL;
            }
        }
    }
    free(needle);
    printf("no\n");
    return NULL;
}

void * writeFile(char * command) {
    element * el;
    short int strCmd = (strlen(command) - 2), i;
    for(i = strCmd; command[i] != '"'; i--){}
    char * text = substr(&command[i + 1], strCmd-i), * path = substr(command, i - 1), * needle= getLastNeedle(path);
    element * last = getLastElement(path);

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(el->type == 1) {
            if(0 == strcmp(el->name, needle)) {
                el->text = text;
                printf("ok %d\n", (int)strlen(text));
                free(needle);
                free(command);
                return NULL;
            }
        }
    }
    free(needle);
    free(command);
    free(text);
    printf("no\n");
    return NULL;
}

void * delete_r(element * fs) {
    if(fs->nChilds != 0){
        for(int i = 0; i < fs->nChilds; i++) {
            delete_r(fs->childs[i]);
        }
        free(fs->childs);
    }
    if(fs->text != NULL) free(fs->text);
    free(fs->name);
    free(fs);
}

void * delete(char * command, int all) {
    char * needle = getLastNeedle(command);
    element * el, * last = getLastElement(command);
    short int i;

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(0 == strcmp(el->name, needle)) {
            if(el->type == 0) {
                if(el->nChilds > 0 && all == 0) break;
                for(int j = 0; j < el->nChilds; j++) {
                    delete_r(el->childs[j]);
                }
            }
            last->nChilds--;
            if(i != last->nChilds) {
                last->childs[i] = last->childs[last->nChilds];
            }
            if(last->nChilds > 0) {
                last->childs = (element **)realloc(last->childs, last->nChilds*8);
            } else {
                free(last->childs);
            }
            free(el);
            free(needle);
            printf("ok\n");
            return NULL;
        }
    }
    printf("no\n");
}

int search(element * fs, char * path, char * name) {
    if(fs->nChilds == 0) return 0;
    char newPath[4095];
    searchList * newList = NULL;
    element * el;
    short int i, finishedFlag = 0;

    for (i = 0; i < fs->nChilds; i++) {
        strcpy(newPath, path);
        el = fs->childs[i];
        strcat(newPath, "/");
        strcat(newPath, el->name);
        if(0 == strcmp(name, el->name)){
            newList = (searchList *)malloc(24);
            probeList->text = (char *)malloc(strlen(newPath) + 1);
            strcpy(probeList->text, newPath);
            probeList->next = newList;
            newList->text = NULL;
            newList->next = NULL;
            newList->prev = probeList;
            probeList = newList;
            if(finishedFlag == 0) finishedFlag++;
        }
        if(el->type == 0 && el->nChilds > 0) {
            if(0 != search(el, newPath, name) && finishedFlag == 0) finishedFlag++;
        }
    }
    return finishedFlag;
}

int main() {
    char command[4000], * supportString;
    searchList *supportList;
    short int i;
    root.type = 0;
    for(;;) {
        fgets(command, 4000, stdin);
        if(0 == strncmp(command, "create", 6)) {
            if(0 == strncmp(command, "create_dir", 10)) {
                for(i = 11; command[i] == ' '; i++) {}
                create(substr(&command[i], strlen(command)-i-1), 0);
                continue;
            } else {
                for(i = 7; command[i] == ' '; i++) {}
                create(substr(&command[i], strlen(command)-i-1), 1);
                continue;
            }
        }
        if(0 == strncmp(command, "read", 4)) {
            for(i = 5; command[i] == ' '; i++) {}
            readFile(substr(&command[i], strlen(command)-i-1));
            continue;
        }
        if(0 == strncmp(command, "write", 5)) {
            for(i = 6; command[i] == ' '; i++) {}
            writeFile(substr(&command[i], strlen(command)-i-1));
            continue;
        }
        if(0 == strncmp(command, "delete", 6)) {
            if(0 == strncmp(command, "delete_dir", 10)) {
                for(i = 9; command[i] == ' '; i++) {}
                delete(substr(&command[i], strlen(command)-i-1), 1);
                continue;
            } else {
                for(i = 7; command[i] == ' '; i++) {}
                delete(substr(&command[i], strlen(command)-i-1), 0);
                continue;
            }
        }
        if(0 == strncmp(command, "find", 4)) {
            searchPaths = (searchList *)malloc(24);
            searchPaths->prev = NULL;
            probeList = searchPaths;
            for(i = 5; command[i] == ' '; i++) {}
            if(0 != search(&root, "", substr(&command[i], strlen(command)-i-1))) {
                supportList = searchPaths;
                for(;supportList->text != NULL;) {
                    probeList = supportList->next;
                    for(;probeList->text != NULL;) {
                        if(strcmp(supportList->text, probeList->text) > 0) {
                            supportString = supportList->text;
                            supportList->text = probeList->text;
                            probeList->text = supportString;
                        }
                        probeList = probeList->next;
                    }
                    printf("ok %s\n", supportList->text);
                    free(supportList->text);
                    supportList = supportList->next;
                    free(supportList->prev);
                }
            } else printf("no\n");
            continue;
        }
        if(0 == strncmp(command, "exit", 4)) exit(0);
    }
}
