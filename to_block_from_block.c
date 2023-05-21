#include "data_structures_fs.h"
#include "init.c"
// #include "inode.c"
#include "bitmap.c"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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
    num_inodes = (num_blocks / 4) / sizeof(struct inode_fs);
    reserved_blocks = 1 + (num_inodes * sizeof(struct inode_fs)) + 
                        ((num_inodes / 8) / BLOCK_SIZE + 1) + (((num_blocks - ((num_inodes / 8) / BLOCK_SIZE + 1) - 
                        (num_inodes * sizeof(struct inode_fs)) - 1) / 8) / BLOCK_SIZE + 1); 

    // Mapeamos el superbloque
    private_data -> superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, 0);

    // Si no se mapea bien, hay un error
    if(private_data -> superblock == MAP_FAILED)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }
    init_superblock(private_data -> superblock);

    // Mapeamos el bitmap de bloques
    private_data -> block_bitmap = mmap(NULL, sizeof(struct block_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, private_data -> superblock -> block_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> block_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de bloques\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el bitmap de inodos
    private_data -> inode_bitmap = mmap(NULL, sizeof(struct inode_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, private_data -> superblock -> inode_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> inode_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de inodos\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer inodo
    private_data -> inode = mmap(NULL, sizeof(struct inode_fs), PROT_READ | PROT_WRITE, MAP_SHARED, private_data -> fd, private_data -> superblock -> first_inode_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if(private_data -> inode == MAP_FAILED)
    {
        printf("Error al mapear el primer inodo\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer bloque de datos
    private_data -> data_block = mmap(NULL, 
                                      sizeof(char) * BLOCK_SIZE * (num_blocks - reserved_blocks), 
                                      PROT_READ | PROT_WRITE, MAP_SHARED, 
                                      private_data -> fd, 
                                      private_data -> superblock -> first_data_block * BLOCK_SIZE);
    
    // Si no se mapea bien, hay un error
    if(private_data -> data_block == MAP_FAILED)
    {
        printf("Error al mapear el primer bloque de datos\n");
        return EXIT_FAILURE;
    }

    // Cerramos el fichero
    close(private_data -> fd);



    // Sincronizamos los cambios
    // Superbloque
    // if((msync(superblock,sizeof(struct superblock_fs),MS_SYNC)) < 0)
    // {
    //     printf("Error al sincronizar el superbloque\n");
    //     return EXIT_FAILURE;
    // }
    // Bitmap de bloques
    // if((msync(block_bitmap,sizeof(struct block_bitmap_fs),MS_SYNC)) < 0)
    // {
    //     printf("Error al sincronizar el bitmap de bloques\n");
    //     return EXIT_FAILURE;
    // }
    // Bitmap de inodos
    // if((msync(inode_bitmap,sizeof(struct inode_bitmap_fs),MS_SYNC)) < 0)
    // {
    //     printf("Error al sincronizar el bitmap de inodos\n");
    //     return EXIT_FAILURE;
    // }

    return 0;
}