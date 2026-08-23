#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "task.h"

int main() {

    while (1) {
        char entrada[1024];

        printf("processflow> ");

        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        char *comando = strtok(entrada, " ");

        if (comando == NULL) {
            continue;
        }

        if (strcmp(comando, "exit") == 0) {
            break;
        }

        if (strcmp(comando, "task") == 0) {

            char *token_nome = strtok(NULL, " ");

            if (token_nome == NULL) {
                printf("Erro: Faltando nome da tarefa.\n");
                continue;
            }

            char *token_cmd = strtok(NULL, " ");

            if (token_cmd == NULL) {
                printf("Erro: Faltando programa da tarefa.\n");
                continue;
            }

            char *args[MAX_ARGS];

            args[0] = token_cmd;

            int indice = 1;
            char *argumento = strtok(NULL, " ");

            while (argumento != NULL && indice < MAX_ARGS - 1) {
                args[indice] = argumento;
                indice++;

                argumento = strtok(NULL, " ");
            }

            args[indice] = NULL;

            cadastrar_task(token_nome, token_cmd, args);
        }
    }

    return 0;
}