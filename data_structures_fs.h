
/******************************************************************************************
 *                                   DATA STRUCTURES                                       *
 *******************************************************************************************/
#ifndef DATA_STRUCTURES_FS_H
#define DATA_STRUCTURES_FS_H

#define N_DIRECTOS 10
#define N_SIMPLES 1

#define BLOCK_SIZE 4096

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

long num_blocks;
long num_inodes;
long reserved_blocks;

// Directorios
struct directory_entry
{
    char name[24];
    struct inode_fs *inode;
};

// Inodo
struct inode_fs
{
    int i_num;
    char i_type;
    int i_tam;
    int i_permission;
    int i_links;
    long i_directos[N_DIRECTOS];
    long i_simple_ind[N_SIMPLES];
    uint8_t i_relleno [16];
};

//SUPERBLOQUE
 struct superblock_fs {
    long inodes_count;
    long blocks_count;
    long free_blocks_count;
    long free_inodes_count;
    long first_data_block;
    long first_inode_block;
    long block_bitmap_first_block;
    long inode_bitmap_first_block;
    long block_size;
    long inode_size;
    char modified;
 };

// BITMAP DE BLOQUES
struct block_bitmap_fs
{
    uint8_t *bitmap;
};

// BITMAP DE INODOS
struct inode_bitmap_fs
{
    uint8_t *bitmap;
};

// Estructura filesystem
typedef struct{
    struct superblock_fs *superblock;
    struct inode_bitmap_fs *inode_bitmap;
    struct block_bitmap_fs *block_bitmap;
    struct inode_fs *inode;
    unsigned char *data_block;
    int fd;
} filesystem_t;

filesystem_t *private_data;

/******************************************************************************************
 *                                   FUNCTIONS                                             *
 *******************************************************************************************/

// bitmap.c
int free_inode();
void remove_inode_bitmap(int);
long free_block();
void remove_block_bitmap(long);

// inode.c
struct inode_fs *create_inode(char, char *);
struct inode_fs *create_root();
void remove_inode(struct inode_fs *);
void remove_entry(char *, struct inode_fs *);
void clean_inode(struct inode_fs *);
//block_list get_blocks_indirect(long);
void add_block_indirect(long, long);
long create_block();

// file_manager.c
char *get_directory(char *);
void touch(char *, char);
void print_directory(struct inode_fs);
void unlink_fs(char *, char *);
void rmdir_fs(char *);
int append(char *, char *);
int overwrite(char *, char *);
char *read_file(char *);
void rename_file(char *, char *);

// tree_manager.c
void insert(char *, struct inode_fs *, struct inode_fs *);
struct inode_fs *search_in_directory(char *, struct inode_fs);
struct inode_fs *search(char *);
struct inode_fs *search_directory(char *);

// init.c
void init_superblock();

#endif
