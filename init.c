#include "data_structures_fs.h"

void init_superblock(filesystem_t *private_data)
{
   private_data -> superblock->MAGIC_NUM = 17062023;
   private_data -> superblock->block_size = BLOCK_SIZE;
   private_data -> superblock->blocks_count = private_data->blocks_for_inodes + private_data->blocks_for_inode_bitmap + private_data->blocks_for_block_bitmap + 1;
   private_data -> superblock->inode_bitmap_first_block = 1;
   private_data -> superblock->block_bitmap_first_block = private_data -> superblock -> inode_bitmap_first_block + private_data->blocks_for_inode_bitmap;
   private_data -> superblock->first_inode_block = private_data -> superblock -> block_bitmap_first_block + private_data->blocks_for_block_bitmap;
   private_data -> superblock->first_data_block = private_data -> superblock -> first_inode_block + private_data->blocks_for_inodes;
   private_data -> superblock->inode_size = 128;
   private_data -> superblock->inodes_count = 0;
   private_data -> superblock->free_blocks_count = private_data->num_blocks - private_data->blocks_for_inodes - private_data->blocks_for_inode_bitmap - private_data->blocks_for_block_bitmap - 1;
   private_data -> superblock->free_inodes_count = private_data->num_inodes;
}

void init_block_bitmap(filesystem_t *private_data)

{
   //Setea a 1 todos los bloques reservados
   for (int i = 0; i < private_data->reserved_blocks; i++)
   {
      (void) free_block(private_data);
   }
}


