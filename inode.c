#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Creacion de un inodo asignando el primer inodo libre en el bitmap de inodos
//Se devuelve el inodo creado
//Si no hay inodos libres, se devuelve NULL
//Hay que buscar el primer inodo libre con la funcion creada en bitmap.c
//Se utiliza la estructura struct inode_fs para crear el inodo y se le pasan los campos por parametro

struct inode_fs *create_inode(char type, char *name){
    struct inode_fs *new_inode = malloc(sizeof(struct inode_fs));
    new_inode -> i_type = type;
    new_inode -> i_tam = 0;
    strcpy(new_inode -> i_name, name);
    new_inode -> i_num = free_inode();
    return new_inode;
}

//Creación del inodo root
struct inode_fs *create_root(){
    struct inode_fs *root = create_inode('d', "/");
    insert(".", root, root);
    return root;
}

// Eliminación de un inodo
void remove_inode(struct inode_fs *inode){
    // Eliminamos el inodo del bitmap
    remove_inode_bitmap(inode->i_num);
    
    // Limpiar todos los bloques del inodo
    clean_inode(inode);

    //Liberamos el inodo
    free(inode);
    inode = NULL;
}

// Añadir carácter
void add_char_to_inode(struct inode_fs *file, char contenido)
{
    int i = 0, end = 0;
    char *buffer = malloc(sizeof(char)*1024);
    while(i < N_DIRECTOS && file->i_directos[i] != NULL && !end){
        memcpy(buffer, blocks[file->i_directos[i]], 1024); // Nos traemos nuestro bloque en la pos file->i_directos[i]
        if(strlen(buffer) < 1024){
            buffer[strlen(buffer)] = contenido;
            memcpy(blocks[file->i_directos[i]], buffer, 1024); // Guardamos el bloque modificado
            end = 1;
        }
        memset(buffer, 0, 1024);
        i++;
    }

    if(!end && i < N_DIRECTOS) {
        // Estamos ante un bloque vacío
        buffer[0] = contenido;
        file->i_directos[i] = create_block();
        memcpy(blocks[file->i_directos[i]], buffer, 1024);
        // Liberamos el espacio reservado para el buffer
        free(buffer);
        buffer = NULL;
    } else if(!end && i == N_DIRECTOS) {
        // Estamos ante un bloque indirecto
        // Comprobar si hay algun puntero indirecto
        if (file->i_simple_ind[0] == NULL){

            //Crear bloque de punteros directos
            file->i_simple_ind[0] = create_block();
        }

        // Obtener todos los bloques directos del puntero indirecto
        block_list blocks_aux = get_blocks_indirect(file->i_simple_ind[0]);

        // Se itera la lista con un puntero auxiliar
        block_list aux = blocks_aux;
        
        if (aux != NULL) memcpy(buffer, blocks[aux->block_index], 1024);

        while (aux != NULL && strlen(buffer) == 1024){
            aux = aux->next;
            if (aux != NULL) memcpy(buffer, blocks[aux->block_index], 1024);
        }

        // Si aux es NULL, no hay bloques libres
        // Creamos un bloque nuevo con el caracter
        if (aux == NULL){
            // TODO: hay que comprobar si hay espacio en el puntero indirecto
            long new_block = create_block();
            blocks[new_block][0] = contenido;
            // Se añade el puntero del bloque al bloque indirecto
            //TODO: Es una funcion que dado un puntero indirecto y un bloque directo, lo añade al puntero indirecto
            add_block_indirect(file->i_simple_ind[0], new_block);
        } else {
            // Si aux no es NULL, hay un bloque con hueco
            // Se añade el caracter al bloque
            buffer[strlen(buffer)] = contenido;
            memcpy(blocks[aux->block_index], buffer, 1024);
        }
        free(buffer); 
    }else{
        free(buffer);
    }
}

void clean_inode(struct inode_fs *file) {
    // Limpiamos primero los directos
    for(int i = 0; i < N_DIRECTOS && file -> i_directos[i] != NULL; i++){
        memset(blocks[file -> i_directos[i]], 0, 1024);
        file -> i_directos[i] = NULL;
    }
    file -> i_tam = 0;
}

// Función que trae los bloques de los punteros indirectos
block_list get_blocks_indirect(long i_indirecto)
{
    //TODO
    ;
}

// Función que añade un bloque al puntero indirecto
void add_block_indirect(long indirect_pointer, long direct_pointer){
    // Localizar el primer hueco en el puntero indirecto
    int i = 0; // Posicion dentro del bloque
    int offset = 0; // Tamaño de un puntero
    long block_index;
    // Nos traemos el bloque
    memcpy(&block_index, blocks[indirect_pointer] + offset, sizeof(long));
    while(block_index != NULL){
        i++;
        offset = sizeof(long)*i;
        memcpy(&block_index, blocks[indirect_pointer] + offset, sizeof(long));
    }
    // Añadimos el bloque al puntero indirecto
    memcpy(blocks[indirect_pointer] + offset, &direct_pointer, sizeof(long));
}

// Función que trae los bloques de los punteros indirectos dobles
block_list get_blocks_indirect_double(long i_double_indirecto)
{
    ;
}

// Función que trae los bloques de los punteros indirectos triples
block_list get_blocks_indirect_triple(long i_triple_indirecto)
{
    ;
}

long create_block(){
    long *block = malloc(1024);
    memset(block, 0, 1024);
    long index = free_block();
    blocks[index] = block;
    return index;
}

int remove_block(long index)
{
    if(index < 0 || index > NUM_BLOCKS){
        return -1;
    }
    blocks[index] = NULL;
    remove_block_bitmap(index);
    return 0;
}




