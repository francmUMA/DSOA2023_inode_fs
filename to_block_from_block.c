#include "data_structures_fs.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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
    
    //Guardar en el mismo bloque de memoria la entrada de directorio dos veces.
    long block_mem = malloc(1024); // Primer bloque
    memcpy(block_mem,test_fran,sizeof(directory_entry));
    memcpy(block_mem+sizeof(directory_entry),test_fran,sizeof(directory_entry));


    //Mostrar el bloque donde se ha mapeado
    printf("Bloque de memoria: %ld", block_mem);

    //Acceder a las entradas de directorio del bloque de memoria
    directory_entry *traido_mem = malloc(sizeof(directory_entry));
    memcpy(traido_mem,block_mem,sizeof(directory_entry));
    printf("Entrada: %s", traido_mem->name);
    memcpy(traido_mem,block_mem+sizeof(directory_entry),sizeof(directory_entry));
    printf("Entrada: %s", traido_mem->name);

    //Eliminamos la segunda entrada de directorio
    memset(&block_mem+sizeof(directory_entry),0,sizeof(directory_entry));
    memcpy(traido_mem,block_mem+sizeof(directory_entry),sizeof(directory_entry));
    printf("Entrada: %s", traido_mem->name);
}