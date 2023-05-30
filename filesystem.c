#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.c"
#include "inode.c"
#include "init.c"
#include "file_manager.c"
#include "tree_manager.c"

int main()
{
    // Inicializamos el sistema de ficheros
    private_data = malloc(sizeof(filesystem_t));

    // Abrir el fichero con el contenido del filesystem
    private_data -> fd = open("filesystem.img", O_RDWR, 0666);

    if (private_data -> fd == -1)
    {
        printf("Error al abrir el fichero\n");
        return -1;
    }

    // Cogemos los datos del fichero binary
    struct stat st;

    // Si fstat es -1, hay un error
    if (fstat(private_data -> fd, &st))
    {
        printf("Error al obtener los datos del fichero\n");
        return -1;
    }
    
    // Inicializamos la informacion privada del sistema de ficheros
    num_blocks = st.st_size / BLOCK_SIZE;
    num_inodes = (num_blocks / 4) * BLOCK_SIZE / sizeof(struct inode_fs);
    blocks_for_inodes = num_blocks / 4;
    blocks_for_inode_bitmap = (num_inodes / 8) / BLOCK_SIZE + 1;
    blocks_for_block_bitmap = (num_blocks / 8) / BLOCK_SIZE + 1;
    reserved_blocks = blocks_for_inodes + blocks_for_inode_bitmap + blocks_for_block_bitmap + 1;

    private_data -> superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, 0);

    // Si no se mapea bien, hay un error
    if(private_data -> superblock == MAP_FAILED)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }
    
    init_superblock(private_data -> superblock);

    // Mapeamos el bitmap de bloques
    private_data -> block_bitmap = mmap(NULL, blocks_for_block_bitmap * BLOCK_SIZE, 
                                        PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, 
                                        private_data -> superblock -> block_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> block_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de bloques\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el bitmap de inodos
    private_data -> inode_bitmap = mmap(NULL, blocks_for_inode_bitmap * BLOCK_SIZE, 
                                        PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, 
                                        private_data -> superblock -> inode_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> inode_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de inodos\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer inodo
    private_data -> inode = mmap(NULL, blocks_for_inodes * BLOCK_SIZE,
                                PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, 
                                private_data -> superblock -> first_inode_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> inode == MAP_FAILED)
    {
        printf("Error al mapear el primer inodo\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer bloque de datos
    private_data -> block = mmap(NULL, 
                                st.st_size, 
                                PROT_READ | PROT_WRITE, MAP_SHARED, 
                                private_data -> fd, 
                                0);
    
    // Si no se mapea bien, hay un error
    if(private_data -> block == MAP_FAILED)
    {
        printf("Error al mapear el primer bloque de datos\n");
        return EXIT_FAILURE;
    }

    // Cerramos el fichero
    close(private_data -> fd);
    init_block_bitmap();
}