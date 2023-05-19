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
    struct block_bitmap_fs *block_bitmap_aux = malloc(sizeof(struct block_bitmap_fs));
    
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

    // Cogemos el tamaño y el numero de bloques es st.size / BLOCK_SIZE
    long num_blocks = st.st_size/4096;
    
    // Mapeamos el fichero poco a poco
    // Superbloque
    superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // Bitmap de bloques
    block_bitmap = mmap(NULL, sizeof(struct block_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->block_bitmap_first_block * BLOCK_SIZE);
    // Bitmap de inodos
    inode_bitmap = mmap(NULL, sizeof(struct inode_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->inode_bitmap_first_block * BLOCK_SIZE);
    // Mapeamos el primer inodo
    struct inode_fs *inode = mmap(NULL, sizeof(struct inode_fs) * NUM_INODES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->first_inode_block * BLOCK_SIZE);
    // Mapeamos los bloques de datos
    unsigned char *data_block = mmap(NULL, sizeof(char) * BLOCK_SIZE * (NUM_BLOCKS - superblock->first_data_block), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->first_data_block * BLOCK_SIZE);
    create();
    msync(superblock,BLOCK_SIZE,MS_SYNC);
    // Cerramos el fichero
    close(fd);
    return 0;
}