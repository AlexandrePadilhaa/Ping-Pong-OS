/*esse arquivo n deveria ser dado?*/
#include "ppos_disk.h"
#include "ppos.h"


//PROJETO B

static disk_t disk ;// hard disk structure
task_t taskManager;

// interface de gerencia do disco
void diskManager(){
/*IMPLEMENTAR*/
}

// inicializacao do gerente de disco
int disk_mgr_init (int *numBlocks, int *blockSize) {
    
    int initDisk = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);

    if (!initDisk)
    {
        printf("Erro ao inicializar gerente de disco");
        return -1;
    }
    
    //inicializando tamanho de blocos e numero de blocos
    *blockSize = disk_cmd (DISK_CMD_BLOCKSIZE, 0, 0);
    *numBlocks = disk_cmd (DISK_CMD_DISKSIZE, 0, 0);

    if (*numBlocks == NULL || *blockSize == NULL )
    {
        printf("Erro ao inicializar gerente de disco");
        return -1;
    }
    

      // tarefa gerenciadora do disco
    task_create(&taskManager,diskManager,"Disk Manager");

    return 0;
}

/*Cada tarefa que solicita uma operação de leitura/escrita no disco deve ser suspensa
até que a operação solicitada seja completada.*/

// leitura de um bloco, do disco para o buffer
int disk_block_read (int block, void *buffer) {
    

    if (block < 0 || block >= disk->numBlocks) {
        printf("Erro: Bloco inválido.\n");
        return -1;
    }
    return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write (int block, void *buffer) {
        if (block < 0 || block >= disk->numBlocks) {
        printf("Erro: Bloco inválido.\n");
        return -1;
    }

    return 0;
}