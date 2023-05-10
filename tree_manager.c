#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(char *name, struct inode_fs *directory_dst, struct inode_fs* n_node) // Versión 1 (Como si solo tuvieramos directos)
{
    int i = 0, end = 0, offset;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){
        // Vamos a buscar el primer hueco libre en el bloque
        // Recorremos el bloque
        memcpy(entry, blocks[directory_dst->i_directos[i]], sizeof(struct directory_entry));
        for(int j = 1; j < 32 && entry -> inode != NULL; j++){
            //Traemos la entrada de directorio
            offset = sizeof(struct directory_entry)*j;
            memcpy(entry, blocks[directory_dst->i_directos[i]]+offset, sizeof(struct directory_entry)); 
        }
        // Nos aseguramos de que nuestra entrada es null
        if (entry -> inode == NULL)
        {
            // Guardamos nuestra entrada de bloque
            strcpy(entry->name, name);

            entry->inode = n_node;
            // Copiado el bloque
            memcpy(blocks[directory_dst->i_directos[i]]+offset, entry,sizeof(struct directory_entry));
            end = 1;
        }
        i++;
    }
    
    // Si no hemos encontrado un hueco libre, creamos un nuevo bloque si es posible
    if (!end && i < N_DIRECTOS){
        // Creamos un nuevo bloque
        directory_dst -> i_directos[i] = create_block();

        // Modificamos la entrada de directorio
        strcpy(entry->name, name);

        // Si son n_node es el mismo inodo que directory_dst, se guarda en la entry el directory_dst
        if ((*n_node).i_num == (*directory_dst).i_num){
            entry->inode = directory_dst;
        } else {
            entry->inode = n_node;
        }

        // Copiado el bloque    
        memcpy(blocks[directory_dst->i_directos[i]], entry, sizeof(struct directory_entry));

        entry = NULL;
        free(entry);
    }
    else if (!end) {
        printf("No hay espacio en el directorio\n");
    }
}

//Eliminar una entrada de directorio
void remove_entry(char *name, struct inode_fs *directory_dst){
    //Buscar la entrada de directorio
    int i = 0, end = 0, offset  = 0;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){

        // Recorremos el bloque
        memcpy(entry, blocks[directory_dst->i_directos[i]]+offset, sizeof(struct directory_entry));
        for(int j = 1; j < 32 && entry -> inode != NULL && !end; j++){

            // Si es la entrada que buscamos, la eliminamos
            if (strcmp(entry->name, name) == 0){

                //Se elimina la entrada de directorio
                memset(blocks[directory_dst->i_directos[i]]+offset, 0, sizeof(struct directory_entry));
                end = 1;
            } else {
                //Traemos la entrada de directorio
                offset = sizeof(struct directory_entry)*j;
                memcpy(entry, blocks[directory_dst->i_directos[i]]+offset, sizeof(struct directory_entry));
            }
            
        }
        i++;
    }
    if (end){
        free(entry);
        entry = NULL;
    } else {
        printf("No se ha encontrado la entrada de directorio\n");
    }
}

// Función para buscar un inodo en un directorio concreto
struct inode_fs *search_in_directory(char *target, struct inode_fs directory){
    //Creamos el inodo que vamos a devolver
    struct inode_fs *res = malloc(sizeof(struct inode_fs));
    res = NULL;

    //Recorremos el directorio
    int end = 0, offset = 0;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));

    for (int i = 0; i < N_DIRECTOS && directory.i_directos[i] != NULL & !end; i++){

        // Recorremos el bloque
        memcpy(entry, blocks[directory.i_directos[i]]+offset, sizeof(struct directory_entry));
        for (int j = 1; j < 32 && entry -> inode != NULL && !end; j++){
            if (strcmp(entry->name, target) == 0){
                // Si es el inodo que buscamos, lo devolvemos
                res = entry->inode;
                end = 1;
            } else {
                //Traemos la entrada de directorio
                offset = sizeof(struct directory_entry)*j;
                memcpy(entry, blocks[directory.i_directos[i]]+offset, sizeof(struct directory_entry));
            }
        }
    }

    return res;
}

struct inode_fs *search(char *path)
{
    // Parsing path (nuestro path acaba en el directorio del archivo que queremos buscar)
    struct inode_fs *current_inode = malloc(sizeof(struct inode_fs));
    char path_aux[24], cmp_path[500] = "";
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    
    current_inode = search_in_directory(token, *root);
    if (current_inode != NULL) {
        while(token != NULL && current_inode->i_type == 'd'){
            strcat(cmp_path, "/");
            // Buscamos el inodo en el directorio actual
            if(current_inode == NULL) {
                printf("No se ha encontrado el directorio\n");
                return NULL;
            }
            
            // Concatenamos el path
            strcat(cmp_path, current_inode->i_name);
            token = strtok(NULL, "/");
            if (token != NULL) current_inode = search_in_directory(token, *current_inode);
        }   
        
        if (current_inode ->i_type == '-') {
            strcat(cmp_path, "/");
            strcat(cmp_path, current_inode->i_name);
        }

        if(strcmp(cmp_path,path) != 0){
            printf("No existe el fichero\n");
            return NULL;
        }
    }

    return current_inode;
}

struct inode_fs *search_directory(char *path){
    if (strcmp(path, ".") == 0 || strcmp(path, "/") == 0){
        return root;
    }
    struct inode_fs *current_dir = malloc(sizeof(struct inode_fs));
    char path_aux[24];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    current_dir = search_in_directory(token, *root);

    while(token != NULL){
        if(current_dir == NULL || current_dir->i_type != 'd') {
            return NULL;
        } 
        token = strtok(NULL, "/");
        if (token != NULL){
            current_dir = search_in_directory(token, *current_dir);
        }
    }
    return current_dir;
}

