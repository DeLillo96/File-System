#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUTMAX 66036
#define MAXNAME 255
#define MAXFILE 1024
#define MAXCHILDS 1024
#define MAXHEIGHT 255

enum type_of_element {dir = 0, file = 1};

typedef struct element_str {
    enum type_of_element type;
    char name[MAXNAME];
    struct element_str *childs[MAXCHILDS];
    int nChilds;
    char text[MAXFILE];
} element;

typedef struct searchList_str {
	char *text;
    struct searchList_str *next;
    struct searchList_str *prev;
} searchList;

searchList *searchPaths, *probeList;

char * substr(char * string, int startIndex, int endIndex) {
    char * newString;
    int i, c = 0;

    while (string[startIndex] == ' ') {
        startIndex++;
    }
    if(startIndex < endIndex) {
        newString = (char *)malloc(endIndex - startIndex + 1);
        memcpy(newString, &string[startIndex], endIndex - startIndex);
        newString[endIndex - startIndex + 1] = '\0';
        return newString;
    } else return NULL;
}

char * getNeedle(char * path, int reverse) {
    int i, c = 0, startIndex = 0;

    if(reverse == 1) {
        for(i = strlen(path); i > 0; i--){
            if(path[i] == '/') {
                startIndex = i;
                return substr(path, (i + 1), strlen(path));
            }
        }
    }
    for (i = (startIndex + 1); i < strlen(path); i ++) {
            if(path[i] == '\0' || path[i] == '/') break;
    }
    return substr(path, (startIndex + 1), i);
}

char * getText(char * path) {
    char * text;
    int startRecord = 0, startIndex = 0, endIndex = strlen(path);

    for (int i = (endIndex - 1); i > 0; i --) {
        if(path[i] == '"') {
            if(startRecord == 1){
                startIndex = i+ 1;
                break;
            } else {
                endIndex = i;
                startRecord = 1;
                continue;
            }
        }
    }
    return substr(path, startIndex, endIndex);
}

element * getLastElement(element * fs, char * path) {
    element * probeDir;
    char * needle;
    int length , i;

    if(path == NULL) return fs;
    needle = getNeedle(path, 0);
    length = strlen(needle) + 1;

    if(length != strlen(path)) {
        if(fs->type == dir) {
            for(i = 0; i < fs->nChilds; i++) {
                probeDir = fs->childs[i];
                if(0 == strcmp(probeDir->name, needle)) {
                    return getLastElement(probeDir, substr(path, length, strlen(path)));
                }
            }
        }
        return NULL;
    } else {
        return fs;
    }
}

void * create(element * fs, char * command, enum type_of_element el) {
    element * last = getLastElement(fs, command);
    element * new, * elem;
    char * needle;

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    needle = getNeedle(command, 1);
    for(int i = 0; i < last->nChilds; i++) {
        elem = last->childs[i];
        if(elem->type == el) {
            if(0 == strcmp(elem->name, needle)) {
                printf("no\n");
                return NULL;
            }
        }
    }
    new = (element *)malloc(sizeof(element));
    new->type = el;
    strcpy(new->name, needle);
    strcpy(new->text, "\0");
    last->childs[ last->nChilds ] = new;
    last->nChilds++;
    printf("ok\n");
}

void * readFile(element * fs, char * command) {
    element * last = getLastElement(fs, command);
    element * el;
    char* needle  = getNeedle(command, 1);

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(int i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(el->type == file) {
            if(0 == strcmp(el->name, needle)) {
                printf("contenuto %s\n", el->text);
                return NULL;
            }
        }
    }
    printf("no\n");
}

void * writeFile(element * fs, char * command) {
    element * el;
    char * needle, * text = getText(command), * path = substr(command, 0, strlen(command) - strlen(text) - 3);
    element * last = getLastElement(fs, path);

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    needle = getNeedle(path, 1);
    for(int i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(el->type == file) {
            if(0 == strcmp(el->name, needle)) {
                strcpy(el->text, text);
                printf("ok %d\n", (int)strlen(text));
                return NULL;
            }
        }
    }
    printf("no\n");
}

void * delete(element * fs, char * command, int all) {
    element * el, * last;
    char * needle;

    if(command != NULL) {
        last = getLastElement(fs, command);
        if(last == NULL) {
            printf("no\n");
            return NULL;
        }
        needle = getNeedle(command, 1);
    } else last = fs;

    for(int i = 0; i < last->nChilds; i++) {
        el = last->childs[i];
        if(command == NULL) needle = el->name;
        if(0 == strcmp(el->name, needle)) {
            if(el->type == dir) {
                if(el->nChilds > 0 && all == 0) return NULL;
                for(int j = 0; j < el->nChilds; j++) {
                    delete(el->childs[j], NULL, 1);
                }
            }
            last->nChilds--;
            if(i != last->nChilds) {
                last->childs[i] = last->childs[last->nChilds];
            }
            if(last->nChilds != 0) free(el);
            if(command != NULL){
                printf("ok\n");
            }
            return NULL;
        }
    }
    if(command != NULL) {
        printf("no\n");
    }
}

int search(element * fs, char * path, char * name) {
    char newPath[MAXHEIGHT*MAXNAME + MAXHEIGHT];
    searchList * newList = NULL;
    element * el;
    int i, finishedFlag = 0;

    for (i = 0; i < fs->nChilds; i++) {
        strcpy(newPath, path);
        el = fs->childs[i];
        strcat(newPath, "/");
        strcat(newPath, el->name);
        if(0 == strcmp(name, el->name)){
            newList = (searchList *)malloc(sizeof(searchList));
            probeList->text = (char *)malloc(strlen(newPath) + 1);
            strcpy(probeList->text, newPath);
            probeList->next = newList;
            newList->text = NULL;
            newList->next = NULL;
            newList->prev = probeList;
            probeList = newList;
            if(finishedFlag == 0) finishedFlag++;
        }
        if(el->type == dir && el->nChilds > 0) {
            if(0 != search(el, newPath, name) && finishedFlag == 0) finishedFlag++;
        }
    }
    return finishedFlag;
}

int main() {
    element root;
    char command[INPUTMAX], * supportString;
    searchList *supportList1, *supportList2;
    root.type = dir;
    strcpy(root.name, "root");
    while (1) {
        if(NULL == fgets(command, INPUTMAX, stdin))continue;
        if(0 == strncmp(command, "create_dir", 10)) {
            create(&root, substr(command, 10, strlen(command) - 1), dir);
        } else {
            if(0 == strncmp(command, "create", 6)) create(&root, substr(command, 6, strlen(command) - 1), file);
        }
        if(0 == strncmp(command, "read", 4)) readFile(&root, substr(command, 4, strlen(command) - 1));
        if(0 == strncmp(command, "write", 5)) writeFile(&root, substr(command, 5, strlen(command) - 1));
        if(0 == strncmp(command, "delete_r", 8)) {
            delete(&root, substr(command, 8, strlen(command) - 1), 1);
        } else {
            if(0 == strncmp(command, "delete", 6)) delete(&root, substr(command, 6, strlen(command) - 1), 0);
        }
        if(0 == strncmp(command, "find", 4)) {
            searchPaths = (searchList *)malloc(sizeof(searchList));
            searchPaths->prev = NULL;
            probeList = searchPaths;
            if(0 != search(&root, "", substr(command, 4, strlen(command) - 1))) {
                supportList1 = searchPaths;
                while(supportList1->text != NULL) {
                    supportList2 = supportList1->next;
                    while(supportList2->text != NULL) {
                        if(strcmp(supportList1->text, supportList2->text) > 0) {
                            supportString = supportList1->text;
                            supportList1->text = supportList2->text;
                            supportList2->text = supportString;
                        }
                        supportList2 = supportList2->next;
                    }
                    printf("ok %s\n", supportList1->text);
                    free(supportList1->text);
                    supportList1 = supportList1->next;
                    free(supportList1->prev);
                }
            } else printf("no\n");
        }
        if(0 == strncmp(command, "exit", 4)) break;
    }
    return 0;
}
