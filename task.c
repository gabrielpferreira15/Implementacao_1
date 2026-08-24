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

void executar_paralelo(task *tasks[], int total) {
    pid_t pids[MAX_TASKS];
    int pids_criados = 0;

    for (int i = 0; i < total; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Erro ao executar o fork");
            continue;
        }
        if (pid == 0) {
            execvp(tasks[i]->cmd, tasks[i]->args);
            perror("Erro ao executar a tarefa em paralelo");
            exit(EXIT_FAILURE);
        }
        pids[pids_criados] = pid;
        pids_criados++;
    }

    for (int i = 0; i < pids_criados; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("Erro ao aguardar um ou mais processos");
            continue;
        }
        
        if (WIFEXITED(status)) {
            int codigo_saida = WEXITSTATUS(status);
            if (codigo_saida != 0) {
                printf("Aviso: Tarefa paralela encerrou com código de erro %d.\n", codigo_saida);
            }
        } else if (WIFSIGNALED(status)) {
            printf("Aviso: Tarefa paralela interrompida pelo sinal %d.\n", WTERMSIG(status));
        }
    }
}

void executar_pipe(task *tasks[], int total) {
    int fd_anterior = 0;
    pid_t pids[MAX_TASKS];

    for (int i = 0; i < total; i++) {
        int fd_atual[2];

        pids[i] = fork();

        if (pids[i] == 0) {
            if (fd_anterior != 0) {
                dup2(fd_anterior, 0);
                close(fd_anterior);
            }

            if (i < total - 1) {
                dup2(fd_atual[1], 1);
                close(fd_atual[0]);
                close(fd_atual[1]); 
            }

            execvp(tasks[i]->cmd, tasks[i]->args);

        } else {
            if (fd_anterior != 0) {
                close(fd_anterior);
            }

            if (i < total - 1) {
                fd_anterior = fd_atual[0];
                close(fd_atual[1]); 
            }
        }
    }

    for (int i = 0; i < total; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}