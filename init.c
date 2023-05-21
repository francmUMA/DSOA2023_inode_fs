#include "data_structures_fs.h"

void init_superblock()
{
   long blocks_for_inodes = num_inodes * sizeof(struct inode_fs);
   long blocks_for_inode_bitmap = (num_inodes / 8) / BLOCK_SIZE + 1;
   long blocks_for_block_bitmap = ((num_blocks - blocks_for_inode_bitmap - blocks_for_inodes - 1) / 8) / BLOCK_SIZE + 1;

   private_data -> superblock->block_size = BLOCK_SIZE;
   private_data -> superblock->blocks_count = 1;
   private_data -> superblock->inode_bitmap_first_block = 1;
   private_data -> superblock->block_bitmap_first_block = private_data -> superblock -> inode_bitmap_first_block + blocks_for_inode_bitmap;
   private_data -> superblock->first_inode_block = private_data -> superblock -> block_bitmap_first_block + blocks_for_block_bitmap;
   private_data -> superblock->first_data_block = private_data -> superblock -> first_inode_block + blocks_for_inodes;
   private_data -> superblock->inode_size = 128;
   private_data -> superblock->inodes_count = 0;
   private_data -> superblock->free_blocks_count = num_blocks - (private_data -> superblock->first_data_block);
   private_data -> superblock->free_inodes_count = num_inodes;
   private_data -> superblock->modified = 'N';
}


