#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "task.h"

task catalogo[MAX_TASKS];
int tarefas_totais = 0;

job lista_jobs[MAX_JOBS];
int total_jobs = 0;

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
    catalogo[tarefas_totais].arq_in = NULL;
    catalogo[tarefas_totais].arq_out = NULL;
    catalogo[tarefas_totais].arq_append = NULL;

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
        configurar_redirecionamento(t);

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

        if (i < total - 1) {
            if (pipe(fd_atual) < 0) {
                perror("Erro ao criar pipe");
                return;
            }
        }
        pids[i] = fork();

        if (pids[i] == 0) {
            if (fd_anterior != 0) {
                if (dup2(fd_anterior, 0) < 0) {
                    perror("Erro no dup2 para entrada do pipe");
                    close(fd_anterior);
                    exit(EXIT_FAILURE);
                }
                close(fd_anterior);
            }

            if (i < total - 1) {
                if (dup2(fd_atual[1], 1) < 0) {
                    perror("Erro no dup2 para saída do pipe");
                    close(fd_atual[0]);
                    close(fd_atual[1]);
                    exit(EXIT_FAILURE);
                }
                close(fd_atual[0]);
                close(fd_atual[1]); 
            }

            execvp(tasks[i]->cmd, tasks[i]->args);
            perror("Erro no exec do pipe");
            exit(EXIT_FAILURE);
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
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("Erro ao aguardar processo no pipe");
            continue;
        }

        if (WIFEXITED(status)) {
            int codigo_saida = WEXITSTATUS(status);
            if (codigo_saida != 0) {
                printf("Aviso: A tarefa '%s' no pipe encerrou com código %d.\n", tasks[i]->nome, codigo_saida);
            }
        } else if (WIFSIGNALED(status)) {
            printf("Aviso: A tarefa '%s' no pipe foi interrompida pelo sinal %d.\n", tasks[i]->nome, WTERMSIG(status));
        }
    }
}

void configurar_redirecionamento(task *t) {
    if (t->arq_in != NULL) {
        int fd = open(t->arq_in, O_RDONLY);
        if (fd < 0) { perror("Erro entrada"); exit(EXIT_FAILURE); }
        dup2(fd, 0);
        close(fd);
    }
    
    if (t->arq_out != NULL) {
        int fd = open(t->arq_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("Erro saída"); exit(EXIT_FAILURE); }
        dup2(fd, 1);
        close(fd);
    }

    if (t->arq_append != NULL) {
        int fd = open(t->arq_append, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) { perror("Erro append"); exit(EXIT_FAILURE); }
        dup2(fd, 1);
        close(fd);
    }
}

void executar_background(task *t) {
    if (total_jobs >= MAX_JOBS) {
        fprintf(stderr, "Erro: Limite de jobs em background atingido.\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Erro no fork para background");
        return;
    }

    if (pid == 0) {
        int dev_null = open("/dev/null", O_RDONLY);
        if (dev_null != -1) {
            dup2(dev_null, 0);
            close(dev_null);
        }

        execvp(t->cmd, t->args);
        perror("Erro ao executar tarefa em background");
        exit(EXIT_FAILURE);
    } else {
        total_jobs++;
        lista_jobs[total_jobs - 1].id = total_jobs;
        lista_jobs[total_jobs - 1].pid = pid;
        lista_jobs[total_jobs - 1].nome_task = strdup(t->nome);
        lista_jobs[total_jobs - 1].ativo = 1;

        printf("[%d] %d\n", total_jobs, pid);
    }
}

void listar_jobs() {
    int encontrou = 0;
    for (int i = 0; i < total_jobs; i++) {
        if (lista_jobs[i].ativo) {
            int status;
            pid_t res = waitpid(lista_jobs[i].pid, &status, WNOHANG);

            if (res == 0) {
                printf("[%d] %d RUNNING %s\n", lista_jobs[i].id, lista_jobs[i].pid, lista_jobs[i].nome_task);
                encontrou = 1;
            } else if (res > 0) {
                printf("[%d] %d DONE    %s\n", lista_jobs[i].id, lista_jobs[i].pid, lista_jobs[i].nome_task);
                lista_jobs[i].ativo = 0;
                encontrou = 1;
            } else {
                lista_jobs[i].ativo = 0;
            }
        }
    }
    if (!encontrou) {
        printf("Nenhum job em background no momento.\n");
    }
}

void aguardar_job(int job_id) {
    int indice = -1;
    for (int i = 0; i < total_jobs; i++) {
        if (lista_jobs[i].id == job_id && lista_jobs[i].ativo) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        fprintf(stderr, "Erro: job informado não existe (%d).\n", job_id);
        return;
    }

    int status;
    if (waitpid(lista_jobs[indice].pid, &status, 0) == -1) {
        perror("Erro ao aguardar job");
        return;
    }

    lista_jobs[indice].ativo = 0;
    printf("Job [%d] finalizado.\n", job_id);
}
