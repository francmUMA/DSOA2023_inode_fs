
/******************************************************************************************
*                                   DATA STRUCTURES                                       *  
*******************************************************************************************/

#define N_DIRECTOS 10
#define N_SIMPLES 1
// #define N_DOBLES 1
// #define N_TRIPLES 1

typedef struct
{
    int id_node;
    Name f_nombre;
} file_register;

typedef char Name[32];


//Lista de bloques libres
typedef struct Block *Free_blocks_list; // Mapa de bloques libres

//Descriptor de bloque
struct Block{
    void *memory_address;
    int block_size;
    // file_register files[32];
    Free_blocks_list next;
};

//Lista de inodos libres
typedef struct Inode *Free_inodes_list;

//Inodo
struct Inode{
    char i_type;
    int i_tam;
    // int i_permission;
    int i_directos[N_DIRECTOS];
    int i_simple_ind[N_SIMPLES];
    // int i_double_ind[N_DOBLES];
    // int i_triple_ind[N_TRIPLES];
    char i_relleno [20]; // Darle vueltas
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
    unsigned char MODIFIED; // Por si es modificado el superbloque
};


