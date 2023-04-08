
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
    long block_number;
    int block_size;
    Free_blocks_list next;
};

//Lista de inodos libres
typedef struct inode *Free_inodes_list;

//Inodo
struct inode{
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
struct superblock {
    long free_blocks;
    Free_blocks_list free_blocks_list;
    long inode_list_size;
    Free_inodes_list free_inodes_list;
    unsigned char MODIFIED; // Por si es modificado el superbloque
};

//BITMAP DE BLOQUES
struct block_bitmap {
    unsigned char bitmap[NUM_BLOCKS / 8];       
};
  
//BITMAP DE INODOS
struct inode_bitmap{
    unsigned char bitmap[NUM_INODES / 8]; 
};


