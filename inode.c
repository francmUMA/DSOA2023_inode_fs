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
    // Recorremos cada bloque y pones cada entrada a 0
    for(int i = 0; i < N_DIRECTOS && inode -> i_directos[i] != NULL; i++){
        memset(inode -> i_directos[i], 0, 1024);
        inode -> i_directos[i] = NULL;
    }

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
        i++;
    } 

    if(!end && i < N_DIRECTOS)
    {
        // Estamos ante un bloque vacío
        buffer[0] = contenido;
        file->i_directos[i] = malloc(sizeof(char) * 1024);
        memcpy(file->i_directos[i], buffer, 1024);
        // Liberamos el espacio reservado para el buffer
        free(buffer);
        buffer = NULL;
    }else if(i == N_DIRECTOS)
    {
        printf("XUPALA NO HAY ESPACIO\n");
    }
    
}