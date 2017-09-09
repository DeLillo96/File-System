#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUTMAX 50036
#define MAXNAME 255
#define MAXFILE 1024
#define MAXCHILDS 1024
#define MAXHEIGHT 255

enum type_of_element {dir = 0, file = 1};
enum color_of_element {white = 0, black = 1};

typedef struct {
    enum type_of_element type;
    enum type_of_element color;
    char name[MAXNAME];
    void *childs[MAXCHILDS];
    int nChilds;
    char text[MAXFILE];
} element;

char * substr(char *, int, int);
char * getNeedle(char *, int);
char * getText(char *);
element * getLastElement(element *, char *);
void * create(element *, char *, enum type_of_element);
void * readFile(element *, char *);
void * writeFile(element *, char *);
void * delete(element *, char *, int);
int search(element *, char *, char *);

enum type_of_element probeColor = white;

int main() {
    element root;
    char command[INPUTMAX], * testStr;
    int ex = 0;
    root.type = dir;
    root.color = probeColor;
    strcpy(root.name, "root");
    while (ex == 0) {
        if(NULL == fgets(command, INPUTMAX, stdin))continue;
        if(0 == strcmp(substr(command, 0, 10), "create_dir")) {
            create(&root, substr(command, 10, strlen(command)), dir);
        } else {
            if(0 == strcmp(substr(command, 0, 6), "create")) create(&root, substr(command, 6, strlen(command)), file);
        }
        if(0 == strcmp(substr(command, 0, 4), "read")) readFile(&root, substr(command, 4, strlen(command)));
        if(0 == strcmp(substr(command, 0, 5), "write")) writeFile(&root, substr(command, 5, strlen(command)));
        if(0 == strcmp(substr(command, 0, 8), "delete_r")) {
            delete(&root, substr(command, 8, strlen(command)), 1);
        } else {
            if(0 == strcmp(substr(command, 0, 6), "delete")) delete(&root, substr(command, 6, strlen(command)), 0);
        }
        if(0 == strcmp(substr(command, 0, 4), "find")) {
            if(0 == search(&root, "", substr(command, 4, strlen(command)))) printf("no\n");
        }
        if(0 == strcmp(substr(command, 0, 4), "exit")) ex++;
    }
    return 0;
}

char * substr(char * string, int startIndex, int endIndex) {
    char * newString = (char *)malloc((endIndex - startIndex)*sizeof(char)), * other;
    int i, c = 0;

    for(i = startIndex; i < endIndex; i ++) {
            if(string[i] != '\n' && string[i] != ' ') {
                newString[c] = string[i];
                c++;
            }
            if(string[i] == '\0') break;
    }
    other = (char *)malloc((c+1)*sizeof(char));
    for(i = 0; i < c; i++) {
        other[i] = newString[i];
    }
    other[i] = '\0';
    return other;
}

char * getNeedle(char * path, int reverse) {
    int i, c = 0, startIndex = 0;

    if(reverse == 1) {
        for(i = strlen(path); i > 0; i--){
            if(path[i] == '/') {
                startIndex = i;
                break;
            }
        }
    }
    if(path[startIndex] == '/') {
        for (i = (startIndex + 1); i < strlen(path); i ++) {
            if(path[i] == '\0' || path[i] == '/') break;
        }
    } else return NULL;
    return substr(path, (startIndex + 1), i);
}

char * getText(char * path) {
    char * text;
    int i, c = 0, startRecord = 0;

    text = (char *)malloc(MAXFILE*sizeof(char));

    for (i = 0; i < MAXFILE; i ++) {
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

    if(path == NULL || 0 == strcmp(path, "\0")) return fs;
    needle = getNeedle(path, 0);
    length = strlen(needle) + 1;

    if(length != strlen(path)) {
        if(fs->type == dir) {
            for(i = 0; i < fs->nChilds; i++) {
                probeDir = (element *) fs->childs[i];
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
        elem = (element *) last->childs[i];
        if(elem->type == el) {
            if(0 == strcmp(elem->name, needle)) {
                printf("no\n");
                return NULL;
            }
        }
    }
    new = (element *) malloc(sizeof(element));
    new->type = el;
    new->color = probeColor;
    strcpy(new->name, needle);
    strcpy(new->text, "\0");
    last->childs[ last->nChilds ] = (void *) new;
    last->nChilds++;
    for(int i = 0; i < last->nChilds; i++){
        elem = (element *) last->childs[i];
        for(int j = (i + 1); j < last->nChilds; j++) {
            new = (element *) last->childs[j];
            if(strcmp(elem->name, new->name) > 0) {
                last->childs[i] = last->childs[j];
                last->childs[j] = (void *) elem;
                elem = (element *) last->childs[i];
            }
        }
    }
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
        el = (element *) last->childs[i];
        if(el->type == file) {
            if(0 == strcmp(el->name, needle)) {
                if(0 != strcmp("\0", el->text)){
                    printf("contenuto %s\n", el->text);
                } else {
                    printf("contenuto \n");
                }
                return NULL;
            }
        }
    }
    printf("no\n");
}

void * writeFile(element * fs, char * command) {
    element * el;
    char * needle, * text = getText(command), * path = substr(command, 0, strlen(command) - strlen(text) - 2);
    element * last = getLastElement(fs, path);

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    needle = getNeedle(path, 1);
    for(int i = 0; i < last->nChilds; i++) {
        el = (element *) last->childs[i];
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
    element * el, * last = getLastElement(fs, command);
    char * needle = getNeedle(command, 1);

    if(last == NULL) {
        if(0 != strcmp(command, "/*")) printf("no\n");
        return NULL;
    }
    for(int i = 0; i < last->nChilds; i++) {
        el = (element *) last->childs[i];
        if(0 == strcmp(command, "/*")) {
            needle = el->name;
        }
        if(0 == strcmp(el->name, needle)) {
            if(el->type == dir) {
                if(el->nChilds > 0 && all == 0) return NULL;
                for(int j = 0; j < el->nChilds; j++) {
                    delete((element *)el->childs[j], "/*", 1);
                }
            }
            last->nChilds--;
            if(i != last->nChilds) {
                last->childs[i] = last->childs[last->nChilds];
            }
            if(last->nChilds != 0) free(el);
            if(0 != strcmp(command, "/*")){
                printf("ok\n");
            }
            return NULL;
        }
    }
    if(0 != strcmp(command, "/*")){
        printf("no\n");
    }
}

int search(element * fs, char * path, char * name) {
    char * newPath = (char *)malloc((MAXHEIGHT*MAXNAME + MAXHEIGHT)*sizeof(char));
    element * el;
    int i, finishedFlag = 0;

    for (i = 0; i < fs->nChilds; i++) {
        strcpy(newPath, path);
        el = (element *) fs->childs[i];
        strcat(newPath, "/");
        strcat(newPath, el->name);
        if(0 == strcmp(name, el->name)){
            printf("si %s\n", newPath);
            if(finishedFlag == 0) finishedFlag++;
        }
        if(el->type == dir && el->nChilds > 0) {
            if(0 != search(el, newPath, name) && finishedFlag == 0) finishedFlag++;
        }
    }
    return finishedFlag;
}
