
/******************************************************************************************
*                                   DATA STRUCTURES                                       *  
*******************************************************************************************/

//Lista de bloques libres
typedef struct Block *Free_blocks_list;

//Descriptor de bloque
struct Block{
    void *memory_address;
    int block_size;
    Free_blocks_list next;
};

//Lista de inodos libres
typedef struct Inode *Free_inodes_list;

//Inodo
struct Inode{
    ;
};


//SUPERBLOQUE
struct Superblock {
    int size;
    long free_blocks;
    Free_blocks_list free_blocks_list;
    Block next_free_block;
    long inode_list_size;
    Free_inodes_list free_inodes_list;
    Inode next_free_inode;
    unsigned char MODIFIED;
};

