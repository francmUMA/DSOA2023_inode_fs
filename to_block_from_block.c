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
    // create();
    // Abrir un fichero binario donde vamos a mapear toda la informacion
    fd = open("test.bin", O_CREAT | O_RDWR, 0666);

    if (fd == -1)
    {
        printf("Error al abrir el fichero\n");
        return -1;
    }

    // Creamos block e inode bitmap
    // block_bitmap = malloc(sizeof(struct block_bitmap_fs));
    // inode_bitmap = malloc(sizeof(struct inode_bitmap_fs));
    // memset(block_bitmap, 0, sizeof(struct block_bitmap_fs));
    // memset(inode_bitmap, 0, sizeof(struct inode_bitmap_fs));

    // // Escribimos el superblock en el fichero
    // write(fd, superblock, sizeof(struct superblock_fs));

    // // Nos desplazamos a la posicion del primer bloque del bitmap de bloques
    // lseek(fd, superblock->block_bitmap_first_block * BLOCK_SIZE, SEEK_SET);
    // write(fd, block_bitmap, sizeof(struct block_bitmap_fs));

    // // Lo mismo pero para el bitmap de inodos
    // lseek(fd, superblock->inode_bitmap_first_block * BLOCK_SIZE, SEEK_SET);
    // write(fd, inode_bitmap, sizeof(struct inode_bitmap_fs));

    // Escribimos un entero en el 4 byte
    // lseek(fd, 8, SEEK_SET);
    // int test1 = 2;
    // write(fd, &test1, sizeof(int));

    // // Mapeamos el entero que hay en el byte 8
    // int *test2 = mmap(NULL, 3*sizeof(int), PROT_READ, MAP_SHARED, fd, 0);
    // printf("El entero es: %d\n", test2[2]);

    // // Unmapeamos el entero
    // munmap(test2, 3*sizeof(int));

    // // Eliminamos el entero
    // lseek(fd, 8, SEEK_SET);
    // int test3 = 0;
    // write(fd, &test3, sizeof(int));

    // Creamos el bitmap de bloques
    // Mostramos el tamaño del bitmap
    // printf("El tamaño del bitmap de bloques es: %ld\n", sizeof(struct block_bitmap_fs));
    // block_bitmap = malloc(sizeof(struct block_bitmap_fs));
    // memset(block_bitmap, 0, sizeof(struct block_bitmap_fs));
    // int indice1 = free_block();
    // write(fd, block_bitmap, sizeof(struct block_bitmap_fs));
    // free(block_bitmap);
    superblock = (struct superblock_fs *) mmap(NULL, sizeof(struct superblock_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    block_bitmap = (struct block_bitmap_fs *) mmap(NULL, sizeof(struct block_bitmap_fs), PROT_READ | PROT_WRITE, MAP_SHARED, fd, superblock->block_bitmap_first_block * BLOCK_SIZE);
    int indice1 = free_block();
    munmap(block_bitmap, sizeof(struct block_bitmap_fs));
    munmap(superblock, sizeof(struct superblock_fs));

    // Cerramos el fichero
    close(fd);

    return 0;
}