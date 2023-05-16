
/******************************************************************************************
 *                                   DATA STRUCTURES                                       *
 *******************************************************************************************/
#ifndef DATA_STRUCTURES_FS_H
#define DATA_STRUCTURES_FS_H

#define N_DIRECTOS 10
#define N_SIMPLES 1
#define N_DOBLES 1
#define N_TRIPLES 1

#define NUM_BLOCKS 1000
#define NUM_INODES 1000

// Directorios
struct directory_entry
{
    char name[24];
    struct inode_fs *inode;
};

// Lista de bloques libres
// typedef struct block *Free_blocks_list;

// Descriptor de bloque
//  struct block{
//      long block_number;
//      int block_size;
//      Free_blocks_list next;
//  };

// Lista de inodos libres
// typedef struct inode *Free_inodes_list;

// Bloque directo
typedef struct block_list *block_list;

struct block_list
{
    long block_index;
    block_list *next;
};

// Inodo
struct inode_fs
{
    int i_num;
    char i_type;
    char i_name[24];
    int i_tam;
    int i_permission;
    int i_links;
    long i_directos[N_DIRECTOS];
    long i_simple_ind[N_SIMPLES];
    // long i_double_ind[N_DOBLES];
    // long i_triple_ind[N_TRIPLES];
    // char i_relleno [20];
};

static long *blocks[NUM_BLOCKS];

// SUPERBLOQUE
//  struct superblock {
//      long free_blocks;
//      Free_blocks_list free_blocks_list;
//      long inode_list_size;
//      Free_inodes_list free_inodes_list;
//      unsigned char MODIFIED; // Por si es modificado el superbloque
//  };

// BITMAP DE BLOQUES
struct block_bitmap_fs
{
    unsigned char bitmap[NUM_BLOCKS / 8];
};

// BITMAP DE INODOS
struct inode_bitmap_fs
{
    unsigned char bitmap[NUM_INODES / 8];
};

struct inode_bitmap_fs *inode_bitmap;
struct block_bitmap_fs *block_bitmap;
struct inode_fs *root;

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
block_list get_blocks_indirect(long);
void add_block_indirect(long, long);
long create_block();

// file_manager.c
char *get_directory(char *);
void touch(char *, char);
void print_directory(struct inode_fs);
void unlink(char *, char *);
void rmdir(char *);
int append(char *, char *);
int overwrite(char *, char *);
char *read_file(char *);
void rename_file(char *, char *);

// tree_manager.c
void insert(char *, struct inode_fs *, struct inode_fs *);
struct inode_fs *search_in_directory(char *, struct inode_fs);
struct inode_fs *search(char *);
struct inode_fs *search_directory(char *);

#endif
