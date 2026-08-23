#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

task catalogo[MAX_TASKS];
int tarefas_totais = 0;

void cadastrar_task(char *nome, char *cmd, char *args[]) {

    if (tarefas_totais >= MAX_TASKS) {
        printf("Erro: Limite de tarefas atingido.\n");
        return;
    }

    catalogo[tarefas_totais].nome = strdup(nome);
    catalogo[tarefas_totais].cmd = strdup(cmd);

    int indice = 0;

    while (args[indice] != NULL && indice < MAX_ARGS - 1) {
        catalogo[tarefas_totais].args[indice] = strdup(args[indice]);
        indice++;
    }

    catalogo[tarefas_totais].args[indice] = NULL;

    tarefas_totais++;
}
