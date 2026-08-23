#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
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

task* buscar_task(char *nome) {
    
    for (int i = 0; i < tarefas_totais; i++) {
        if (strcmp(catalogo[i].nome, nome) == 0) {
            return &catalogo[i];
        }
    }
    return NULL;
}

void executar_task(task *t){
    pid_t pid = fork();

    if (pid < 0){
        perror("Erro ao executar o fork");
        return;
    }

    if (pid == 0) {
        execvp(t->cmd, t->args);

        perror("Erro ao executar a tarefa");

        exit(EXIT_FAILURE);
    } 
    else {
        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("Erro ao aguardar o processo filho");
            return;
        }

        if (WIFEXITED(status)) {
            int codigo_saida = WEXITSTATUS(status);
            if (codigo_saida != 0) {
                printf("Aviso: Tarefa encerrou com código de erro %d.\n", codigo_saida);
            }
        } else if (WIFSIGNALED(status)) {
            printf("Aviso: Tarefa interrompida abruptamente pelo sinal %d.\n", WTERMSIG(status));
        } 
    }
}