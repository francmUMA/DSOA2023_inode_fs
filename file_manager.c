#include "data_structures_fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

char *get_path_directory(char *path)
{
    // Get the name of the directory and the name of the file
    char *path_directory = malloc(strlen(path));
    char *name = malloc(24);
    memset(path_directory, 0, strlen(path_directory));
    memset(name, 0, 24);
    char path_aux[strlen(path)];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    while (token != NULL)
    {
        strcat(path_directory, "/");
        strcpy(name, token);
        token = strtok(NULL, "/");
        if (token != NULL)
        {
            strcat(path_directory, name);
        }
    }
    free(name);
    return path_directory;
}

// Create a new file with the given name in the given directory
void touch(char *path, char type, filesystem_t *private_data)
{
    // Get the name of the directory and the name of the file
    char *path_directory = malloc(strlen(path));
    char *name = malloc(24);
    memset(path_directory, 0, strlen(path_directory));
    memset(name, 0, 24);
    char path_aux[strlen(path)];
    strcpy(path_aux, path);
    char *token = strtok(path_aux, "/");
    while (token != NULL)
    {
        strcpy(name, token);
        token = strtok(NULL, "/");
        strcat(path_directory, "/");
        if (token != NULL)
        {
            strcat(path_directory, name);
        }
    }

    if (strcmp(path_directory, "") == 0)
    {
        strcpy(path_directory, ".");
    }

    // Search for the directory
    long dir_number = search_directory(path_directory, private_data);
    if (private_data->inode[dir_number].i_type == 0)
    {
        printf("Directory not found\n");
        return;
    }

    // Search for the file
    long file_number = search_in_directory(name, dir_number, private_data);
    if (file_number != -1 && private_data->inode[file_number].i_type != 0)
    {
        printf("File already exists\n");
        return;
    }

    // Create the file
    long new_file_number = create_inode(type, name, private_data);
    if (type == 'd')
    {
        // Create the . and .. entries
        insert(".", new_file_number, new_file_number, private_data);
        insert("..", new_file_number, dir_number, private_data);
    }

    // Add the file to the directory
    insert(name, dir_number, new_file_number, private_data);
    free(path_directory);
    free(name);
}

// Print the directory tree
// Looks de entries in every directory and prints them
// If the entry is a directory, it print the entries in that directory recursively

void print_directory(long directory, filesystem_t *private_data)
{ // solo se usan los punteros directos (version 1)
    // Bring the entries of the directory
    int i = 0;
    struct directory_entry *entry;
    while (i < N_DIRECTOS && private_data->inode[directory].i_directos[i] != NULL)
    {
        // Recorremos el bloque
        entry = (struct directory_entry *)private_data->block[private_data->inode[directory].i_directos[i]];
        int j;
        for (j = 0; j < 128 && (strcmp(entry[j].name, "") != 0 || entry[j].inode != 0); j++)
        { // j es offset
            if(entry[j].inode != -1){
                // Print the entry
                printf("%s ", entry[j].name);
                if (private_data->inode[entry[j].inode].i_type == 'd' && strcmp(entry[j].name, ".") != 0 && strcmp(entry[j].name, "..") != 0)
                {
                    printf("\n -> ");
                    print_directory(entry[j].inode, private_data);
                }
            }
        }
        printf("\n");
        i++;
    }
}

void unlink_fs(char *path, filesystem_t *private_data)
{
    // Search for the directory
    long dir_number = search(get_path_directory(path), private_data);
    if (private_data->inode[dir_number].i_type == 0)
    {
        printf("Directory not found\n");
        return;
    }

    // Search for the file
    long file_number = search(path, private_data);
    if (private_data->inode[file_number].i_type == 0)
    {
        printf("File not found\n");
        return;
    }
    else if (private_data->inode[file_number].i_type == 'd')
    {
        printf("Cannot remove a directory\n");
        return;
    }

    private_data->inode[file_number].i_links -= 1;

    struct directory_entry *entry = (struct directory_entry *)private_data->block[private_data->inode[file_number].entry_block];
    // Remove the file from the directory
    remove_entry(entry[private_data->inode[file_number].offset].name, dir_number, private_data);

    // Remove the file
    if (private_data->inode[file_number].i_links == 0)
    {
        remove_inode(file_number, private_data);
    }
}

// Borrar directorio vacío
void rmdir_fs(char *path, filesystem_t *private_data)
{
    long current_dir_number = search_directory(path, private_data);

    if (private_data->inode[current_dir_number].i_type == 0)
    {
        printf("No se ha encontrado el directorio\n");
        return;
    }

    int i = 0, n_dentries = 0, end = 0;
    struct directory_entry *entry;
    // Comprobamos que el directorio está vacío
    entry = (struct directory_entry *)private_data->block[private_data->inode[current_dir_number].i_directos[i]];
    while (i < N_DIRECTOS && !end && private_data->inode[entry[i].inode].i_type != 0)
    {
        // Recorremos el bloque
        for (int j = 0; j < 128 && (strcmp(entry[j].name, "") != 0 || entry[j].inode != 0) && !end; j++)
        { // j es offset
            if(entry[j].inode != -1){
                n_dentries++;
            }
            if(strcmp(entry[j+1].name, "") == 0 && entry[j+1].inode == 0){
                end = 1;
            }
        }

        i++;
    }

    if (n_dentries > 2)
    {
        printf("El directorio no está vacío\n");
        return;
    }
    else
    {
        // Eliminamos las entradas . y ..
        remove_entry(((struct directory_entry *)private_data->block[private_data->inode[current_dir_number].entry_block])[private_data->inode[current_dir_number].offset].name, entry[1].inode, private_data);
        remove_entry("..", current_dir_number, private_data);
        remove_entry(".", current_dir_number, private_data);
        // Eliminamos el directorio
        remove_inode(current_dir_number, private_data);
    }
}

// Añade contenido a un archivo
int append(char *path, char *contenido, filesystem_t *private_data)
{
    long file_number = search(path, private_data);

    if (private_data->inode[file_number].i_type == 0)
    {
        printf("No se ha encontrado el archivo\n");
        return -1;
    }

    // Comprobamos que sea archivo con i_type = '-'
    if (private_data->inode[file_number].i_type != '-')
    {
        printf("No se puede sobreescribir un directorio\n");
        return -1;
    }

    // Comprobamos que el contenido no sea mayor que
    if ((strlen(contenido) + private_data->inode[file_number].i_tam) > (sizeof(char) * 1024 * 27))
    {
        printf("El contenido es mayor que el tamaño del archivo\n");
        return -1;
    }

    // Añadir caracter al inodo
    int i;
    for (i = 0; i < strlen(contenido); i++)
    {
        add_char_to_inode(file_number, contenido[i], private_data);
    }

    private_data->inode[file_number].i_tam += strlen(contenido);

    return 0;
}

// Añade contenido a un archivo
int overwrite(char *path, char *contenido, filesystem_t *private_data)
{
    // Limpia los bloques del inodo
    long file_number = search(path, private_data);
    if (private_data->inode[file_number].i_type == 0)
    {
        printf("File not found\n");
        return -1;
    }

    clean_inode(file_number, private_data);

    return append(path, contenido, private_data);
}

// Lee el contenido de un archivo
char *read_file(char *path, filesystem_t *private_data)
{
    long file_number = search(path, private_data);
    if (private_data->inode[file_number].i_type == 0)
    {
        printf("File not found\n");
        return NULL;
    }
    else
    {
        // Comprobamos que sea archivo con i_type = '-'
        if (private_data->inode[file_number].i_type != '-')
        {
            printf("No se puede leer un directorio\n");
            return NULL;
        }
        else
        {
            // Mostramos el contenido del inodo
            char *buffer = malloc(sizeof(char) * 4096);
            char *res = malloc(private_data->inode[file_number].i_tam);

            int counter = 0;
            for (int i = 0; i < N_DIRECTOS && counter < private_data->inode[file_number].i_tam; i++)
            {
                strcpy(buffer, private_data->block[private_data->inode[file_number].i_directos[i]]);
                for (int j = 0; j < 4096 && counter < private_data->inode[file_number].i_tam; j++)
                {
                    res[counter] = buffer[j];
                    counter++;
                }
                memset(buffer, 0, sizeof(char) * 4096);
            }

            free(buffer);
            return res;
        }
    }
}

void rename_file(char *path, char *new_name, filesystem_t *private_data)
{
    // TODO
    long file_number = search(path, private_data);
    if (private_data->inode[file_number].i_type == 0)
    {
        printf("File not found\n");
        return;
    }

    // Buscamos el directorio padre
    char *parent_path = get_path_directory(path);
    long parent_number = search_directory(parent_path, private_data);

    // Buscamos en el directorio
    long new_file_number = search_in_directory(new_name, parent_number, private_data);

    if (new_file_number > -1)
    {
        printf("Ya existe un archivo con ese nombre\n");
        return;
    }

    // Cambiamos el nombre
    strcpy(((struct directory_entry *)private_data->block[private_data->inode[file_number].entry_block])[private_data->inode[file_number].offset].name, new_name);
}