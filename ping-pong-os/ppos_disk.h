// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.2 -- Julho de 2017

// interface do gerente de disco rígido (block device driver)

#ifndef __DISK_MGR__
#define __DISK_MGR__
#include "ppos.h"

// estruturas de dados e rotinas de inicializacao e acesso
// a um dispositivo de entrada/saida orientado a blocos,
// tipicamente um disco rigido.

// estrutura que representa um disco no sistema operacional
typedef struct disk_t {
  struct task_t *tarefas_suspensas;    // tarefas suspensas do disco
  struct task_t *tarefa_atual;        // tarefa que esta sendo atendida
  struct task_t *tarefa_gerenciadora; // tarefa gerenciadora de disco
  int state; // indica estado do disco, 0 para livre, 1 em utilizacao
  int sinal; // flag sinal de disco
  int blocos_percorridos; // blocos percorridos pelo leitor de disco
  semaphore_t sem_disco;
  // completar com os campos necessarios
} disk_t;

typedef struct Pedido {
  task_t *tarefa;
  void *buffer; // Ponteiro genérico para o buffer
  int bloco;    // Valor do bloco
  int pedido;   // Valor do pedido (0 leitura 1 escrita)
} Pedido;

typedef struct FilaPedidos {
  struct Pedido *next;    // Ponteiro para o primeiro nó da fila
  struct Pedido *prev;    // Ponteiro para o último nó da fila
  int head;            // indica onde esta a cabeça do leitor
} FilaPedidos;

// inicializacao do gerente de disco
// retorna -1 em erro ou 0 em sucesso
// numBlocks: tamanho do disco, em blocos
// blockSize: tamanho de cada bloco do disco, em bytes

void escalonamentoFCFS();

void escalonamentoSSTF();

void escalonamentoCSCAN();

void gerenciaDisco(void * args);

int disk_mgr_init(int *numBlocks, int *blockSize);

// leitura de um bloco, do disco para o buffer
int disk_block_read(int block, void *buffer);

// escrita de um bloco, do buffer para o disco
int disk_block_write(int block, void *buffer);

#endif
