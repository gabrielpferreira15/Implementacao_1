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

            if (tarefas_totais >= 100) {
            printf("Erro: Limite de tarefas atingido.\n");
            continue;
            }

            char *token_nome = strtok(NULL, " ");
            if (token_nome == NULL) {
                printf("Erro: Faltando nome da tarefa.\n");
                continue;
            }

            catalogo[tarefas_totais].nome = strdup(token_nome);

            char *token_cmd = strtok(NULL, " ");
            if (token_cmd == NULL) {
                printf("Erro: Faltando programa da tarefa.\n");
                continue;
            }

            catalogo[tarefas_totais].cmd = strdup(token_cmd);

            catalogo[tarefas_totais].args[0] = strdup(token_cmd);

            int indice = 1;
            char *argumento = strtok(NULL, " ");

            while(argumento != NULL && indice < 49){
                catalogo[tarefas_totais].args[indice] = strdup(argumento);
                indice++;
                argumento = strtok(NULL, " ");
            }

            catalogo[tarefas_totais].args[indice] = NULL;

            tarefas_totais++;
        }
    }

    return 0;
}