#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Creacion de un inodo asignando el primer inodo libre en el bitmap de inodos
//Se devuelve el inodo creado
//Si no hay inodos libres, se devuelve NULL
//Hay que buscar el primer inodo libre con la funcion creada en bitmap.c
//Se utiliza la estructura struct inode_fs para crear el inodo y se le pasan los campos por parametro

struct inode_fs *create_inode(char type, char *name,  struct inode_bitmap_fs *inode_bitmap){
    struct inode_fs *new_inode = malloc(sizeof(struct inode_fs));
    new_inode -> i_type = type;
    new_inode -> i_tam = 0;
    strcpy(new_inode -> i_name, name);
    new_inode -> i_num = free_inode(inode_bitmap);
    return new_inode;
}

//Creación del inodo root
struct inode_fs *create_root(struct inode_bitmap_fs *inode_bitmap){
    struct inode_fs *root = create_inode('d', "/", inode_bitmap);
    insert(".", root, root);
    return root;
}

// Eliminación de un inodo
void remove_inode(struct inode_fs *inode, struct inode_bitmap_fs *inode_bitmap){
    // Eliminamos el inodo del bitmap
    remove_inode_bitmap(inode_bitmap, inode->i_num);
    
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
        memcpy(buffer, file->i_directos[i], 1024);
        if(strlen(buffer) < 1024){
            buffer[strlen(buffer)] = contenido;
            memcpy(file->i_directos[i], buffer, 1024);
            end = 1;
        }
        memset(buffer, 0, 1024);
        i++;
    } 

    if(!end && i < N_DIRECTOS) {
        // Estamos ante un bloque vacío
        buffer[0] = contenido;
        file->i_directos[i] = malloc(sizeof(char) * 1024);
        memcpy(file->i_directos[i], buffer, 1024);
        // Liberamos el espacio reservado para el buffer
        free(buffer);
        buffer = NULL;
    } else if(!end && i == N_DIRECTOS) {
        // Estamos ante un bloque indirecto
        // Comprobar si hay algun puntero indirecto
        if (file->i_simple_ind[0] == NULL){
            long direct_pointers_block = malloc(1024);

            //Guardar el bloque en el inodo
            file->i_simple_ind[0] = direct_pointers_block;
        }

        // Obtener todos los bloques directos del puntero indirecto
        block_list *blocks = get_blocks_indirect(file->i_simple_ind[0]);

        //Encontrar el primer bloque con hueco
        //Se crea un buffer donde se guarda el contenido del bloque
        char *buffer = malloc(sizeof(char)*1024);

        // Se iteraa la lista con un puntero auxiliar
        block_list aux = (*blocks);
        memcpy(buffer, aux->block, 1024);
        while (aux != NULL && strlen(buffer) == 1024){
            aux = aux->next;
            if (aux != NULL) memcpy(buffer, aux->block, 1024);
        }

        // Si aux es NULL, no hay bloques libres
        // Creamos un bloque nuevo con el caracter
        if (aux == NULL){
            //TODO: hay que comprobar si hay espacio en el puntero indirecto
            char *new_block = malloc(sizeof(char)*1024);
            new_block[0] = contenido;
            // Se añade el puntero del bloque al bloque indirecto
            //TODO: Es una funcion que dado un puntero indirecto y un bloque directo, lo añade al puntero indirecto
            add_block_indirect(file->i_simple_ind[0], new_block);
        } else {
            // Si aux no es NULL, hay un bloque con hueco
            // Se añade el caracter al bloque
            buffer[strlen(buffer)] = contenido;
            memcpy(aux->block, buffer, 1024);
            free(buffer);
        }
    }
    
}

void clean_inode(struct inode_fs *file) {
    // Limpiamos primero los directos
    for(int i = 0; i < N_DIRECTOS && file -> i_directos[i] != NULL; i++){
        memset(file -> i_directos[i], 0, 1024);
        file -> i_directos[i] = NULL;
    }
}

// Función que trae los bloques de los punteros indirectos
block_list *get_blocks_indirect(long i_indirecto)
{
    block_list blocks = malloc(sizeof(struct block_list));
    
    int i = 0;
    long current = malloc(sizeof(long));
    long next = malloc(sizeof(long));

    int offset = i * sizeof(long);

    memcpy(current, i_indirecto + offset, sizeof(long));
    if(current == NULL){
        return NULL;
    }
    i++;
    memcpy(next, i_indirecto + offset, sizeof(long));
    blocks -> block = current;
    blocks -> next = NULL;
    block_list aux = blocks;
    while(next != NULL){
        aux->block = current;
        aux->next = next;
        current = next;
        aux = aux->next;
        i++;
        memcpy(next, i_indirecto + offset, sizeof(long));
    }

    aux->block = current;
    aux->next = NULL;
    //blocks = aux;
    
    return &blocks;
}

// Función que añade un bloque al puntero indirecto
void add_block_indirect(long indirect_pointer, long direct_pointer){
    // Localizar el primer hueco en el puntero indirecto
    int i = 0;
    int offset = i * sizeof(long);
    long pointer = malloc(sizeof(long));
    memcpy(pointer, indirect_pointer + offset, sizeof(long));
    while(pointer != NULL){
        i++;
        memcpy(pointer, indirect_pointer + offset, sizeof(long));
    }
    //Añadir el bloque al puntero indirecto
    memcpy(indirect_pointer + offset, direct_pointer, sizeof(long));
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






