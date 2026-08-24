#ifndef TASK_H
#define TASK_H

#define MAX_TASKS 100
#define MAX_ARGS 50
#define MAX_JOBS 100

#include <sys/wait.h>
typedef struct task {
    char *nome;
    char *cmd;
    char *args[MAX_ARGS];
} task;

typedef struct {
    int id;
    pid_t pid;
    char *nome_task;
    int ativo;
} job;

void cadastrar_task(char *nome, char *cmd, char *args[]);

void executar_task(task *t);

void executar_paralelo(task *tasks[], int total);

void executar_pipe(task *tasks[], int total);

void executar_redirecionado(task *t, char *arquivo, char *modo);

void executar_background(task *t);

void listar_jobs();

void aguardar_job(int job_id);

task* buscar_task(char *nome);

#endif
