#include "ppos_disk.h"
#include "disk.h"
#include "ppos-core-globals.h"
#include "ppos.h"
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

disk_t *disco;
FilaPedidos *fila_pedidos;
struct sigaction action2;

void capturaSinal(int signum)
{
  if (disco->state == 0)
  {
    task_resume(disco->tarefa_gerenciadora);
    disco->state = 1;
  }
  disco->sinal = disco->state;
}

Pedido *escalonamentoFCFS()
{
  if (fila_pedidos != NULL)
  {
    return (Pedido *)queue_remove((queue_t **)&fila_pedidos, (queue_t *)fila_pedidos->head);
  }
  else
  {
    return 0;
  }
}

void gerenciaDisco(void *args){
  while (1){
    sem_down(&disco->sem_disco); // solicita semaforo

    if (disco->sinal)
    { // acorda tarefa que recebe o sinal
      task_resume(disco->tarefa_atual);
      disco->sinal = 0;
    }

    disco->state = disk_cmd(DISK_CMD_STATUS, 0, 0);

    if ((disco->state == DISK_STATUS_IDLE))
    {

      Pedido *pedidoFCFS = escalonamentoFCFS();

      if (!pedidoFCFS)
      {
        disco->blocos_percorridos = disco->blocos_percorridos + abs(fila_pedidos->head - pedidoFCFS->bloco);
        fila_pedidos->head = pedidoFCFS->bloco;
        disco->tarefa_atual = pedidoFCFS->tarefa;

        int pedido = disk_cmd(pedidoFCFS->pedido, pedidoFCFS->bloco, pedidoFCFS->buffer);
        if (pedido == 0)
        {
          printf("Agendamento concluido\n Bloco percorridos até o momento - %d \n", disco->blocos_percorridos);
        }
        else
        {
          printf("Error ao realizar agendamento");
        }
      }

      sem_up(&disco->sem_disco);
      task_yield();
    }
  }
}

int disk_mgr_init(int *numBlocks, int *blockSize)
{
  // inicializa disco e task gerenciadora
  disk_cmd(DISK_CMD_INIT, 0, 0);
  disco = (disk_t *)malloc(sizeof(disk_t));
  disco->tarefas_suspensas = NULL;
  disco->sem_disco = (semaphore_t *)malloc(sizeof(semaphore_t));
  sem_create(disco->sem_disco, 1);

  disco->tarefa_gerenciadora = (task_t *)malloc(sizeof(task_t));
  disco->tarefa_gerenciadora->tipo_task = 0;
  task_create(disco->tarefa_gerenciadora, gerenciaDisco, NULL);

  // inicializa fila de pedidos
  fila_pedidos = (FilaPedidos *)malloc(sizeof(FilaPedidos));
  fila_pedidos->prev = NULL;
  fila_pedidos->next = NULL;

  // inicializando tamanho de blocos e numero de blocos
  *numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
  *blockSize = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);
  if (*numBlocks == NULL || *blockSize == NULL)
  {
    printf("Erro ao inicializar gerente de disco");
    return -1;
  }

  // inicializa sinal
  action2.sa_handler = capturaSinal;
  sigemptyset(&action2.sa_mask);
  action2.sa_flags = 0;
  sigaction(SIGUSR1, &action2, 0);

  return 0;
}

int disk_block_read(int bloco, void *buffer)
{
  if (bloco < 0 || bloco >= disco->blocos_percorridos)
  {
    printf("Erro: Bloco inválido.\n");
    return -1;
  }

  sem_down(&disco->sem_disco);

  Pedido pedidoLeitura;
  pedidoLeitura.bloco = bloco;
  pedidoLeitura.buffer = buffer;
  pedidoLeitura.tarefa = taskExec;
  pedidoLeitura.pedido = DISK_CMD_READ;

  queue_append((queue_t **)&fila_pedidos, (queue_t *)&pedidoLeitura);

  if (disco->tarefa_gerenciadora->state == 's')
  {
    task_resume(&disco->tarefa_gerenciadora);
  }

  sem_up(&disco->sem_disco);

  task_suspend(taskExec, &disco->tarefas_suspensas);
  task_yield();

  return 0;
}

int disk_block_write(int bloco, void *buffer)
{
  if (bloco < 0 || bloco >= disco->blocos_percorridos)
  {
    printf("Erro: Bloco inválido.\n");
    return -1;
  }

  sem_down(&disco->sem_disco);

  Pedido pedidoEscrita;
  pedidoEscrita.bloco = bloco;
  pedidoEscrita.buffer = buffer;
  pedidoEscrita.tarefa = taskExec;
  pedidoEscrita.pedido = DISK_CMD_WRITE;

  queue_append((queue_t **)&fila_pedidos, (queue_t *)&pedidoEscrita);

  if (disco->tarefa_gerenciadora->state == 's')
  {
    task_resume(&disco->tarefa_gerenciadora);
  }

  sem_up(&disco->sem_disco);

  task_suspend(taskExec, &disco->tarefas_suspensas);
  task_yield();

  return 0;
}
