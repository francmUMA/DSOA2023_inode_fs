#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>


//Busqueda del inodo "target" en cuestion en el directorio "directory"
//Si se encuentra el inodo, se devuelve el inodo
//Si no se encuentra, se devuelve NULL
static struct inode *inode_search(char *target, struct inode directory){
    int i = 0;
    struct inode *res = malloc(sizeof(struct inode));
    res = NULL;
    directory_entry *entry = malloc(sizeof(directory_entry));
    while(i < N_DIRECTOS && directory.i_directos[i] != NULL){
        // Recorremos el bloque
        for(int j = 0; j < 32; j++){ // j es offset
            memcpy(directory.i_directos[i]+sizeof(directory_entry)*j, entry, sizeof(directory_entry));

            // Para cada elemento comprobar si es target
            if(strcmp(entry->name,target) == 0){
                // Si es target hacemos return del inodo.
                return entry->inode;
            }

            // En caso de ser directorio entramos en el mismo. Continuamos en caso contrario.
            else if(entry->inode->i_type == 'd' && (res = inode_search(target, *entry->inode)) != NULL){
                return res; // Devuelve el inodo del archivo buscado (recursivamente)
            }
        }
        i++;    
    }
    return res;
}
