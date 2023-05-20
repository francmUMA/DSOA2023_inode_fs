#include "data_structures_fs.h"

void create()
{
   superblock->block_size = BLOCK_SIZE;
   superblock->blocks_count = 0;
   superblock->inode_bitmap_first_block = 1;
   superblock->block_bitmap_first_block = 2;
   superblock->first_inode_block = 34;
   superblock->first_data_block = 66;
   superblock->inode_size = 128;
   superblock->inodes_count = 0;
   superblock->free_blocks_count = NUM_BLOCKS;
   superblock->free_inodes_count = NUM_INODES;
   superblock->modified = 'N';
}
