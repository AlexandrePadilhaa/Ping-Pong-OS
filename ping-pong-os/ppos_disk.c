/*esse arquivo n deveria ser dado?*/
#include "ppos_disk.h"
#include "disk.h"
#include "ppos-core-globals.h"
#include "ppos.h"
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// PROJETO B

disk_t *disco;
FilaPedidos *fila_pedidos;
struct sigaction action;

void capturaSinal(int signum) {
  if (disco->state == 0) {
    task_resume(disco->tarefa_gerenciadora);
    disco->state = 1;
  }
  disco->sinal = disco->state;
}

// interface de gerencia do disco
void diskManager() { /*IMPLEMENTAR*/
}

// inicializacao do gerente de disco
int disk_mgr_init(int *numBlocos, int *tamBloco) {

  int inicializaDisco = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);
  if (!inicializaDisco) {
    printf("Erro ao inicializar gerente de disco");
    return -1;
  }

  // inicializando tamanho de blocos e numero de blocos
  *tamBloco = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);
  *numBlocos = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);

  if (*numBlocos == NULL || *tamBloco == NULL) {
    printf("Erro ao inicializar gerente de disco");
    return -1;
  }

  // inicializa disco
  disco = (disk_t *)malloc(sizeof(disk_t));
  disco->sem_disco = (semaphore_t *)malloc(sizeof(semaphore_t));
  disco->tarefas_supensas = (semaphore_t *)malloc(sizeof(semaphore_t));
  disco->state = 0; // sem uso
  // sem_create para tarefas_supensas

  // inicializa tarefa gerenciadora
  disco->tarefa_gerenciadora = (disk_t *)malloc(sizeof(disk_t));
  disco->tarefa_gerenciadora->tipo_task = 0; // setando como tarefa de sistema
  disco->tarefa_gerenciadora->next = disco->tarefa_gerenciadora->prev = NULL;
  task_create(&disco->tarefa_gerenciadora, diskManager,
              "Gerenciadora de disco");

  // inicializa fila de pedidos
  fila_pedidos = (FilaPedidos *)malloc(sizeof(FilaPedidos));
  fila_pedidos->head = NULL;
  fila_pedidos->tail = NULL;

  // inicializa sinal
  action.sa_handler = capturaSinal;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, 0);

  return 0;
}

/*Cada tarefa que solicita uma operação de leitura/escrita no disco deve ser
suspensa até que a operação solicitada seja completada.*/

// leitura de um bloco, do disco para o buffer
int disk_block_read(int bloco, void *buffer) {

  if (bloco < 0 || bloco >= fila_pedidos->blocos_percorridos) {
    printf("Erro: Bloco inválido.\n");
    return -1;
  }
  return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int bloco, void *buffer) {
  if (bloco < 0 || bloco >= fila_pedidos->blocos_percorridos) {
    printf("Erro: Bloco inválido.\n");
    return -1;
  }

  return 0;
}