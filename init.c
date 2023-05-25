#include "data_structures_fs.h"

void init_superblock()
{
   private_data -> superblock->block_size = BLOCK_SIZE;
   private_data -> superblock->blocks_count = blocks_for_inodes + blocks_for_inode_bitmap + blocks_for_block_bitmap + 1;
   private_data -> superblock->inode_bitmap_first_block = 1;
   private_data -> superblock->block_bitmap_first_block = private_data -> superblock -> inode_bitmap_first_block + blocks_for_inode_bitmap;
   private_data -> superblock->first_inode_block = private_data -> superblock -> block_bitmap_first_block + blocks_for_block_bitmap;
   private_data -> superblock->first_data_block = private_data -> superblock -> first_inode_block + blocks_for_inodes;
   private_data -> superblock->inode_size = 128;
   private_data -> superblock->inodes_count = 0;
   private_data -> superblock->free_blocks_count = num_blocks - blocks_for_inodes - blocks_for_inode_bitmap - blocks_for_block_bitmap - 1;
   private_data -> superblock->free_inodes_count = num_inodes;
   private_data -> superblock->modified = 'N';
}

void init_block_bitmap()
{
   //Setea a 1 todos los bloques reservados
   for (int i = 0; i < reserved_blocks; i++)
   {
      (void) free_block();
   }
}


