#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.c"
#include "inode.c"
#include "file_manager.c"
#include "tree_manager.c"

int main(){
    //Creamos el bitmap de inodos
    struct inode_bitmap_fs *inode_bitmap = malloc(sizeof(struct inode_bitmap_fs));

    //Creamos el inodo root
    struct inode_fs *root = create_root(inode_bitmap);

    //Creamos el directorio /home
    touch("home", 'd', ".", root, inode_bitmap);

    //Creamos el directorio /home/usuario
    touch("usuario", 'd', "/home", root, inode_bitmap);

    //Creamos el directorio /home/usuario/Desktop
    touch("Desktop", 'd', "/home/usuario", root, inode_bitmap);

    //Creamos el directorio /dev
    touch("dev", 'd', ".", root, inode_bitmap);

    //Creamos el archivo diego_cipote.txt
    touch("test.txt", '-', "/home/usuario/Desktop", root, inode_bitmap);
    //print_directory(*root);

    // Añadimos contenido al archivo test.txt
    append("/home/usuario/Desktop/test.txt", "Esto es una prueba", *root);
    append("/home/usuario/Desktop/test.txt", "\nEsto es otra prueba", *root);
    // char test[10241];
    // for (int i = 0; i < 10241; i++){
    //     test[i] = 'a';
    // }
    //overwrite("/home/usuario/Desktop/test.txt", test,*root);
    
    char *content = read_file("/home/usuario/Desktop/test.txt", *root);
    printf("%s\n", content);
    
}