#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "task.h"

int main(int argc, char *argv[]) {
    FILE *arquivo = stdin;
    int modo_interativo = 1;

    if (argc > 2) {
        fprintf(stderr, "Erro: Número incorreto de argumentos. Use: ./processflow [arquivo].pf\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        arquivo = fopen(argv[1], "r");
        if (arquivo == NULL) {
            fprintf(stderr, "Erro: Não foi possível abrir o arquivo workflow '%s'.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        modo_interativo = 0;
    }

    while (1) {
        char entrada[1024];

        if (modo_interativo) {
            printf("processflow> ");
            fflush(stdout); 
        }

        if (fgets(entrada, sizeof(entrada), arquivo) == NULL) {
            break;
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        if (!modo_interativo) {
            printf("%s\n", entrada);
        }

        char *comando = strtok(entrada, " \t\r\n");

        if (comando == NULL) {
            continue;
        }

        if (strcmp(comando, "exit") == 0) {
            break;
        }

        else if (strcmp(comando, "task") == 0) {

            char *token_nome = strtok(NULL, " \t\r\n");

            if (token_nome == NULL) {
                printf("Erro: Faltando nome da tarefa.\n");
                continue;
            }

            char *token_cmd = strtok(NULL, " \t\r\n");

            if (token_cmd == NULL) {
                printf("Erro: Faltando programa da tarefa.\n");
                continue;
            }

            char *args[MAX_ARGS];

            args[0] = token_cmd;

            int indice = 1;
            char *argumento = strtok(NULL, " \t\r\n");

            while (argumento != NULL && indice < MAX_ARGS - 1) {
                args[indice] = argumento;
                indice++;

                argumento = strtok(NULL, " ");
            }

            args[indice] = NULL;

            cadastrar_task(token_nome, token_cmd, args);
        } 
        
        else if (strcmp(comando, "run") == 0) {
            char *token_nome = strtok(NULL, " \t\r\n");

            if (token_nome == NULL) {
                printf("Erro: Faltam argumentos para o comando run.\n");
                continue;
            }

            if (strcmp(token_nome, "sequential") == 0) {
                token_nome = strtok(NULL, " \t\r\n");

                if (token_nome == NULL) {
                    printf("Erro: Informe pelo menos uma tarefa para executar.\n");
                    continue;
                }

                while (token_nome != NULL) {
                    task *t = buscar_task(token_nome);

                    if (t == NULL) {
                        printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                    } else {
                        executar_task(t);
                    }

                    token_nome = strtok(NULL, " \t\r\n"); 
                }
            } 
            
            else if (strcmp(token_nome, "parallel") == 0) {
                task *tarefas_para_rodar[MAX_TASKS];
                int qtd = 0;
                
                token_nome = strtok(NULL, " \t\r\n");

                while (token_nome != NULL && qtd < MAX_TASKS) {
                    task *tarefa_encontrada = buscar_task(token_nome);

                    if (tarefa_encontrada != NULL) {
                        tarefas_para_rodar[qtd] = tarefa_encontrada;
                        qtd++;
                    } else {
                        printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                    }

                    token_nome = strtok(NULL, " \t\r\n");
                }

                if (qtd > 0){
                    executar_paralelo(tarefas_para_rodar, qtd);
                }
            } 

            else if (strcmp(token_nome, "pipe") == 0) {
                task *tarefas_para_rodar[MAX_TASKS];
                int qtd = 0;
                
                token_nome = strtok(NULL, " \t\r\n");

                while (token_nome != NULL && qtd < MAX_TASKS) {
                    task *tarefa_encontrada = buscar_task(token_nome);

                    if (tarefa_encontrada != NULL) {
                        tarefas_para_rodar[qtd] = tarefa_encontrada;
                        qtd++;
                    } else {
                        printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                    }

                    token_nome = strtok(NULL, " \t\r\n");
                }

                if (qtd > 0){
                    executar_pipe(tarefas_para_rodar, qtd);
                }
            }

                        else {
                task *t = buscar_task(token_nome);

                if (t == NULL) {
                    printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                } else {
                    executar_task(t);
                }
            }
        }

        else if (strcmp(comando, "input") == 0 || strcmp(comando, "output") == 0 || strcmp(comando, "append") == 0) {
            char *nome_task = strtok(NULL, " \t\r\n");
            char *nome_arquivo = strtok(NULL, " \t\r\n");

            if (nome_task == NULL || nome_arquivo == NULL) {
                fprintf(stderr, "Erro: Sintaxe incorreta. Use: %s <nome_da_task> <arquivo>\n", comando);
            } else {
                task *t = buscar_task(nome_task);
                
                if (t != NULL) {
                    if (strcmp(comando, "input") == 0) {
                        t->arq_in = strdup(nome_arquivo);
                    } else if (strcmp(comando, "output") == 0) {
                        t->arq_out = strdup(nome_arquivo);
                    } else if (strcmp(comando, "append") == 0) {
                        t->arq_append = strdup(nome_arquivo);
                    }
                } else {
                    fprintf(stderr, "Erro: Tarefa '%s' não encontrada.\n", nome_task);
                }
            }
        }

        else if (strcmp(comando, "start") == 0) {
            char *nome_task = strtok(NULL, " \t\r\n");
            if (nome_task == NULL) {
                fprintf(stderr, "Erro: Sintaxe incorreta. Use: start <tarefa>\n");
            } else {
                task *t = buscar_task(nome_task);
                if (t != NULL) {
                    executar_background(t);
                } else {
                    fprintf(stderr, "Erro: Tarefa '%s' não encontrada.\n", nome_task);
                }
            }
        }

        else if (strcmp(comando, "jobs") == 0) {
            listar_jobs();
        }

        else if (strcmp(comando, "wait") == 0) {
            char *id_str = strtok(NULL, " \t\r\n");
            if (id_str == NULL) {
                fprintf(stderr, "Erro: Sintaxe incorreta. Use: wait <job_ID>\n");
            } else {
                int job_id = atoi(id_str);
                aguardar_job(job_id);
            }
        }

        else if (strcmp(comando, "workdir") == 0) {
            char *diretorio = strtok(NULL, " \t\r\n");
            
            if (diretorio == NULL) {
                fprintf(stderr, "Erro: Sintaxe incorreta. Use: workdir <diretório>\n");
            } else {
                if (chdir(diretorio) != 0) {
                    perror("Erro ao mudar o diretório");
                } else {
                    printf("Diretório de trabalho alterado para: %s\n", diretorio);
                }
            }
        }

        else {
            fprintf(stderr, "Erro: Comando '%s' não reconhecido.\n", comando);
        }
    }

    if (!modo_interativo && arquivo != NULL) {
        fclose(arquivo);
    }

    return 0;
}