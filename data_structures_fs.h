
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

//Directorios
struct directory_entry{ 
    char name[24];
    struct inode_fs *inode;
};


//Lista de bloques libres
//typedef struct block *Free_blocks_list; 

//Descriptor de bloque
// struct block{
//     long block_number;
//     int block_size;
//     Free_blocks_list next;
// };

//Lista de inodos libres
//typedef struct inode *Free_inodes_list;

//Inodo
struct inode_fs{
    int i_num;
    char i_name[24]; // Preguntar
    char i_type;
    int i_tam;
    // int i_permission;
    long i_directos[N_DIRECTOS]; 
    long i_simple_ind[N_SIMPLES];
    // long i_double_ind[N_DOBLES];
    // long i_triple_ind[N_TRIPLES];
    // char i_relleno [20]; 
};


//SUPERBLOQUE
// struct superblock {
//     long free_blocks;
//     Free_blocks_list free_blocks_list;
//     long inode_list_size;
//     Free_inodes_list free_inodes_list;
//     unsigned char MODIFIED; // Por si es modificado el superbloque
// };

//BITMAP DE BLOQUES
// struct block_bitmap_fs {
//     unsigned char bitmap[NUM_BLOCKS / 8];       
// };
  
//BITMAP DE INODOS
struct inode_bitmap_fs{
    unsigned char bitmap[NUM_INODES / 8]; 
};

/******************************************************************************************
*                                   FUNCTIONS                                             *  
*******************************************************************************************/

//bitmap.c
int free_inode(struct inode_bitmap_fs *);

//create_inode.c
struct inode_fs *create_inode(char , char *,  struct inode_bitmap_fs *);
struct inode_fs *create_root(struct inode_bitmap_fs *);

//file_manager.c
void touch(char *, char , char *,  struct inode_fs *, struct inode_bitmap_fs *);
void print_directory(struct inode_fs );

//tree_manager.c
struct inode_fs *inode_search(char *, struct inode_fs *);
void insert(char *, struct inode_fs *, struct inode_fs );

#endif
