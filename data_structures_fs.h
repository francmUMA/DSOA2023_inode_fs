
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
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Directorios
struct directory_entry
{
    char name[24];
    long inode; // Cambiar a long para que sea el número de inodo
};

// Inodo
struct inode_fs
{
    long i_num;
    char i_type;
    int i_tam;
    int i_permission;
    int i_links;
    struct directory_entry *entry;
    long i_directos[N_DIRECTOS];
    long i_simple_ind[N_SIMPLES];
    uint8_t i_relleno [4];
};

//SUPERBLOQUE
 struct superblock_fs {
    int MAGIC_NUM;
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
 };

typedef uint8_t block_t [BLOCK_SIZE];

// Estructura filesystem
typedef struct{
    struct superblock_fs *superblock;
    uint8_t *inode_bitmap;
    uint8_t *block_bitmap;
    struct inode_fs *inode;
    block_t *block;
    int fd;
    char* fichero;
    struct timespec st_atim;  				/* fechas del fichero */
    struct timespec st_mtim; 
    struct timespec st_ctim;  
    uid_t     st_uid;        				/* El usuario y grupo */
    gid_t     st_gid;  
    long num_blocks;
    long num_inodes;
    long reserved_blocks;
    long blocks_for_inodes;
    long blocks_for_inode_bitmap;
    long blocks_for_block_bitmap;
} filesystem_t;

/******************************************************************************************
 *                                   FUNCTIONS                                             *
 *******************************************************************************************/

// bitmap.c
int free_inode(filesystem_t *);
void remove_inode_bitmap(long, filesystem_t *);
long free_block(filesystem_t *);
void remove_block_bitmap(long, filesystem_t *);

// inode.c
struct inode_fs *create_inode(char, char *, filesystem_t *);
void create_root(filesystem_t *);
void remove_inode(struct inode_fs *, filesystem_t *);
void clean_inode(struct inode_fs *, filesystem_t *);
// block_list get_blocks_indirect(long);
// void add_block_indirect(long, long);
long create_block(filesystem_t *);
int remove_block(long, filesystem_t *);

// file_manager.c
char *get_directory(char *);
void touch(char *, char, filesystem_t *);
void print_directory(struct inode_fs, filesystem_t *);
void unlink_fs(char *, filesystem_t *);
void rmdir_fs(char *, filesystem_t *);
int append(char *, char *, filesystem_t *);
int overwrite(char *, char *, filesystem_t *);
char *read_file(char *, filesystem_t *);
void rename_file(char *, char *, filesystem_t *);

// tree_manager.c
void insert(char *, struct inode_fs *, struct inode_fs *, filesystem_t *);
struct inode_fs *search_in_directory(char *, struct inode_fs, filesystem_t *);
struct inode_fs *search(char *, filesystem_t *);
struct inode_fs *search_directory(char *, filesystem_t *);
void remove_entry(char *, struct inode_fs *, filesystem_t *);

// init.c
void init_superblock(filesystem_t *);
void init_block_bitmap(filesystem_t *);

#endif
