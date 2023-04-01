#include "data_structures_fs.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int main()
{
    directory_entry *test_fran = malloc(sizeof(directory_entry)); // Le reservamos memoria
    struct inode *inodo = malloc(sizeof(struct inode));
    int directos[10],simples[1];
    *inodo->i_directos = directos;
    *inodo->i_simple_ind = simples;
    inodo->i_type = '-';
    inodo->i_tam = 0;
    strcpy(test_fran->name, "prueba_search.txt"); // Guardo el nombre

    test_fran->inode = inodo;

    long block_mem = malloc(1024); // Primer bloque

    if(!memcpy(block_mem,test_fran,sizeof(directory_entry))){
        return -1;
    }

    // Ya hemos mapeado nuestro bloque
    directory_entry *traido_mem = (directory_entry *) mmap(NULL,sizeof(directory_entry),PROT_WRITE,MAP_SHARED,block_mem,0);

    printf("Entrada: %s", *traido_mem->name); 
}