#include <stdio.h>
#include <stdint.h>
#include "data_structures_fs.h"

/*
    Funciones para el uso de los bitmaps

    free_inode: devuelve el primer inodo libre del bitmap de inodos
    free_block: devuelve el primer bloque libre del bitmap de bloques

*/

//Devuelve el primer inodo libre del bitmap de inodos
//Cada bit del bitmap de inodos representa un inodo, es decir, cada posicion del bitmap tiene 8 inodos.
//El primer inodo es el 1.
//Primero se busca la primera posicion que tiene un byte diferente de 0xFF, es decir, que tenga un 0.
//Hay un total de NUM_INODES inodos, por lo que se divide entre 8 para saber el numero de bytes que hay que recorrer.

int free_inode(filesystem_t *private_data){
    int byte = 0; 

    //Encontramos el primer byte que no es 0xFF
    while(byte < private_data->num_inodes/8 && (private_data -> inode_bitmap[byte] == 0xFF)){
        byte++;
    }

    //Ahora buscamos el primer bit que es 0
    int bit = 7;

    //Creamos un auxiliar por eficiencia
    unsigned char aux = private_data -> inode_bitmap[byte];
    while(bit >= 0 && (((aux >> bit) & 1) != 0)){                   
        bit--;
    }
    private_data -> inode_bitmap[byte] |= (1 << bit);

    private_data -> inode_bitmap[byte] |= (1 << bit);
    private_data -> superblock->inodes_count++;
    private_data -> superblock->free_inodes_count--;

    return (byte * 8) + (8 - bit) - 1;
}

//Limpiar un inodo del bitmap de inodos
void remove_inode_bitmap(long inode, filesystem_t *private_data){
    int byte = inode / 8;
    int bit = 7 - (inode % 8);
    private_data -> inode_bitmap[byte] &= ~(1 << bit);

    private_data -> superblock -> inodes_count--;
    private_data -> superblock -> free_inodes_count++;
}

long free_block(filesystem_t *private_data){
    int byte = 0; 

    //Encontramos el primer byte que no es 0xFF
    while(byte < private_data->num_blocks/8 && (private_data -> block_bitmap[byte]) == 0xFF){
        byte++;
    }

    //Ahora buscamos el primer bit que es 0
    int bit = 7;

    //Creamos un auxiliar por eficiencia
    unsigned char aux = private_data -> block_bitmap[byte];
    while(bit >= 0 && (((aux >> bit) & 1) != 0)){                   
        bit--;
    }
    private_data -> block_bitmap[byte] |= (1 << bit);
    
    private_data -> superblock -> blocks_count++;
    private_data -> superblock -> free_blocks_count--;     

    return (long) ((byte * 8) + (8 - bit)) - 1;
}

//Limpiar un inodo del bitmap de inodos
void remove_block_bitmap(long block, filesystem_t *private_data){
    int byte = block / 8;
    int bit = 7 - (block % 8);
    private_data -> block_bitmap[byte] &= ~(1 << bit);

    private_data -> superblock -> blocks_count--;
    private_data -> superblock -> free_blocks_count++;
}
