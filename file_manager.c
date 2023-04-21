#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new file with the given name in the given directory
void touch(char *name, char type, char *directory,  struct inode_fs *root, struct inode_bitmap_fs *inode_bitmap){
    // Search for the directory
    struct inode_fs *dir = inode_search(directory, *root);
    if(dir == NULL){
        printf("Directory not found\n");
        return;
    }

    // Search for the file
    struct inode_fs *file = inode_search(name, *dir);
    if(file != NULL){
        printf("File already exists\n");
        return;
    }
    
    // Create the file
    struct inode_fs *new_file = create_inode(type, name, inode_bitmap);
    
    // Add the file to the directory
    insert(name, dir, *new_file);
    if (type == 'd'){
        // Create the . and .. entries
        insert(".",  new_file, *new_file);
        insert("..", new_file, *dir);
    }  
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

