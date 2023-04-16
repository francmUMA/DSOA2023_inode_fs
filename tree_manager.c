#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Busqueda del inodo "target" en cuestion en el directorio "directory"
//Si se encuentra el inodo, se devuelve el inodo
//Si no se encuentra, se devuelve NULL

struct inode_fs *inode_search(char *target, struct inode_fs *directory){
    int i = 0;
    struct inode_fs *res = malloc(sizeof(struct inode_fs));
    res = NULL;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && (*directory).i_directos[i] != NULL){
        // Recorremos el bloque
        memcpy(entry, (*directory).i_directos[i], sizeof(struct directory_entry));
        for(int j = 0; j < 32 && entry -> inode != NULL; j++){ // j es offset
            memcpy(entry, (*directory).i_directos[i]+sizeof(struct directory_entry)*j, sizeof(struct directory_entry));

            // Para cada elemento comprobar si es target
            if(strcmp((*entry).name,target) == 0){
                // Si es target hacemos return del inodo.
                return (*entry).inode;
            }

            // En caso de ser directorio entramos en el mismo. Continuamos en caso contrario.
            else if((*(*entry).inode).i_type == 'd' && (res = inode_search(target, (*entry).inode)) != NULL){
                return res; // Devuelve el inodo del archivo buscado (recursivamente)
            }
        }
        i++;    
    }
    return res;
}

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(char *name, struct inode_fs *directory_dst, struct inode_fs n_node) // Versión 1 (Como si solo tuvieramos directos)
{
    int i = 0, end = 0, offset;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){
        // Vamos a buscar el primer hueco libre en el bloque
        // Recorremos el bloque
        memcpy(entry, (*directory_dst).i_directos[i], sizeof(struct directory_entry));
        for(int j = 0; j < 32 && (entry -> inode) != NULL; j++){
            //Traemos la entrada de directorio
            offset = sizeof(struct directory_entry)*j;
            memcpy(entry, (*directory_dst).i_directos[i]+offset, sizeof(struct directory_entry)); // Preguntar Fran dudilla offset
        }
        // Nos aseguramos de que nuestra entrada es null
        if (entry->inode == NULL)
        {
            // Guardamos nuestra entrada de bloque
            strcpy(entry->name, name);
            entry->inode = &n_node;
            // Copiado el bloque
            memcpy((*directory_dst).i_directos[i]+offset, entry,sizeof(struct directory_entry));
            entry = NULL;
            free(entry);
            end = 1;
        }
        i++;
    }
    
    // Si no hemos encontrado un hueco libre, creamos un nuevo bloque si es posible
    if (!end && i < N_DIRECTOS)
    {
        // Creamos un nuevo bloque
        (*directory_dst).i_directos[i] = malloc(sizeof(struct directory_entry)*32);
        // Guardamos nuestra entrada de bloque
        strcpy(entry->name, name);
        entry->inode = &n_node;
        // Copiamos el bloque
        memcpy((*directory_dst).i_directos[i], entry,sizeof(struct directory_entry));
        entry = NULL;
        free(entry);
    }
    else if (!end) {
        printf("No hay espacio en el directorio\n");
    }
}