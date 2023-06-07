#include "data_structures_fs.h"

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(char *name, struct inode_fs *directory_dst, struct inode_fs* n_node, filesystem_t *private_data) // Versión 1 (Como si solo tuvieramos directos)
{
    int i = 0, end = 0;
    struct directory_entry *entry;
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){
        // Vamos a buscar el primer hueco libre en el bloque
        entry = (struct directory_entry*) private_data -> block[directory_dst->i_directos[i]];
        // Recorremos el bloque
        int j;
        while(j < 128 && entry[j].inode != NULL) j++;
        // Nos aseguramos de que nuestra entrada es null
        if (entry[j].inode == NULL)
        {
            // Guardamos nuestra entrada de bloque
            strcpy(entry[j].name, name);
            n_node -> i_links += 1;
            entry[j].inode = n_node;
            if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) entry[j].inode->entry = &entry[j];
            else if (strcmp(name, ".") == 0 && n_node->i_num == 0) entry[j].inode->entry = &entry[j]; 
            end = 1;
        }
        i++;
    }
    
    // Si no hemos encontrado un hueco libre, creamos un nuevo bloque si es posible
    if (!end && i < N_DIRECTOS){
        // Creamos un nuevo bloque
        directory_dst -> i_directos[i] = create_block(private_data);

        entry = (struct directory_entry*) private_data -> block[directory_dst->i_directos[i]];
        // Modificamos la entrada de directorio
        strcpy(entry[0].name, name);

        // Si n_node es el mismo inodo que directory_dst, se guarda en la entry el directory_dst
        if ((*n_node).i_num == (*directory_dst).i_num){
            entry[0].inode = directory_dst;
        } else {
            entry[0].inode = n_node;
        }
        entry[0].inode->entry = &entry[0];
    }
    else if (!end) {
        printf("No hay espacio en el directorio\n");
    }
}

//Eliminar una entrada de directorio
void remove_entry(char *name, struct inode_fs *directory_dst, filesystem_t *private_data){
    //Buscar la entrada de directorio
    int i = 0, end = 0;
    struct directory_entry *entry;
    while(i < N_DIRECTOS && directory_dst->i_directos[i] != NULL && !end){
        entry = (struct directory_entry*) private_data -> block[directory_dst->i_directos[i]];
        // Recorremos el bloque
        int j;
        for(j = 0; j < 128 && entry[j].inode != NULL && !end; j++){

            // Si es la entrada que buscamos, la eliminamos
            if (strcmp(entry[j].name, name) == 0){

                //Se elimina la entrada de directorio
                memset(&entry[j], 0, sizeof(struct directory_entry));
                end = 1;
            }  
        }
        i++;
    }

    if (!end){
        printf("No se ha encontrado la entrada de directorio\n");
    }
}

// Función para buscar un inodo en un directorio concreto
struct inode_fs *search_in_directory(char *target, struct inode_fs directory, filesystem_t *private_data){
    //Creamos el inodo que vamos a devolver
    struct inode_fs *res = NULL;

    //Recorremos el directorio
    int end = 0;
    struct directory_entry *entry;

    for (int i = 0; i < N_DIRECTOS && directory.i_directos[i] != NULL & !end; i++){
        // Recorremos el bloque
        entry = (struct directory_entry*) private_data -> block[directory.i_directos[i]];
        int j;
        for (j = 0; j < 128 && entry[j].inode != NULL && !end; j++){
            if (strcmp(entry[j].name, target) == 0){
                // Si es el inodo que buscamos, lo devolvemos
                res = entry[j].inode;
                end = 1;
            }
        }
    }
    return res;
}

struct inode_fs *search(char *path, filesystem_t *private_data){
    // Parsing path (nuestro path acaba en el directorio del archivo que queremos buscar)
    struct inode_fs *current_inode;
    char path_aux[strlen(path)], cmp_path[500] = "";
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    
    if(token == NULL) token = ".";

    current_inode = search_in_directory(token, private_data -> inode[0], private_data);
    while(current_inode != NULL && token != NULL && current_inode->i_type == 'd'){
        strcat(cmp_path, "/");
        // Buscamos el inodo en el directorio actual
        if(current_inode == NULL) {
            printf("No se ha encontrado el directorio\n");
            return NULL;
        }
        
        // Concatenamos el path
        if (strcmp(current_inode->entry->name, "..") != 0) strcat(cmp_path, current_inode->entry->name);
        else strcpy(cmp_path, "");
        token = strtok(NULL, "/");
        if (token != NULL) current_inode = search_in_directory(token, *current_inode, private_data);
    }   
    
    // Si no hemos encontrado el inodo, devolvemos NULL
    if (current_inode == NULL) return NULL;

    if (current_inode ->i_type == '-') {
        strcat(cmp_path, "/");
        strcat(cmp_path, current_inode->entry->name);
    }

    if(strcmp(cmp_path,"/.") == 0 && strcmp(path,"/") == 0){
        return current_inode;
    }else if(strcmp(cmp_path,path) != 0){
        printf("No existe el fichero\n");
        return NULL;
    }

    return current_inode;
}

struct inode_fs *search_directory(char *path, filesystem_t *private_data){
    if (strcmp(path, ".") == 0 || strcmp(path, "/") == 0){
        return &(private_data -> inode[0]);
    }
    struct inode_fs *current_dir;
    char path_aux[24];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    current_dir = search_in_directory(token, private_data -> inode[0], private_data);

    while(token != NULL){
        if(current_dir == NULL || current_dir->i_type != 'd') {
            return NULL;
        } 
        token = strtok(NULL, "/");
        if (token != NULL){
            current_dir = search_in_directory(token, *current_dir, private_data);
        }
    }
    return current_dir;
}

