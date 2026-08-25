# Implemntação 1 - Process Flow

## Estrutura de Arquivos
*   **`main.c`**: Contém a estrutura base do projeto, o loop principal de captura de comandos (REPL) e o roteamento das operações usando `if/else`.

*   **`task.h` / `task.c`**: Módulo responsável pela definição das estruturas de dados (`struct task`) e funções de gerenciamento, como cadastro, busca e execução (incluindo `fork` e `execvp`).

*   **`Makefile`**: Arquivo de automação de compilação.

*   **`*.pf`**: Scripts de teste contendo sequências de comandos para o modo workflow (ex: `teste-limite.pf`, `teste-invalido.pf`).

## Como Compilar
O projeto utiliza um `Makefile` para facilitar a construção. No terminal, execute:

```bash
# Para compilar o projeto, rode: 

make

# Para limpar os arquivos compilados, rode:

make clean
```

## Como Executar
O orquestrador possui dois modos de operação:

1. **Modo Interativo:** Um prompt de comandos aguardará as entradas do usuário. 

   ```bash
   # Para executar em modo interativo, rode:

   ./processflow
   ```

2. **Modo Workflow:** Lê e executa todos os comandos de um arquivo `.pf` em lote.

   ```bash
   # Para executar em workflow, rode:

   ./processflow <arquivo>.pf
   ```

## Como Testar

Para testar você pode usar os testes já criados previamente com

```bash
./processflow teste-limite.pf
./processflow teste-invalido.pf
```

Ou você pode entrar no modo interativo com

```bash
./processflow
```

### Lista de comandos do modo interativo:

* **Cadastro de Tarefas:** Para registrar um novo comando no orquestrador.

    ```bash
    task <nome_da_task> <comando_unix> [argumentos...]
    ```

*   **Execução Simples:** Executa uma única tarefa previamente cadastrada.

    ```bash
    run <nome_da_task>
    ```

*   **Execuções Múltiplas:** Executa duas ou mais tarefas cadastradas de diferentes formas.

    *   *Sequencial:* 

        ```bash
        run sequential <task_1> <task_2> ... <task_n>
        ```

    *   *Paralela:* 

        ```bash
        run parallel <task_1> <task_2> ... <task_n>
        ```

    *   *Pipe:* 

        ```bash
        run pipe <task_1> <task_2> ... <task_n>
        ```

*   **Redirecionamento:** Configura a entrada ou saída de uma tarefa antes da sua execução.

    *   *Entrada:* 

        ```bash
        input <nome_da_task> <caminho_do_arquivo>
        ```

    *   *Saída (Sobrescrever):* 

        ```bash
        output <nome_da_task> <caminho_do_arquivo>
        ```

    *   *Saída (Adicionar):* 

        ```bash
        append <nome_da_task> <caminho_do_arquivo>
        ```

*   **Processos em Background:** Gerencia tarefas rodando em segundo plano.

    *   *Iniciar:* 

        ```bash
        start <nome_da_task>
        ```

    *   *Listar ativos:* 

        ```bash
        jobs
        ```

    *   *Aguardar finalização:* 
        ```bash
        wait <job_ID>
        ```

*   **Sistema:** Comandos de controle do próprio orquestrador.

    *   *Mudar diretório:* 

        ```bash
        workdir <caminho_do_diretorio>
        ```

    *   *Encerrar o programa:* 
    
        ```bash
        exit
        ```