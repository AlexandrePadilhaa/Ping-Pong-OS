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
struct sigaction action2;

void capturaSinal(int signum) {
  if (disco->state == 0) {
    task_resume(disco->tarefa_gerenciadora);
    disco->state = 1;
  }
  disco->sinal = disco->state;
}

//Escalonamento CSCAN - Circular Scan
Pedido* escalonamentoCSCAN() {
  if (fila_pedidos != NULL) {
    Pedido *task_priori = fila_pedidos; // ponteiro para a próxima tarefa na fila
    Pedido *task_auxiliar = fila_pedidos;
    int menor_dist = abs(fila_pedidos->head - task_priori->bloco);
    int dist_aux = 0;

    // Encontrar a tarefa com menor distância em relação a cabeça de leitura
    do {
      int dist_aux = abs(fila_pedidos->head - task_auxiliar->bloco);
      if (dist_aux < menor_dist) {
        task_priori = task_auxiliar;
        menor_dist = dist_aux;
      }
      // Próxima tarefa a ser comparada
      task_auxiliar = task_priori;
    } while (task_auxiliar != fila_pedidos); // só finaliza ao encontrar a próxima a ser executada

    // se a tarefa com menor distancia esta na parte inferior do disco (ou na mesma posição da cabeça), a função retorna essa tarefa removendo-a da fila
    if (task_priori->bloco <= fila_pedidos->head) {
      return (Pedido *)queue_remove((queue_t **)&fila_pedidos, (queue_t *)task_priori);
    } else { 
      // senao a tarefa com menor distancia esta na parte superior do disco, a cabeça de leitura é movida para a extremidade inferior do disco
      fila_pedidos->head = 0;
      // encontra a tarefa mais próxima nessa parte da fila e a remove
      task_auxiliar = fila_pedidos->next;
      if (task_auxiliar->bloco <=  fila_pedidos->head) {
        return (Pedido *)queue_remove((queue_t **)&fila_pedidos, (queue_t *)task_auxiliar);
      }
    }
  } else {
    return 0;
  }
}

//Escalonamento FCFS - Shortest Seek-Time First
Pedido* escalonamentoSSTF(){
  if(fila_pedidos != NULL) {
    Pedido* task_priori = fila_pedidos; // ponteiro para a prox tarefa na fila
    Pedido* task_auxiliar = fila_pedidos;
		int menor_dist = abs(fila_pedidos->head - task_priori->bloco);
    int dist_aux = 0;

    do{ // se a distância da task for menor em comparação a menor distancia encontrada, a nossa task auxiliar eh substituida e assim por diante
        int dist_aux = abs(fila_pedidos->head - task_auxiliar->bloco);
        if(dist_aux <  menor_dist) {
            task_priori = task_auxiliar;
						menor_dist = dist_aux;
        }
        //Proxima tarefa a ser comparada
        task_auxiliar = task_auxiliar;
    } while(task_auxiliar != fila_pedidos);  // só finaliza ao encontrar a próxima a ser executada

    task_auxiliar = task_priori;
    return (Pedido *) queue_remove((queue_t **) &fila_pedidos, (queue_t *) task_auxiliar);

  } else {
    return 0;
  }
}

//Escalonamento FCFS - First Come, First Served
Pedido* escalonamentoFCFS(){
  if(fila_pedidos != NULL) {
     return (Pedido *) queue_remove((queue_t **)&fila_pedidos, (queue_t *)fila_pedidos->head);
  } else {
    return 0;
  }
}

// interface de gerencia do disco
void gerenciaDisco(void * args) {
   

      while (1) {
     
      sem_down(&disco->sem_disco);//solicita semaforo

      if (disco->sinal){// acorda tarefa que recebe o sinal 
         task_resume(disco->tarefa_atual); 
         disco->sinal = 0;
      }

       disco->state = disk_cmd(DISK_CMD_STATUS,0,0);

        if ((disco->state == DISK_STATUS_IDLE)){

          Pedido *pedidoFCFS = escalonamentoFCFS();
          //Pedido *pedidoFCFS = escalonamentoSSTF() ;
          //Pedido *pedidoFCFS = escalonamentoCSCAN();
        

          if (pedidoFCFS != 0)
          {
            disco->blocos_percorridos = disco->blocos_percorridos + abs(fila_pedidos->head - pedidoFCFS->bloco);
            fila_pedidos->head = pedidoFCFS->bloco;
            disco->tarefa_atual = pedidoFCFS->tarefa;//disco->tarefa_gerenciadora;//pedidoFCFS->tarefa;
            
            int pedido = disk_cmd(pedidoFCFS->pedido,pedidoFCFS->bloco,pedidoFCFS->buffer);
            if(pedido == 0){
               printf("Agendamento concluido\n Bloco percorridos até o momento - %d \n",disco->blocos_percorridos);
            }else{
               printf("Error ao realizar agendamento");
            }
          }
          
            sem_up(&disco->sem_disco);
            task_yield(); 

        }
         task_suspend(&taskExec,&disco->tarefas_suspensas);
      }
     
}

// inicializacao do gerente de disco
int disk_mgr_init(int *numBlocos, int *tamBloco) {

  int inicializaDisco = disk_cmd(DISK_CMD_INIT, 0, 0);
  if (inicializaDisco != 0) {
    printf("Erro ao inicializar gerente de disco \n ");
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
  disco->tarefas_suspensas = NULL;//(task_t *)malloc(sizeof(task_t));
  disco->tarefa_atual= NULL;//(task_t *)malloc(sizeof(task_t));//NULL;
  disco->state = 0; // sem uso
  sem_create(disco->sem_disco, 1);

  // inicializa tarefa gerenciadora
  disco->tarefa_gerenciadora = (task_t *)malloc(sizeof(task_t));
  disco->tarefa_gerenciadora->tipo_task = 0; // setando como tarefa de sistema
  disco->tarefa_gerenciadora->next = disco->tarefa_gerenciadora->prev = NULL;
  task_create(&disco->tarefa_gerenciadora, gerenciaDisco,
              "Gerenciadora de disco");

  // inicializa fila de pedidos
  fila_pedidos = (FilaPedidos *)malloc(sizeof(FilaPedidos));
  fila_pedidos->next = NULL;
  fila_pedidos->prev = NULL;

  // inicializa sinal
  action2.sa_handler = capturaSinal;
  sigemptyset(&action2.sa_mask);
  action2.sa_flags = 0;
  sigaction(SIGUSR1, &action2, 0);

  return 0;
}

/*Cada tarefa que solicita uma operação de leitura/escrita no disco deve ser
suspensa até que a operação solicitada seja completada.*/

// leitura de um bloco, do disco para o buffer
int disk_block_read(int bloco, void *buffer) {

  if (bloco < 0 || bloco >= disco->blocos_percorridos) {
    printf("Erro: Bloco inválido. 1\n");
    return -1;
  }

  //solicita semaforo
  sem_down(&disco->sem_disco);

  Pedido pedidoLeitura;
  pedidoLeitura.bloco = bloco;
  pedidoLeitura.buffer = buffer;
  pedidoLeitura.tarefa = taskExec; //task em execução cria pedido de leitura
  pedidoLeitura.pedido = DISK_CMD_READ;

  //adicionando pedido a fila
   queue_append((queue_t **) &fila_pedidos, (queue_t *) &pedidoLeitura);
  
  //acorda gerente de disco
  if (disco->tarefa_gerenciadora->state == 's'){
      task_resume(&disco->tarefa_gerenciadora);
   }

   //libera semaforo
   sem_up(&disco->sem_disco);
  
  //suspende tarefas
   task_suspend(&taskExec,&disco->tarefas_suspensas);
   
   task_yield();
  
  return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int bloco, void *buffer) {
  /*if (bloco < 0 || bloco >= disco->blocos_percorridos) {
    printf("Erro: Bloco inválido. 2\n");
    return -1;
  }*/

  
  //solicita semaforo
  sem_down(&disco->sem_disco);


  Pedido pedidoEscrita;
  pedidoEscrita.bloco = bloco;
  pedidoEscrita.buffer = &buffer;
  pedidoEscrita.tarefa = taskExec; //task em execução cria pedido de escrita
  pedidoEscrita.pedido = DISK_CMD_WRITE;
  
  //adicionando pedido a fila
   queue_append((queue_t **) &fila_pedidos, (queue_t *) &pedidoEscrita);

    //acorda gerente de disco
  if (disco->tarefa_gerenciadora->state == 's'){
      task_resume(&disco->tarefa_gerenciadora);
   }

   //libera semaforo
   sem_up(&disco->sem_disco);

  //suspende tarefa
  task_suspend(&taskExec,&disco->tarefas_suspensas);

   task_yield();

  return 0;
}