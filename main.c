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
        
        else if (strcmp(comando, "run") == 0) {
            char *token_nome = strtok(NULL, " ");

            if (token_nome == NULL) {
                printf("Erro: Faltam argumentos para o comando run.\n");
                continue;
            }

            if (strcmp(token_nome, "sequential") == 0) {
                token_nome = strtok(NULL, " ");

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

                    token_nome = strtok(NULL, " "); 
                }
            } 
            
            else if (strcmp(token_nome, "parallel") == 0) {
                task *tarefas_para_rodar[MAX_TASKS];
                int qtd = 0;
                
                token_nome = strtok(NULL, " ");

                while (token_nome != NULL && qtd < MAX_TASKS) {
                    task *tarefa_encontrada = buscar_task(token_nome);

                    if (tarefa_encontrada != NULL) {
                        tarefas_para_rodar[qtd] = tarefa_encontrada;
                        qtd++;
                    } else {
                        printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                    }

                    token_nome = strtok(NULL, " ");
                }

                if (qtd > 0){
                    executar_paralelo(tarefas_para_rodar, qtd);
                }
            } 

            else if (strcmp(token_nome, "pipe") == 0) {
                task *tarefas_para_rodar[MAX_TASKS];
                int qtd = 0;
                
                token_nome = strtok(NULL, " ");

                while (token_nome != NULL && qtd < MAX_TASKS) {
                    task *tarefa_encontrada = buscar_task(token_nome);

                    if (tarefa_encontrada != NULL) {
                        tarefas_para_rodar[qtd] = tarefa_encontrada;
                        qtd++;
                    } else {
                        printf("Erro: Tarefa '%s' não encontrada.\n", token_nome);
                    }

                    token_nome = strtok(NULL, " ");
                }

                if (qtd > 0){
                    executar_pipe(tarefas_para_rodar, qtd);
                }
            }

            else if (strcmp(token_nome, "input") == 0 || strcmp(token_nome, "output") == 0 || strcmp(token_nome, "append") == 0) {
            char *nome_task = strtok(NULL, " \n");
            char *nome_arquivo = strtok(NULL, " \n");

            if (nome_task == NULL || nome_arquivo == NULL) {
                fprintf(stderr, "Erro: Sintaxe incorreta. Use: run %s <nome_da_task> <arquivo>\n", token_nome);
            } else {
                task *t = buscar_task(nome_task);
                if (t != NULL) {
                    executar_redirecionado(t, nome_arquivo, token_nome);
                } else {
                    fprintf(stderr, "Erro: Tarefa '%s' não encontrada no catálogo.\n", nome_task);
                }
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
    }

    return 0;
}