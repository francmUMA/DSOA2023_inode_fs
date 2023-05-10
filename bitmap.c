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

int free_inode(){
    int byte = 0; 

    //Encontramos el primer byte que no es 0xFF
    while(byte < NUM_INODES/8 && ((*inode_bitmap).bitmap[byte]) == 0xFF){
        byte++;
    }

    //Ahora buscamos el primer bit que es 0
    int bit = 7;

    //Creamos un auxiliar por eficiencia
    unsigned char aux = (*inode_bitmap).bitmap[byte];
    while(bit >= 0 && (((aux >> bit) & 1) != 0)){                   
        bit--;
    }
    (*inode_bitmap).bitmap[byte] |= (1 << bit);
    return (byte * 8) + (8 - bit);
}

//Limpiar un inodo del bitmap de inodos
void remove_inode_bitmap(int inode){
    int byte = inode / 8;
    int bit = 7 - (inode % 8);
    (*inode_bitmap).bitmap[byte] &= ~(1 << bit);
}

long free_block(){
    int byte = 0; 

    //Encontramos el primer byte que no es 0xFF
    while(byte < NUM_BLOCKS/8 && ((*block_bitmap).bitmap[byte]) == 0xFF){
        byte++;
    }

    //Ahora buscamos el primer bit que es 0
    int bit = 7;

    //Creamos un auxiliar por eficiencia
    unsigned char aux = (*block_bitmap).bitmap[byte];
    while(bit >= 0 && (((aux >> bit) & 1) != 0)){                   
        bit--;
    }
    (*block_bitmap).bitmap[byte] |= (1 << bit);
    return (long) (byte * 8) + (8 - bit);
}

//Limpiar un inodo del bitmap de inodos
void remove_block_bitmap(long block){
    int byte = block / 8;
    int bit = 7 - (block % 8);
    (*block_bitmap).bitmap[byte] &= ~(1 << bit);
}
