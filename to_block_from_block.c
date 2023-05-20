#include "data_structures_fs.h"
#include "superblock.c"
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
    // Me creo un inodo nuevo y lo inserto en mi estructura de datos
    struct inode_fs *new_inode = malloc(sizeof(struct inode_fs));
    new_inode->i_num = 1;
    new_inode->i_type = 'f';
    new_inode->i_tam = 0;
    new_inode->i_permission = 0777;
    new_inode->i_links = 1;
    // Abrir un fichero binario donde vamos a mapear toda la informacion
    fd = open("filesystem.img", O_RDWR, 0666);

    if (fd == -1)
    {
        printf("Error al abrir el fichero\n");
        return -1;
    }

    // Cogemos los datos del fichero binary
    struct stat st;

    // Si fstat es -1, hay un error
    if (fstat(fd, &st))
    {
        printf("Error al obtener los datos del fichero\n");
        return -1;
    }
    
    // Mapeamos el fichero poco a poco
    // Superbloque
    superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Si no se mapea bien, hay un error
    if(superblock == MAP_FAILED)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }

    // create();

    // Bitmap de bloques
    // block_bitmap = mmap(NULL, sizeof(struct block_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->block_bitmap_first_block * BLOCK_SIZE);
    // Bitmap de inodos
    // inode_bitmap = mmap(NULL, sizeof(struct inode_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->inode_bitmap_first_block * BLOCK_SIZE);
    // int i = free_inode();
    // inode_bitmap->bitmap[i] = new_inode;

    // Mapeamos el primer inodo
    struct inode_fs *inode = mmap(NULL, sizeof(struct inode_fs) * NUM_INODES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (superblock->first_inode_block) * BLOCK_SIZE);
    // Mapeamos los bloques de datos
    // unsigned char *data_block = mmap(NULL, sizeof(char) * BLOCK_SIZE * (NUM_BLOCKS - superblock->first_data_block), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->first_data_block * BLOCK_SIZE);
    
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

    // Cerramos el fichero
    close(fd);
    return 0;
}