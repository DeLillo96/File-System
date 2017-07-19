#include <stdio.h>

#define INPUTMAX 65036

int main(){
    char command[INPUTMAX];
    int exit = 0, nCommand;
    while (exit == 0) {
        printf("$>");
        scanf("%s", &command);
        if( 1 == isAlphanumeric(command) ) {
            nCommand = getCommand(command);
            switch (nCommand) {
                case 8: exit = 1;
            }
        } else {
            printf("no\n");
        }
    }
    return 0;
}

int isAlphanumeric( char * string )
{
    int i;
    for (i = 0; i < INPUTMAX; i ++) {
        if (!(
            (string[i] >= 'A' && string[i] <= 'Z') ||
            (string[i] >= 'a' && string[i] <= 'z') ||
            (string[i] >= '0' && string[i] <= '9') ||
            string[i] == '_'
        )) {
            if(string[i] == '\0')
                return 1;
            else
                return 0;
         }
    }
    return 1;
}

int getCommand( char * command )
{
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
