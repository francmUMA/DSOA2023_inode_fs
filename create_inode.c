#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//Creacion de un inodo asignando el primer inodo libre en el bitmap de inodos
//Se devuelve el inodo creado
//Si no hay inodos libres, se devuelve NULL
//Hay que buscar el primer inodo libre con la funcion creada en bitmap.c
//Se utiliza la estructura struct inode para crear el inodo y se le pasan los campos por parametro

struct inode *create_inode(char type, char *name,  struct inode_bitmap *inode_bitmap){
    struct inode *new_inode = malloc(sizeof(struct inode));
    int i_directos[10], i_simples[1];
    new_inode -> i_type = type;
    new_inode -> i_tam = 0;
    strcpy(new_inode -> i_name, name);
    new_inode -> i_num = free_inode(inode_bitmap);
    new_inode -> i_directos = i_directos;
    new_inode -> i_simple_ind = i_simples;
    return new_inode;
}
