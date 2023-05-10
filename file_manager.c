#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new file with the given name in the given directory
void touch(char *name, char type, char *path_directory){
    // Search for the directory
    struct inode_fs *dir = search_directory(path_directory);
    if(dir == NULL){
        printf("Directory not found\n");
        return;
    }

    // Search for the file
    struct inode_fs *file = search_in_directory(name, *dir);
    if(file != NULL){
        printf("File already exists\n");
        return;
    }
    
    // Create the file
    struct inode_fs *new_file = create_inode(type, name);
    if (type == 'd'){
        // Create the . and .. entries
        insert(".",  new_file, new_file);
        insert("..", new_file, dir);
    }  

    // Add the file to the directory
    insert(name, dir, new_file); 
}

// Print the directory tree
// Looks de entries in every directory and prints them
// If the entry is a directory, it print the entries in that directory recursively

void print_directory(struct inode_fs directory){           //solo se usan los punteros directos (version 1)
    //Bring the entries of the directory
    int i = 0, offset = 0;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && directory.i_directos[i] != NULL){
        // Recorremos el bloque
        memcpy(entry, blocks[directory.i_directos[i]]+offset, sizeof(struct directory_entry));
        for(int j = 1; j < 32 && entry != NULL; j++){ // j es offset
            // Print the entry
            offset = sizeof(struct directory_entry)*j;
            printf("%s ", entry->name);
            if((*(*entry).inode).i_type == 'd' && strcmp((*entry).name, ".") != 0 && strcmp((*entry).name, "..") != 0){
                printf("\n -> "); 
                print_directory(*(*entry).inode);
            }
            memcpy(entry, blocks[directory.i_directos[i]]+offset, sizeof(struct directory_entry));
        }
        printf("\n");
        i++;    
    }
}


void rm(char *name, char *path_directory){
    // Search for the directory
    struct inode_fs *dir = search_directory(path_directory);
    if(dir == NULL){
        printf("Directory not found\n");
        return;
    }

    // Search for the file
    struct inode_fs *file = search_in_directory(name, *dir);
    if(file == NULL){
        printf("File not found\n");
        return;
    } else if ((*file).i_type == 'd'){
        printf("Cannot remove a directory\n");
        return;
    }

    // Remove the file from the directory
    remove_entry(name, dir);

    // Remove the file
    remove_inode(file);
}

// Borrar directorio vacío
void rmdir(char* path)
{
    struct inode_fs *current_dir = search_directory(path);
    
    if(current_dir == NULL) {
        printf("No se ha encontrado el directorio\n");
        return;
    }

    int i = 0;
    // Comprobamos que el directorio está vacío
    while(i < N_DIRECTOS && (*current_dir).i_directos[i] == NULL){
        i++;
    }

    if(i == N_DIRECTOS){
        // Esta vacío
        // Eliminamos la entrada de directorio
        struct inode_fs *parent = search_in_directory("..", *current_dir);
        remove_entry(path, parent); 
        // Liberamos el inodo
        remove_inode(current_dir);
    }
}

// Añade contenido a un archivo
int append(char* path, char *contenido)
{
    struct inode_fs *file = search(path);

    if(file == NULL)
    {
        printf("No se ha encontrado el archivo\n");
        return -1;
    }

    // Comprobamos que sea archivo con i_type = '-'
    if((*file).i_type != '-')
    {
        printf("No se puede sobreescribir un directorio\n");
        return -1;
    }

    // Comprobamos que el contenido no sea mayor que 
    // if(strlen(contenido) > (sizeof(char) * 1024 * 10))
    // {
    //     printf("El contenido es mayor que el tamaño del archivo\n");
    //     return -1;
    // }

    // Añadir caracter al inodo
    int i;
    for(i = 0; i < strlen(contenido); i++)
    {
        add_char_to_inode(file, contenido[i]);
    }
    file -> i_tam += strlen(contenido);

    return 0;
}

// Añade contenido a un archivo
int overwrite(char* path, char *contenido)
{
    // Limpia los bloques del inodo
    struct inode_fs *file = search(path);
    if (file == NULL){
        printf("File not found\n");
        return -1;
    }

    clean_inode(file);

    return append(path, contenido);
}

// Lee el contenido de un archivo
char *read_file(char *path){
    struct inode_fs *file = search(path);
    if (file == NULL){
        printf("File not found\n");
        return;
    }

    // Comprobamos que sea archivo con i_type = '-'
    if((*file).i_type != '-')
    {
        printf("No se puede leer un directorio\n");
        return;
    }

    // Mostramos el contenido del inodo
    char *buffer = malloc(sizeof(char) * 1024);
    char *res = malloc(file -> i_tam);

    int counter = 0;
    for(int i = 0; i < N_DIRECTOS && counter < file -> i_tam; i++){
        memcpy(buffer, blocks[file -> i_directos[i]], sizeof(char) * 1024);
        for(int j = 0; j < 1024 && counter < file -> i_tam; j++){
            res[counter] = buffer[j];
            counter++;
        }
        memset(buffer, 0, sizeof(char) * 1024);
    }
    block_list direct_blocks_indirect = get_blocks_indirect(file -> i_simple_ind[0]);
    while(direct_blocks_indirect != NULL){
        memcpy(buffer, blocks[direct_blocks_indirect -> block_index], sizeof(char) * 1024);
        for(int j = 0; j < 1024 && counter < file -> i_tam; j++){
            res[counter] = buffer[j];
            counter++;
        }
        memset(buffer, 0, sizeof(char) * 1024);
        direct_blocks_indirect = direct_blocks_indirect -> next;
    }
    free(buffer);

    return res;
}