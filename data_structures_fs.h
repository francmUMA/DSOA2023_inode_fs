
/******************************************************************************************
*                                   DATA STRUCTURES                                       *  
*******************************************************************************************/

#define N_DIRECTOS 10
#define N_SIMPLES 1
#define N_DOBLES 1
#define N_TRIPLES 1

#define NUM_BLOCKS 1000
#define NUM_INODES 1000

//Directorios
typedef struct { 
    char name[24];
    struct inode *inode;
} directory_entry;


//Lista de bloques libres
typedef struct block *Free_blocks_list; 

//Descriptor de bloque
struct block{
    void *memory_address;
    int block_size;
    Free_blocks_list next;
};

//Lista de inodos libres
typedef struct inode *Free_inodes_list;

//Inodo
struct inode{
    char i_type;
    int i_tam;
    // int i_permission;
    int i_directos[N_DIRECTOS];
    int i_simple_ind[N_SIMPLES];
    // int i_double_ind[N_DOBLES];
    // int i_triple_ind[N_TRIPLES];
    char i_relleno [20]; 
};


//SUPERBLOQUE
struct superblock {

    long free_blocks;
    Free_blocks_list free_blocks_list;
    struct block next_free_block;
    long inode_list_size;
    Free_inodes_list free_inodes_list;
    struct inode next_free_inode;
    unsigned char MODIFIED; // Por si es modificado el superbloque
};

//BITMAP DE BLOQUES
struct block_bitmap {
    unsigned char bitmap[NUM_BLOCKS / 8];       //Se divide entre 8 porque cada byte tiene 8 bits, por lo que sería un array de 125 bytes.
};
  
//BITMAP DE INODOS
struct inode_bitmap{
    unsigned char bitmap[NUM_INODES / 8]; 
};

