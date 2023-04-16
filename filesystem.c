#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.c"
#include "create_inode.c"
#include "file_manager.c"
#include "tree_manager.c"

int main(){
    //Creamos el bitmap de inodos
    struct inode_bitmap_fs *inode_bitmap = malloc(sizeof(struct inode_bitmap_fs));

    //Creamos el inodo root
    struct inode_fs *root = create_root(inode_bitmap);
    print_directory(*root);

    //Creamos el directorio /home
    touch("home", 'd', ".", root, inode_bitmap);
    print_directory(*root);

    //Creamos el directorio /home/usuario
    touch("usuario", 'd', "home", root, inode_bitmap);

    //Creamos el directorio /home/usuario/Desktop
    touch("Desktop", 'd', "usuario", root, inode_bitmap);

    //Creamos el directorio /dev
    touch("dev", 'd', ".", root, inode_bitmap);

    //Printeamos el arbol
    print_directory(*root);
}