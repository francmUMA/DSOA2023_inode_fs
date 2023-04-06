#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct superblock *create(long n_free_blocks, long inode_list_sz)
{
   /* Solo le pasamos dos parámetros a nuestra función.
   */
   struct superblock *new_sBlock = malloc(sizeof(struct superblock));
   new_sBlock->free_blocks = n_free_blocks;
   // El tamaño (entiendo que) será el tamaño del block_bitmap multiplicado por el tamaño de bloque
   new_sBlock->free_blocks_list = malloc(sizeof(struct block) * n_free_blocks);
   // new_sBlock->next_free_block = *new_sBlock->free_blocks_list;
   new_sBlock->inode_list_size = inode_list_sz;
   new_sBlock->free_inodes_list = malloc(sizeof(struct inode) * inode_list_sz);
   new_sBlock->MODIFIED = "N"; // Una N de no, porque lo acabamos de crear
   return new_sBlock;
}



