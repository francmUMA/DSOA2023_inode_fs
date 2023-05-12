#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.c"
#include "inode.c"
#include "file_manager.c"
#include "tree_manager.c"

int main()
{
    // Creamos el bitmap de inodos
    block_bitmap = malloc(sizeof(struct block_bitmap_fs));
    inode_bitmap = malloc(sizeof(struct inode_bitmap_fs));

    // Creamos el inodo root
    root = create_root();

    // Creamos el directorio /home
    touch("home", 'd', ".");

    // Creamos el directorio /home/usuario
    touch("usuario", 'd', "/home");

    // Creamos el directorio /home/usuario/Desktop
    touch("Desktop", 'd', "/home/usuario");

    // Creamos el directorio /dev
    touch("dev", 'd', ".");

    // Creamos el archivo diego_cipote.txt
    touch("test.txt", '-', "/home/usuario/Desktop");
    // print_directory(*root);

    // Añadimos contenido al archivo test.txt
    append("/home/usuario/Desktop/test.txt", "Esto es una prueba");
    append("/home/usuario/Desktop/test.txt", "\nEsto es otra prueba");

    char *content = read_file("/home/usuario/Desktop/test.txt");
    printf("%s\n", content);

    char test[27648];
    for (int i = 0; i < 27648; i++)
    {
        test[i] = 'a';
    }
    overwrite("/home/usuario/Desktop/test.txt", test);
    char *content2 = read_file("/home/usuario/Desktop/test.txt");
    printf("%d\n", strlen(content2));
    for(int i = 0; i < 1200; i++){
        append("/home/usuario/Desktop/test.txt", "a");
    }
    char *content1 = read_file("/home/usuario/Desktop/test.txt");
    printf("%d\n", strlen(content1));
}