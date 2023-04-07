#include "data_structures_fs.h"
#include "bitmap.c"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(struct inode *directory_dst, struct inode n_node) // Versión 1 (Como si solo tuvieramos directos)
{
    int i = 0, end = 0, offset;
    directory_entry *entry = malloc(sizeof(directory_entry));
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){
        // Vamos a buscar el primer hueco libre en el bloque
        // Recorremos el bloque
        memcpy(entry, (*directory_dst).i_directos[i], sizeof(directory_entry));
        for(int j = 0; j < 32 && (entry -> inode) != NULL; j++){
            //Traemos la entrada de directorio
            offset = sizeof(directory_entry)*j;
            memcpy(entry, (*directory_dst).i_directos[i]+offset, sizeof(directory_entry));
        }
        // Nos aseguramos de que nuestra entrada es null
        if (entry->inode == NULL)
        {
            // Guardamos nuestra entrada de bloque
            strcpy(entry->name,n_node.i_name);
            entry->inode = &n_node;
            // Copiado el bloque
            memcpy((*directory_dst).i_directos[i]+offset, entry,sizeof(directory_entry));
            entry = NULL;
            free(entry);
            end = 1;
        }
        i++;
    }
}