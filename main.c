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

        char *comando = strtok(entrada, " ");

        if(comando == NULL){
            continue;
        }

        if((strcmp(comando, "exit") == 0)){ 
            break;
        }

        printf("Comando detectado: %s\n", comando);

        char *argumento = strtok(NULL, " ");

        while (argumento != NULL) {

            printf("Comando detectado: %s\n", argumento);

            argumento = strtok(NULL, " ");

        }
    }

    return 0;
}