#include "data_structures_fs.h"

//Creacion de un inodo asignando el primer inodo libre en el bitmap de inodos
//Se devuelve el inodo creado
//Si no hay inodos libres, se devuelve NULL
//Hay que buscar el primer inodo libre con la funcion creada en bitmap.c
//Se utiliza la estructura struct inode_fs para crear el inodo y se le pasan los campos por parametro

struct inode_fs *create_inode(char type, char *name, filesystem_t *private_data){
    long inode_num = free_inode(private_data);
    struct inode_fs *new_inode = &(private_data -> inode[inode_num]);
    new_inode -> i_type = type;
    if (type == 'd'){
        new_inode -> i_permission = 0755;
    } else if (type == '-'){
        new_inode -> i_permission = 0644;
    }
    new_inode -> i_links = 0;
    new_inode -> i_tam = 0;
    new_inode -> i_num = inode_num;

    return new_inode;
}

//Creación del inodo root 
void create_root(filesystem_t *private_data){
    struct inode_fs *root = create_inode('d', "/", private_data);
    insert(".", root, root, private_data);
    insert("..", root, root, private_data);
}

// Eliminación de un inodo
void remove_inode(struct inode_fs *inode, filesystem_t *private_data){
    // Eliminamos el inodo del bitmap
    remove_inode_bitmap(inode->i_num, private_data);
    
    // Limpiar todos los bloques del inodo
    clean_inode(inode, private_data);

    //Liberamos el inodo
    memset(inode, 0, sizeof(struct inode_fs));
}

// Añadir carácter
void add_char_to_inode(struct inode_fs *file, char contenido, filesystem_t *private_data)
{
    int i = 0, end = 0;
    char *buffer = malloc(sizeof(char)*4096);
    while(i < N_DIRECTOS && file->i_directos[i] != NULL && !end){
        strcpy(buffer, private_data -> block[file -> i_directos[i]]); // Nos traemos nuestro bloque en la pos file->i_directos[i]
        if(strlen(buffer) < 4096){
            buffer[strlen(buffer)] = contenido;
            strcpy(private_data -> block[file->i_directos[i]], buffer); // Guardamos el bloque modificado
            end = 1;
        }
        memset(buffer, 0, 4096);
        i++;
    }

    if(!end && i < N_DIRECTOS) {
        // Estamos ante un bloque vacío
        buffer[0] = contenido;
        file->i_directos[i] = create_block(private_data);
        strcpy(private_data -> block[file->i_directos[i]], buffer);
    } 
    // Liberamos el espacio reservado para el buffer
    free(buffer);
    buffer = NULL;
}

void clean_inode(struct inode_fs *file, filesystem_t *private_data) {
    // Limpiamos primero los directos
    for(int i = 0; i < N_DIRECTOS && file -> i_directos[i] != NULL; i++){
        memset(private_data -> block[file -> i_directos[i]], 0 , BLOCK_SIZE);
        file -> i_directos[i] = (long int) NULL;
    }
    file -> i_tam = 0;
}

// Función que trae los bloques de los punteros indirectos
//block_list get_blocks_indirect(long i_indirecto)
//{
    // //TODO
    // block_list *head = malloc(sizeof(struct block_list));
    // if(blocks[i_indirecto] == NULL){
    //     *head = NULL;
    // }else{
    //     int i = 0;
    //     int offset = 0;
    //     long block_index;

    //     // Accedemos al primer puntero del bloque para ver si existe
    //     memcpy(&block_index, blocks[i_indirecto] + offset, sizeof(long));

    //     // Si no existe, devolvemos NULL
    //     if(block_index == NULL){
    //         *head = NULL;
    //     } else {
    //         // Si existe, creamos el nodo head
    //         block_list node = malloc(sizeof(struct block_list));
    //         node -> block_index = block_index;
    //         node -> next = NULL;
    //         *head = node;

    //         // Hasta aqui tenemos una lista con un nodo solo
    //         // Vamos a crear el resto de nodos
    //         i++;
    //         offset = sizeof(long)*i;
    //         block_list aux = *head;

    //         while(offset < 1024 && block_index != NULL && block_index < NUM_BLOCKS){
    //             memcpy(&block_index, blocks[i_indirecto] + offset, sizeof(long));
    //             // Creamos el nodo
    //             block_list new_node = malloc(sizeof(struct block_list));
    //             new_node -> block_index = block_index;
    //             new_node -> next = NULL;

    //             // Lo añadimos a la lista
    //             aux -> next = new_node;
    //             aux = aux -> next;
    //             // Actualizamos el offset
    //             i++;
    //             offset = sizeof(long)*i;
    //         }
    //     }
    // }
    // return *head;
//}

// Función que añade un bloque al puntero indirecto
//void add_block_indirect(long indirect_pointer, long direct_pointer){
    // Localizar el primer hueco en el puntero indirecto
    // int i = 0; // Posicion dentro del bloque
    // int offset = 0; // Tamaño de un puntero
    // long block_index;
    // // Nos traemos el bloque
    // memcpy(&block_index, blocks[indirect_pointer] + offset, sizeof(long));
    // while(offset < 1024 && block_index != NULL && block_index < NUM_BLOCKS){
    //     i++;
    //     offset = sizeof(long)*i;
    //     memcpy(&block_index, blocks[indirect_pointer] + offset, sizeof(long));
    // }
    // // Añadimos el bloque al puntero indirecto
    // memcpy(blocks[indirect_pointer] + offset, &direct_pointer, sizeof(long));
//    ;
//}

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




