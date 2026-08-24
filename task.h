#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 100
#define MAX_ARGS 50

typedef struct task {
    char *nome;
    char *cmd;
    char *args[MAX_ARGS];
} task;

void cadastrar_task(char *nome, char *cmd, char *args[]);

void executar_task(task *t);

void executar_paralelo(task *tasks[], int total);

void executar_pipe(task *tasks[], int total);

task* buscar_task(char *nome);

#endif
