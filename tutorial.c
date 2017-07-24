#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUTMAX 65036
#define MAXNAME 255
#define MAXFILE 4096
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
char * substr(char *, int, int);
char * getNeedle(char *, int);
char * getText(char *);
element * getLastElement(element *, char *);
void * create(element *, char *, enum type_of_element);
void * readFile(element *, char *);
void * writeFile(element *, char *);
void * delete(element *, char *, int);
void * search(element, char *);
char * getPathInLeftTree(element);

int main() {
    element root;
    char command[INPUTMAX];
    char * path;
    int ex = 0, nCommand;
    root.type = is_dir;
    strcpy(root.name, "root");
    while (ex == 0) {
        if(NULL == fgets(command, INPUTMAX, stdin))continue;
        if(0 == strcmp(substr(command, 0, 10), "create_dir")){
            create(&root, substr(command, 11, strlen(command)-1), is_dir);
        } else {
            if(0 == strcmp(substr(command, 0, 6), "create")) create(&root, substr(command, 7, strlen(command)-1), is_file);
        }
        if(0 == strcmp(substr(command, 0, 4), "read")) readFile(&root, substr(command, 5, strlen(command)-1));
        if(0 == strcmp(substr(command, 0, 5), "write")) writeFile(&root, substr(command, 6, strlen(command)-1));
        if(0 == strcmp(substr(command, 0, 8), "delete_r")){
            delete(&root, substr(command, 9, strlen(command)-1), 1);
        } else {
            if(0 == strcmp(substr(command, 0, 6), "delete")) delete(&root, substr(command, 7, strlen(command)-1), 0);
        }
        if(0 == strcmp(substr(command, 0, 4), "find")) search(root, substr(command, 5, strlen(command)-1));
        if(0 == strcmp(substr(command, 0, 4), "exit")) {
            printf("ok\n");
            ex++;
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
            (string[i] >= '0' && string[i] <= '9')
        )) {
            if(string[i] == '\0' || string[i] == '\n')
                return 1;
            else
                return 0;
         }
    }
    return 1;
}

char * substr(char * string, int startIndex, int endIndex) {
    char * newString;
    int i,c = 0;

    newString = (char *)malloc((endIndex - startIndex)*sizeof(char));

    for(i = startIndex; i < endIndex; i ++) {
        if(newString[c] != ' ') {
            newString[c] = string[i];
            c++;
        }
    }

    return newString;
}

char * getNeedle(char * path, int reverse) {
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

    needle = getNeedle(path, 0);
    length = strlen(needle) + 1;

    if(length != strlen(path) && fs->type == is_dir) {
        for(i = 0; i < fs->nChilds; i++) {
            probeDir = (element *) fs->childs[i];
            if( 0 == strcmp(probeDir->name, needle)) {
                if(probeDir->type == is_dir){
                    return getLastElement(probeDir, substr(path, length, strlen(path)));
                }
            }
        }
    } else {
        if(length == strlen(path)) return fs;
    }
    return NULL;
}

void * create(element * fs, char * command, enum type_of_element el) {
    element * last = getLastElement(fs, command);
    element * new;
    char * needle;

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
needle = getNeedle(command, 1);
    for(int i = 0; i < last->nChilds; i++) {
        new = (element *) last->childs[i];
        if(new->type == el) {
            if( 0 == strcmp(new->name, needle)) {
                printf("no\n");
                return NULL;
            }
        }
    }
    new = (element *) malloc(sizeof(element));
    new->type = el;
    strcpy(new->name, needle);
    strcpy(new->text, " ");
    last->childs[ last->nChilds ] = (void *) new;
    last->nChilds++;
    printf("ok\n");
}

void * readFile(element * fs, char * command) {
    element * last = getLastElement(fs, substr(command, 0, strlen(command)));
    element * file;
    char* needle  = getNeedle(command, 1);

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    for(int i = 0; i < last->nChilds; i++) {
        file = (element *) last->childs[i];
        if(file->type == is_file) {
            if(0 == strcmp(file->name, needle)) {
                printf("contenuto %s\n", file->text);
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

    if(last == NULL) {
        printf("no\n");
        return NULL;
    }
    needle = getNeedle(command, 1);

    for(int i = 0; i < last->nChilds; i++) {
        file = (element *) last->childs[i];
        if(file->type == is_file){
            if( 0 == strcmp(file->name, needle)) {
                strcpy(file->text, text);
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
        printf("no\n");
        return NULL;
    }
    for(int i = 0; i < last->nChilds; i++) {
        el = (element *) last->childs[i];
        if(0 == strcmp(command, "/*")) {
            needle = el->name;
        }
        if(0 == strcmp(el->name, needle)) {
            if(el->type == is_dir) {
                if(el->nChilds > 0 && all == 0) return NULL;
                for(int j = 0; j < el->nChilds; j++) {
                    delete((element *)el->childs[j], "/*", 1);
                }
            }
            last->nChilds--;
            if(i != last->nChilds) {
                last->childs[i] = last->childs[last->nChilds];
            }
            free(el);
        }
    }
}

void * search(element fs, char * name) {
    element * probe;
    int pathIndex = 0, ex = 0, supportIndex = 0;
    char * paths[MAXHEIGHT*MAXNAME + MAXHEIGHT],  * support[MAXHEIGHT*MAXNAME + MAXHEIGHT], * stringProbe;

    while (ex == 0) {
        paths[pathIndex] = getPathInLeftTree(fs);
        probe = getLastElement(&fs, paths[pathIndex]);
        if(probe != NULL) {
            pathIndex++;
            probe->nChilds--;
            if(0 == strcmp(probe->name, "root") && 0 == probe->nChilds) ex++;
        }
    }
    for(int i = 0; i < pathIndex; i++) {
        if(0 == strcmp(name, substr(paths[i], strlen(paths[i]) - strlen(name), strlen(paths[i])))) {
            support[supportIndex] = paths[i];
            supportIndex++;
        }
    }
    if(supportIndex == 0) {
        printf("no\n");
        return NULL;
    }
    for(int i = 0; i < supportIndex; i++) {
        stringProbe = support[i];
        for(int j = (i + 1); j < supportIndex; j++) {
            if(1 == strcmp(stringProbe, support[j])) {
                support[i] = support[j];
                support[j] = stringProbe;
                stringProbe = support[i];
            }
        }
        printf("ok %s\n", support[i]);
    }
}

char * getPathInLeftTree(element fs) {
    char * path = (char *)malloc((MAXHEIGHT*MAXNAME + MAXHEIGHT)*sizeof(char));
    element el = *((element *) fs.childs[(fs.nChilds - 1)]);
    strcat(path, "/");
    strcat(path, el.name);
    if(el.type == is_dir) {
        if(el.nChilds > 0) {
            strcat(path, getPathInLeftTree(el));
        }
    }
    return path;
}
