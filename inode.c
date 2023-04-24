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
