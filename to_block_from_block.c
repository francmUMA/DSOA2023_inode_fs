#include "data_structures_fs.h"
#include "bitmap.c"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

long create_block(){
    long *block = malloc(1024);
    memset(block, 0, 1024);
    long index = free_block();
    blocks[index] = block;
    return index;
}

int main()
{
    // HAY QUE INICIALIZAR EN EL FUSE MAIN
    block_bitmap = malloc(sizeof(struct block_bitmap_fs));
    inode_bitmap = malloc(sizeof(struct inode_bitmap_fs));
    // directory_entry *test_fran = malloc(sizeof(directory_entry)); // Le reservamos memoria
    // struct inode *inodo = malloc(sizeof(struct inode));
    // int directos[10],simples[1];
    // *inodo->i_directos = directos;
    // *inodo->i_simple_ind = simples;
    // inodo->i_type = '-';
    // inodo->i_tam = 0;
    // strcpy(test_fran->name, "prueba_search.txt"); // Guardo el nombre

    // test_fran->inode = inodo;
    
    //Guardar en el mismo bloque de memoria la entrada de directorio dos veces.
    char test[20] = "Esto es una prueba";
    long block_index = create_block();
    long aux;

    //Copiamos el puntero direct a block_mem
    memcpy(&aux,blocks[block_index],sizeof(long));

    //Mostramos el contenido y la direccion de block mem
    printf("Contenido de block_mem: %s\n", aux);
    printf("Direccion de block_mem: %ld\n", &aux);

    //memcpy(block_mem,test_fran,sizeof(directory_entry));
    // memcpy(block_mem+sizeof(directory_entry),test_fran,sizeof(directory_entry));


    //Mostrar el bloque donde se ha mapeado
    //printf("Bloque de memoria: %ld", block_mem);

    //Acceder a las entradas de directorio del bloque de memoria
    //directory_entry *traido_mem = malloc(sizeof(directory_entry));
    //memcpy(traido_mem,block_mem+sizeof(directory_entry),sizeof(directory_entry));
    //printf("Entrada: %c", traido_mem->inode->i_type);
    /*
    memcpy(traido_mem,block_mem+sizeof(directory_entry),sizeof(directory_entry));
    printf("Entrada: %s", traido_mem->name);

    //Eliminamos la segunda entrada de directorio
    memset(&block_mem+sizeof(directory_entry),0,sizeof(directory_entry));
    memcpy(traido_mem,block_mem+sizeof(directory_entry),sizeof(directory_entry));
    printf("Entrada: %s", traido_mem->name);
    */
    return 0;
    }