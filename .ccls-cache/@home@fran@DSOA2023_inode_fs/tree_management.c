#include "data_structures_fs.h"
#include "tree_search.c"
#include <stdio.h>
#include <stdlib.h>

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(struct inode *directory_dst, struct inode n_node)
{
    int i = 0;
    directory_entry *entry = malloc(sizeof(directory_entry));
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL){
        // Vamos a buscar el primer hueco libre en el bloque
        // Recorremos el bloque
        memcpy(entry, (*directory_dst).i_directos[i], sizeof(directory_entry));
        for(int j = 0; j < 32 && (entry -> inode) != NULL; j++){ // j es offset
            //Traemos la entrada de directorio
            memcpy(entry, (*directory_dst).i_directos[i]+sizeof(directory_entry)*j, sizeof(directory_entry));
            // Si el inodo es NULL, es que es un hueco libre
        }
        i++;
    }
}