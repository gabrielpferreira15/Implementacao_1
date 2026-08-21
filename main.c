#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    while (1){
        char entrada[1024];

        printf("processflow> ");

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        if((strcmp(entrada, "exit") == 0)){ 
            break;
        }
    }

    return 0;
}