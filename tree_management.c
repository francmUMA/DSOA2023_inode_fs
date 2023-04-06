#include "data_structures_fs.h"
#include "tree_search.c"
#include <stdio.h>
#include <stdlib.h>

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(struct inode *directory_dst, struct inode n_node)
{
    int i = 0, j = 0;
    struct directory_entry *entry = malloc(sizeof(directory_entry));
    // Comenzamos en nuestro directorio raíz
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL){
        // Vamos a buscar el primer hueco libre en el bloque
        // Nos traemos el bloque
        while(j < 32 && entry != NULL){
            
        }
        i++;
    }
}