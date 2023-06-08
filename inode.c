#include "data_structures_fs.h"

//Creacion de un inodo asignando el primer inodo libre en el bitmap de inodos
//Se devuelve el inodo creado
//Si no hay inodos libres, se devuelve NULL
//Hay que buscar el primer inodo libre con la funcion creada en bitmap.c
//Se utiliza la estructura struct inode_fs para crear el inodo y se le pasan los campos por parametro

long create_inode(char type, char *name, filesystem_t *private_data){
    long inode_num = free_inode(private_data);
    private_data->inode[inode_num].i_type = type;
    if (type == 'd'){
        private_data->inode[inode_num].i_permission = S_IFDIR | 0755;
    } else if (type == '-'){
        private_data->inode[inode_num].i_permission = S_IFREG | 0666;
    }
    private_data->inode[inode_num].i_links = 0;
    private_data->inode[inode_num].i_tam = 0;
    private_data->inode[inode_num].i_num = inode_num;

    return inode_num;
}

//Creación del inodo root 
void create_root(filesystem_t *private_data){
    long root = create_inode('d', "/", private_data);
    insert(".", root, root, private_data);
    insert("..", root, root, private_data);
}

// Eliminación de un inodo
void remove_inode(long inode_number, filesystem_t *private_data){
    // Eliminamos el inodo del bitmap
    remove_inode_bitmap(inode_number, private_data);
    
    // Limpiar todos los bloques del inodo
    clean_inode(inode_number, private_data);

    //Liberamos el inodo
    memset(&private_data->inode[inode_number], 0, sizeof(struct inode_fs));
}

// Añadir carácter
void add_char_to_inode(long file_number, char contenido, filesystem_t *private_data)
{
    int i = 0, end = 0;
    char *buffer = malloc(sizeof(char)*4096);
    while(i < N_DIRECTOS && private_data->inode[file_number].i_directos[i] != 0 && !end){
        strcpy(buffer, private_data -> block[private_data->inode[file_number].i_directos[i]]); // Nos traemos nuestro bloque en la pos file->i_directos[i]
        if(strlen(buffer) < 4096){
            buffer[strlen(buffer)] = contenido;
            strcpy(private_data -> block[private_data->inode[file_number].i_directos[i]], buffer); // Guardamos el bloque modificado
            end = 1;
        }
        memset(buffer, 0, 4096);
        i++;
    }

    if(!end && i < N_DIRECTOS) {
        // Estamos ante un bloque vacío
        buffer[0] = contenido;
        private_data->inode[file_number].i_directos[i] = create_block(private_data);
        strcpy(private_data -> block[private_data->inode[file_number].i_directos[i]], buffer);
    } 
    // Liberamos el espacio reservado para el buffer
    free(buffer);
    buffer = NULL;
}

void clean_inode(long file_number, filesystem_t *private_data) {
    // Limpiamos primero los directos
    for(int i = 0; i < N_DIRECTOS && private_data->inode[file_number].i_directos[i] != 0; i++){
        memset(private_data -> block[private_data->inode[file_number].i_directos[i]], 0 , BLOCK_SIZE);
        private_data->inode[file_number].i_directos[i] = (long int) NULL;
    }
    private_data->inode[file_number].i_tam = 0;
}

long create_block(filesystem_t *private_data) {
    return free_block(private_data);
}


int remove_block(long index, filesystem_t *private_data)
{
    if(index < 0 || index > private_data->num_blocks){
        return -1;
    }
    memset(private_data -> block[index], 0, BLOCK_SIZE);
    remove_block_bitmap(index, private_data);
    return 0;
}




