#define FUSE_USE_VERSION 26

#include "data_structures_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bitmap.c"
#include "inode.c"
#include "init.c"
#include "file_manager.c"
#include "tree_manager.c"
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    // Inicializamos el sistema de ficheros
    filesystem_t *private_data = malloc(sizeof(filesystem_t));

	private_data->fichero = strdup(argv[argc - 1]); // fichero 

    printf("Fichero: %s\n", private_data->fichero);
	// análisis parámetros de entrada
    // Nos traemos todo el contenido del fichero en private_data
    // Abrir el fichero con el contenido del filesystem
    private_data->fd = open(private_data->fichero, O_RDWR);

    if (private_data->fd == -1)
    {
        perror("open");
        printf("Error al abrir el fichero\n");
        return -1;
    }

    // Cogemos los datos del fichero binary
    struct stat st;

    // Si fstat es -1, hay un error
    if (fstat(private_data->fd, &st))
    {
        printf("Error al obtener los datos del fichero\n");
        return -1;
    }

    private_data->st_uid= st.st_uid;
	private_data->st_gid= st.st_gid;
	private_data->st_atime = st.st_atime;
	private_data->st_ctime = st.st_ctime;
	private_data->st_mtime = st.st_mtime;

    // Inicializamos la informacion privada del sistema de ficheros
    private_data->num_blocks = st.st_size / BLOCK_SIZE;
    private_data->num_inodes = (private_data->num_blocks / 4) * BLOCK_SIZE / sizeof(struct inode_fs);
    private_data->blocks_for_inodes = private_data->num_blocks / 4;
    private_data->blocks_for_inode_bitmap = (private_data->num_inodes / 8) / BLOCK_SIZE + 1;
    private_data->blocks_for_block_bitmap = (private_data->num_blocks / 8) / BLOCK_SIZE + 1;
    private_data->reserved_blocks = private_data->blocks_for_inodes + private_data->blocks_for_inode_bitmap + private_data->blocks_for_block_bitmap + 1;

    private_data->superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd, 0);

    // Si no se mapea bien, hay un error
    if (private_data->superblock == MAP_FAILED)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }

    init_superblock(private_data);
    // Mapeamos el bitmap de bloques
    private_data->block_bitmap = mmap(NULL, private_data->blocks_for_block_bitmap * BLOCK_SIZE,
                                      PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd,
                                      private_data->superblock->block_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if (private_data->block_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de bloques\n");
        return EXIT_FAILURE;
    }
    
    // Mapeamos el bitmap de inodos
    private_data->inode_bitmap = mmap(NULL, private_data->blocks_for_inode_bitmap * BLOCK_SIZE,
                                      PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd,
                                      private_data->superblock->inode_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if (private_data->inode_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de inodos\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer inodo
    private_data->inode = mmap(NULL, private_data->blocks_for_inodes * BLOCK_SIZE,
                               PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd,
                               private_data->superblock->first_inode_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if (private_data->inode == MAP_FAILED)
    {
        printf("Error al mapear el primer inodo\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer bloque de datos
    private_data->block = mmap(NULL,
                               st.st_size,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               private_data->fd,
                               0);

    // Si no se mapea bien, hay un error
    if (private_data->block == MAP_FAILED)
    {
        printf("Error al mapear el primer bloque de datos\n");
        return EXIT_FAILURE;
    }

    init_block_bitmap(private_data);
    // No deja crear el root
    create_root(private_data);
  
    close(private_data->fd);
    return 0;
}