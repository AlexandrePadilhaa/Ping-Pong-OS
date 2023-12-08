#include "ppos.h"
#include "ppos-core-globals.h"

// ****************************************************************************
// Coloque aqui as suas modificações, p.ex. includes, defines variáveis, 
// estruturas e funções

#include <signal.h>
#include <sys/time.h>

#define TASK_TIPO_SISTEMA '0'
#define TASK_TIPO_USUARIO '1'
#define MILISEGUNDO 1000
#define QUANTUM_MAX 20
#define TEMPO_DEFAULT 99999

// estrutura que define um tratador de sinal (deve ser global ou static)
struct sigaction action;

// estrutura de inicialização to timer
struct itimerval timer;

// estrutura do relogio
int quantum = QUANTUM_MAX;


// funções 1.2.a
void task_set_eet (task_t *task, int et){

    if(task != NULL){//ajuste na tarefa do parametro
        task->eet = et;
        task->ret = task->eet - task->tempo_decorrido;

    }else{//ajuste na tarefa em execução
        taskExec->eet = et;
        taskExec->ret = task->eet - task->tempo_decorrido;

    }

}

int task_get_eet(task_t *task){	
  if(task != NULL){
    return task->eet;
  }else{
    return taskExec->eet;
  }
}

int task_get_ret(task_t *task) {
  if(task != NULL){
    return task->ret;
  } else{
    return taskExec->ret;
  }
}

//Quando uma tarefa recebe o processador, o dispatcher ajusta um contador de ticks que
//essa tarefa pode usar, ou seja, seu quantum definido em número de ticks. A cada tick, esse contador
//deve ser decrementado; quando ele chegar a zero, o processador deve ser devolvido ao dispatcher
//e a tarefa volta à fila de prontas.

static void temporizador () {// contar tarefas sistema
	systemTime++;
	if (taskExec != NULL) {
        taskExec->tempo_decorrido++;
         //Atualizar tempo
        taskExec->tempo_final = systemTime - taskExec->tempo_inicial;
		if (taskExec->tipo_task == TASK_TIPO_USUARIO) { //se é tarefa de sistema, executa a preempcao
			quantum--;
		}
        task_set_eet(taskExec, task_get_eet(NULL) );

		if (quantum <= 0 && taskExec != taskDisp) {         
			task_yield();
            quantum = QUANTUM_MAX;			
		}
	}
}

void inicializa_sinal (){
    // registra a ação para o sinal de timer SIGALRM
    action.sa_handler = temporizador;
    sigemptyset (&action.sa_mask) ;
    action.sa_flags = 0 ;
    if (sigaction (SIGALRM, &action, 0) < 0)
    {
        perror ("Erro em sigaction: ");
        exit (1) ;
    }

    // ajusta valores do temporizador
    timer.it_value.tv_usec = MILISEGUNDO;      // primeiro disparo, em micro-segundos que é 1000 para ser mili
    timer.it_value.tv_sec  = 0 ;      // primeiro disparo, em segundos
    timer.it_interval.tv_usec = MILISEGUNDO;   // disparos subsequentes, em micro-segundos que é 1000 para ser mili
    timer.it_interval.tv_sec  = 0 ;   // disparos subsequentes, em segundos

    // arma o temporizador ITIMER_REAL (vide man setitimer)
    if (setitimer (ITIMER_REAL, &timer, 0) < 0)
    {
        perror ("Erro em setitimer: ");
        exit (1) ;
    }
}

void log_tarefa(){
	printf("\nTask %d exit: execution time %u ms, processor time %u ms, %d activations\n", taskExec->id, taskExec->tempo_final, taskExec->tempo_decorrido, taskExec->ativacoes);
}

// ****************************************************************************

void before_ppos_init () {
    // chama o inicializador do temporizador
    inicializa_sinal();
#ifdef DEBUG
    printf("\ninit - BEFORE");
#endif
}

void after_ppos_init () {
    // inicialza o tempo e tipo das tarefas de sistema
	task_set_eet(taskDisp, TEMPO_DEFAULT);
    taskDisp->tipo_task = TASK_TIPO_SISTEMA;
    taskDisp->ativacoes = 0;
    task_set_eet(taskMain, TEMPO_DEFAULT);
    taskMain->tipo_task = TASK_TIPO_SISTEMA;
    taskMain->ativacoes = 0;
#ifdef DEBUG
    printf("\ninit - AFTER");
#endif
}

void before_task_create (task_t *task ) {
    // seta o tempo das tarefas de usuario
    if(task->tipo_task != TASK_TIPO_SISTEMA){
		task_set_eet(task, TEMPO_DEFAULT);
		task->tipo_task = TASK_TIPO_USUARIO;
        task->ativacoes = 0;
    } 
#ifdef DEBUG
    printf("\ntask_create - BEFORE - [%d]", task->id);
#endif
}

void after_task_create (task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_create - AFTER - [%d]", task->id);
#endif
}

void before_task_exit () {
    //Atualizar tempo
    taskExec->tempo_final = systemTime - taskExec->tempo_inicial;
	task_set_eet(taskExec, task_get_eet(taskExec));
	log_tarefa();
#ifdef DEBUG
    printf("\ntask_exit - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_exit () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_exit - AFTER- [%d]", taskExec->id);
#endif
}

void before_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - BEFORE - [%d -> %d]", taskExec->id, task->id);
#endif
}

void after_task_switch ( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_switch - AFTER - [%d -> %d]", taskExec->id, task->id);
#endif
}

void before_task_yield () {
    taskExec->ativacoes++;
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - BEFORE - [%d]", taskExec->id);
#endif
}
void after_task_yield () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_yield - AFTER - [%d]", taskExec->id);
#endif
}


void before_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - BEFORE - [%d]", task->id);
#endif
}

void after_task_suspend( task_t *task ) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_suspend - AFTER - [%d]", task->id);
#endif
}

void before_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - BEFORE - [%d]", task->id);
#endif
}

void after_task_resume(task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_resume - AFTER - [%d]", task->id);
#endif
}

void before_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - BEFORE - [%d]", taskExec->id);
#endif
}

void after_task_sleep () {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_sleep - AFTER - [%d]", taskExec->id);
#endif
}

int before_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_task_join (task_t *task) {
    // put your customization here
#ifdef DEBUG
    printf("\ntask_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}


int before_sem_create (semaphore_t *s, int value) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_create (semaphore_t *s, int value) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_down (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_down - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_down (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_down - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_up (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_up - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_up (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_up - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_sem_destroy (semaphore_t *s) {
    // put your customization here
#ifdef DEBUG
    printf("\nsem_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_create (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_lock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_lock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_unlock (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_unlock - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mutex_destroy (mutex_t *m) {
    // put your customization here
#ifdef DEBUG
    printf("\nmutex_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_create (barrier_t *b, int N) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_join (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_join - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_barrier_destroy (barrier_t *b) {
    // put your customization here
#ifdef DEBUG
    printf("\nbarrier_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_create (mqueue_t *queue, int max, int size) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_create - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_send (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_send - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_recv (mqueue_t *queue, void *msg) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_recv - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_destroy (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_destroy - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

int before_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - BEFORE - [%d]", taskExec->id);
#endif
    return 0;
}

int after_mqueue_msgs (mqueue_t *queue) {
    // put your customization here
#ifdef DEBUG
    printf("\nmqueue_msgs - AFTER - [%d]", taskExec->id);
#endif
    return 0;
}

task_t * scheduler() {
	#ifdef DEBUG
    	printf("iniciando scheduler\n");
	#endif

    task_t *task_auxiliar = NULL; // task auxiliar que vai nos ajudar a retornar a menor delas

    // se a fila estiver vazia
    if (readyQueue == NULL){
        return NULL;
	} 
    //se a fila contiver um ou mais elementos
    else {
        task_t* task_priori = readyQueue; // ponteiro para a prox tarefa na fila
        task_auxiliar = readyQueue;
				int menor_tempo = task_get_ret(task_priori);

        do{ // se o tempo restante da task for menor em comparação ao menor tempo encontrado, a nossa task auxiliar e substituida e assim por diante
            if(task_get_ret(task_auxiliar) <  menor_tempo) {
                task_priori = task_auxiliar;
								menor_tempo = task_get_ret(task_auxiliar);
            }
            //Proxima tarefa a ser comparada
            task_auxiliar = task_auxiliar->next;

        } while(task_auxiliar != readyQueue);  // só finaliza ao encontrar a próxima a ser executada

        task_auxiliar = task_priori;
    }
    return task_auxiliar;

    #ifdef DEBUG
    	printf("task %d foi escolhida pelo scheduler\n", task_auxiliar->id);
	#endif
}

