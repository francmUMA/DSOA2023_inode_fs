#include "data_structures_fs.h"

// Tengo que modificar nuestra estructura de árbol de búsqueda
void insert(char *name, long directory_dst_number, long n_node_number, filesystem_t *private_data) // Versión 1 (Como si solo tuvieramos directos)
{
    int i = 0, end = 0;
    struct directory_entry *entry;
    while(i < N_DIRECTOS && private_data->inode[directory_dst_number].i_directos[i] != 0 && !end){
        // Vamos a buscar el primer hueco libre en el bloque
        entry = (struct directory_entry*) private_data -> block[private_data->inode[directory_dst_number].i_directos[i]];
        // Recorremos el bloque
        int j = 0;
        while(j < 128 && strcmp(entry[j].name, "") != 0) j++;
        // Nos aseguramos de que nuestra entrada es null
        if (strcmp(entry[j].name, "") == 0)
        {
            // Guardamos nuestra entrada de bloque
            strcpy(entry[j].name, name);
            private_data->inode[n_node_number].i_links += 1;
            entry[j].inode = n_node_number;
            if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
            {
                private_data->inode[entry[j].inode].entry_block = private_data->inode[directory_dst_number].i_directos[i];
                private_data->inode[entry[j].inode].offset = j;
            }
            else if (strcmp(name, ".") == 0 && private_data->inode[n_node_number].i_num == 0){
                private_data->inode[entry[j].inode].entry_block = private_data->inode[directory_dst_number].i_directos[i];
                private_data->inode[entry[j].inode].offset = j;
            } 
            end = 1;
        }
        i++;
    }
    
    // Si no hemos encontrado un hueco libre, creamos un nuevo bloque si es posible
    if (!end && i < N_DIRECTOS){
        // Creamos un nuevo bloque
        private_data->inode[directory_dst_number].i_directos[i] = create_block(private_data);

        entry = (struct directory_entry*) private_data -> block[private_data->inode[directory_dst_number].i_directos[i]];
        // Modificamos la entrada de directorio
        strcpy(entry[0].name, name);

        // Si n_node es el mismo inodo que directory_dst, se guarda en la entry el directory_dst
        if (private_data->inode[n_node_number].i_num == private_data->inode[directory_dst_number].i_num){
            entry[0].inode = directory_dst_number;
        } else {
            entry[0].inode = n_node_number;
        }
        private_data->inode[entry[0].inode].entry_block = private_data->inode[directory_dst_number].i_directos[i];
        private_data->inode[entry[0].inode].offset = 0;
    }
    else if (!end) {
        printf("No hay espacio en el directorio\n");
    }
}

//Eliminar una entrada de directorio
void remove_entry(char *name, long directory_dst_number, filesystem_t *private_data){
    //Buscar la entrada de directorio
    int i = 0, end = 0;
    struct directory_entry *entry;
    while(i < N_DIRECTOS && private_data->inode[directory_dst_number].i_directos[i] != 0 && !end){
        entry = (struct directory_entry*) private_data -> block[private_data->inode[directory_dst_number].i_directos[i]];
        // Recorremos el bloque
        int j;
        for(j = 0; j < 128 && (strcmp(entry[j].name, "") != 0 || entry[j].inode != 0) && !end; j++){
            if(entry[j].inode == -1) continue;
            // Si es la entrada que buscamos, la eliminamos
            if (strcmp(entry[j].name, name) == 0){

                //Se elimina la entrada de directorio
                memset(&entry[j], 0, sizeof(struct directory_entry));
                entry[j].inode = -1;
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
long search_in_directory(char *target, long directory_number, filesystem_t *private_data){
    //Creamos el inodo que vamos a devolver
    long res = -1;

    //Recorremos el directorio
    int end = 0;
    struct directory_entry *entry;

    for (int i = 0; i < N_DIRECTOS && private_data->inode[directory_number].i_directos[i] != 0 & !end; i++){
        // Recorremos el bloque
        entry = (struct directory_entry*) private_data -> block[private_data->inode[directory_number].i_directos[i]];
        int j;
        for (j = 0; j < 128 && (strcmp(entry[j].name, "") != 0 || entry[j].inode != 0) && !end; j++){
            if (strcmp(entry[j].name, target) == 0){
                // Si es el inodo que buscamos, lo devolvemos
                res = entry[j].inode;
                end = 1;
            }
        }
    }
    return res;
}

long search(char *path, filesystem_t *private_data){
    // Parsing path (nuestro path acaba en el directorio del archivo que queremos buscar)
    if(strcmp(path,"") == 0)
    {
        printf("No se ha introducido un path\n");
        return -1;
    }
    
    long current_inode_number;
    char path_aux[strlen(path)], cmp_path[500] = "";
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    struct directory_entry *entry;
    if(token == NULL) token = ".";

    current_inode_number = search_in_directory(token, 0, private_data);
    if(current_inode_number == -1) {
        printf("No se ha encontrado el archivo\n");
        return -1;
    }
    while(private_data->inode[current_inode_number].i_type != 0 && token != NULL && private_data->inode[current_inode_number].i_type == 'd'){
        strcat(cmp_path, "/");
        // Buscamos el inodo en el directorio actual
        if(private_data->inode[current_inode_number].i_type == 0) {
            printf("No se ha encontrado el directorio\n");
            return -1;
        }
        
        // Concatenamos el path
        entry = (struct directory_entry*) private_data -> block[private_data->inode[current_inode_number].entry_block];
        if (strcmp(entry[private_data->inode[current_inode_number].offset].name, "..") != 0) strcat(cmp_path, entry[private_data->inode[current_inode_number].offset].name);
        else strcpy(cmp_path, "");
        token = strtok(NULL, "/");
        if (token != NULL) current_inode_number = search_in_directory(token, current_inode_number, private_data);
        if(current_inode_number == -1) {
            printf("No se ha encontrado el archivo\n");
            return -1;
        }
    }   
    
    // Si no hemos encontrado el inodo, devolvemos NULL
    if (private_data->inode[current_inode_number].i_type == 0) return -1;

    if (private_data->inode[current_inode_number].i_type == '-') {
        entry = (struct directory_entry*) private_data -> block[private_data->inode[current_inode_number].entry_block];
        strcat(cmp_path, "/");
        strcat(cmp_path, entry[private_data->inode[current_inode_number].offset].name);
    }

    if(strcmp(cmp_path,"/.") == 0 && strcmp(path,"/") == 0){
        return current_inode_number;
    }else if(strcmp(cmp_path,path) != 0){
        printf("No existe el fichero\n");
        return -1;
    }

    return current_inode_number;
}

long search_directory(char *path, filesystem_t *private_data){
    if (strcmp(path, ".") == 0 || strcmp(path, "/") == 0){
        return 0;
    }
    long current_dir_number;
    char path_aux[24];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    current_dir_number = search_in_directory(token, 0, private_data);

    while(token != NULL){
        if(private_data->inode[current_dir_number].i_type == 0 || private_data->inode[current_dir_number].i_type != 'd') {
            return -1;
        } 
        token = strtok(NULL, "/");
        if (token != NULL){
            current_dir_number = search_in_directory(token, current_dir_number, private_data);
        }
    }
    return current_dir_number;
}

