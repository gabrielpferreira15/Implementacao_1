#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct task{
    char *nome;
    char *cmd;
    char *args[50];
} task;

int main(){
    task catalogo[100];
    int tarefas_totais = 0;
    int indice = 0;

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

        if((strcmp(comando, "task") == 0)){ 

            char *argumento = strtok(NULL, " ");

            catalogo[tarefas_totais].nome = strdup(argumento);

            argumento = strtok(NULL, " ");

            catalogo[tarefas_totais].cmd = strdup(argumento);

            argumento = strtok(NULL, " ");

            catalogo[tarefas_totais].args[indice] = strdup(argumento);
            indice++;

            argumento = strtok(NULL, " ");

            while(argumento != NULL){
                catalogo[tarefas_totais].args[indice] = strdup(argumento);

                argumento = strtok(NULL, " ");
                indice++;
            }
            indice = 0;
            tarefas_totais++;
        }
    }

    return 0;
}