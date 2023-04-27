#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new file with the given name in the given directory
void touch(char *name, char type, char *path_directory,  struct inode_fs *root, struct inode_bitmap_fs *inode_bitmap){
    // Search for the directory
    struct inode_fs *dir = search_directory(path_directory, root);
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
    struct inode_fs *new_file = create_inode(type, name, inode_bitmap);
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
    int i = 0;
    struct directory_entry *entry = malloc(sizeof(struct directory_entry));
    while(i < N_DIRECTOS && directory.i_directos[i] != NULL){
        // Recorremos el bloque
        memcpy(entry, directory.i_directos[i], sizeof(struct directory_entry));
        for(int j = 1; j < 32 && (*entry).inode != NULL; j++){ // j es offset
            // Print the entry
            printf("%s ", entry->name);
            if((*(*entry).inode).i_type == 'd' && strcmp((*entry).name, ".") != 0 && strcmp((*entry).name, "..") != 0){
                printf("\n  -> "); 
                print_directory(*(*entry).inode);
            }
            memcpy(entry, directory.i_directos[i]+sizeof(struct directory_entry)*j, sizeof(struct directory_entry));
        }
        printf("\n");
        i++;    
    }
}

void rm(char *name, char *path_directory, struct inode_fs *root, struct inode_bitmap_fs *inode_bitmap){
    // Search for the directory
    struct inode_fs *dir = search_directory(path_directory, root);
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
    remove_inode(file, inode_bitmap);
}

// Borrar directorio vacío
void rmdir(char* path, struct inode_fs *root, struct inode_bitmap_fs *inode_bitmap)
{
    struct inode_fs *current_dir = search_directory(path, root);
    
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
        remove_entry(path, root);
        // Liberamos el inodo
        remove_inode(current_dir,inode_bitmap);
    }
}

// Sobreescribir contenido a un archivo
int overwrite(char* path, char *contenido, struct inode_fs root)
{
    struct inode_fs *file = search(path,root);

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
    if(strlen(contenido) > (sizeof(char) * 1024 * 10))
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

    return 0;
}