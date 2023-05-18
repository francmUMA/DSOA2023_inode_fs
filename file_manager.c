#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *get_path_directory(char* path)
{
    // Get the name of the directory and the name of the file
    char *path_directory = malloc(strlen(path));
    char *name = malloc(24);
    memset(path_directory, 0, strlen(path_directory));
    memset(name, 0, 24);
    char path_aux[strlen(path)];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    while (token != NULL){
        strcpy(name, token);
        token = strtok(NULL, "/");
        if (token != NULL){
            strcat(path_directory, "/");
            strcat(path_directory, name);
        }
    }
    free(name);
    return path_directory;
}

// Create a new file with the given name in the given directory
void touch(char *path, char type){
    // Get the name of the directory and the name of the file
    char *path_directory = malloc(strlen(path));
    char *name = malloc(24);
    memset(path_directory, 0, strlen(path_directory));
    memset(name, 0, 24);
    char path_aux[strlen(path)];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    while (token != NULL){
        strcpy(name, token);
        token = strtok(NULL, "/");
        if (token != NULL){
            strcat(path_directory, "/");
            strcat(path_directory, name);
        }
    }

    if (strcmp(path_directory, "") == 0){
        strcpy(path_directory, ".");
    }

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
    free(path_directory);
    free(name); 
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
        for(int j = 1; j < 32 && entry->inode != NULL; j++){ // j es offset
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


void unlink_fs(char *name, char *path_directory){
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
    file -> i_links -= 1;

    // Remove the file from the directory
    remove_entry(name, dir);

    // Remove the file
    if (file -> i_links == 0){
        remove_inode(file);
    }
}

// Borrar directorio vacío
void rmdir_fs(char* path)
{
    struct inode_fs *current_dir = search_directory(path);
    
    if(current_dir == NULL) {
        printf("No se ha encontrado el directorio\n");
        return;
    }

    int i = 0;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    memcpy(entry, blocks[(*current_dir).i_directos[0]], sizeof(struct directory_entry));
    // Comprobamos que el directorio está vacío
    while(entry->inode != NULL)
    {
        i++;
        memcpy(entry, blocks[(*current_dir).i_directos[0]] + sizeof(struct directory_entry) * i, sizeof(struct directory_entry));
    }

    if(i > 2)
    {
        printf("El directorio no está vacío\n");
        return;
    }else{
        // Eliminamos las entradas . y ..
        memcpy(entry, blocks[(*current_dir).i_directos[0]] + 32, sizeof(struct directory_entry));
        remove_entry(current_dir -> i_name, (*entry).inode);
        remove_entry("..", current_dir);
        remove_entry(".", current_dir);
        // Eliminamos el directorio
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
    if((strlen(contenido) + file->i_tam) > (sizeof(char) * 1024 * 27))
    {
        printf("El contenido es mayor que el tamaño del archivo\n");
        return -1;
    }

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
    if(file->i_simple_ind[0] == NULL){
        free(buffer);
        return res;
    }

    block_list direct_blocks_indirect = get_blocks_indirect(file -> i_simple_ind[0]);
    
    while(direct_blocks_indirect != NULL && direct_blocks_indirect -> block_index < NUM_BLOCKS){
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

void rename_file(char *path, char *new_name){
    //TODO
    struct inode_fs *file = search(path);
    if (file == NULL){
        printf("File not found\n");
        return;
    }

    // Buscamos el directorio padre
    char *parent_path = get_path_directory(path);
    struct inode_fs *parent = search_directory(parent_path);

    // Buscamos en el directorio
    struct inode_fs *new_file = search_in_directory(new_name, *parent);

    if(new_file != NULL)
    {
        printf("Ya existe un archivo con ese nombre\n");
        return;
    }

    // Cambiamos el nombre
    remove_entry(file->i_name, parent);
    strcpy(file->i_name, new_name);
    insert(new_name, parent, file);
}