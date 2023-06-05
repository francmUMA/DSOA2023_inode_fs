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
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>


static int getattr_fs(const char *path, struct stat *stbuf)
{
    // Buscamos el inodo
    int res = 0; // Valor a devolver para mostrar que funciona correctamente
    struct inode_fs *inode = search(path);

    if(inode == NULL){
        res = -ENOENT;
    }else{
        // Copiamos los datos del inodo al stat
        stbuf->st_mode = S_IFREG | inode->i_permission;
        stbuf->st_nlink = inode->i_links;
        stbuf->st_uid = private_data->st_uid;
        stbuf->st_gid = private_data->st_gid;
        // Otra barbarie
        stbuf->st_atime = private_data->st_atime;
        stbuf->st_mtime = private_data->st_mtime;
        stbuf->st_ctime = private_data->st_ctime;
        stbuf->st_size = inode->i_tam;
        stbuf->st_blocks = inode->i_tam / BLOCK_SIZE + 1;
    }
    
	return res;
}

/***********************************
 * */

static int readdir_fs(const char *path, void *buf, fuse_fill_dir_t filler,
					  off_t offset, struct fuse_file_info *fi)
{
    struct inode_fs *inode = search(path);
    // Si el inodo no es directorio, devolvemos error
    if(inode == NULL && inode->i_type != "d"){
        return -ENOENT;
    }

    struct directory_entry *entry;
    for(int i = 0; i < N_DIRECTOS; i++){
        entry = private_data->block[inode->i_directos[i]];
        for(int j = 0; j < 128 && entry[j].inode != NULL; j++){
            if(filler(buf, entry[j].name, NULL, 0) != 0) return -ENOMEM;
        }
    }

	return 0;
}

/***********************************
 * */
static int open_fs(const char *path, struct fuse_file_info *fi)
{
	/* completar */
    struct inode_fs *inode = search(path);

    if(inode == NULL)
    {
        fi->fh = -1;
        return -ENOENT;
    } 

    fi->fh = inode->i_num;

	return 0;
}

/***********************************
 * */
static int read_fs(const char *path, char *buf, size_t size, off_t offset,
				   struct fuse_file_info *fi)
{
    int res = fi->fh;
    size_t len;

    if(res < 0) return -ENOENT;

    char *content = read_file(path);
	len = strlen(content);

	if(offset < len){
		if(offset + size > len){
			size = len - offset;
			memcpy(buf, content + offset, size);
		}
	}else
		size = 0;

	return size;
}

// static int write_fs(const char *path, char *buf, size_t size, off_t offset,
// 				   struct fuse_file_info *fi)
// {
// 	return size;
// }

/***********************************
 * operaciones FUSE
 * */
static struct fuse_operations basic_oper = {
	.getattr = getattr_fs,
	.readdir = readdir_fs,
	.open = open_fs,
	.read = read_fs,
};

int main(int argc, char *argv[])
{
    // Inicializamos el sistema de ficheros
    private_data = malloc(sizeof(filesystem_t));

	if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
		return -1;

	private_data->fichero = strdup(argv[argc - 2]); // fichero donde están los capítulos
	argv[argc - 2] = argv[argc - 1];
	argv[argc - 1] = NULL;
	argc--;

    // Abrir el fichero con el contenido del filesystem
    private_data->fd = open("filesystem.img", O_RDWR, 0666);

    if (private_data->fd == -1)
    {
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
    num_blocks = st.st_size / BLOCK_SIZE;
    num_inodes = (num_blocks / 4) * BLOCK_SIZE / sizeof(struct inode_fs);
    blocks_for_inodes = num_blocks / 4;
    blocks_for_inode_bitmap = (num_inodes / 8) / BLOCK_SIZE + 1;
    blocks_for_block_bitmap = (num_blocks / 8) / BLOCK_SIZE + 1;
    reserved_blocks = blocks_for_inodes + blocks_for_inode_bitmap + blocks_for_block_bitmap + 1;

    private_data->superblock = mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd, 0);

    // Si no se mapea bien, hay un error
    if (private_data->superblock == MAP_FAILED)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }

    init_superblock(private_data -> superblock);

    // Mapeamos el bitmap de bloques
    private_data->block_bitmap = mmap(NULL, blocks_for_block_bitmap * BLOCK_SIZE,
                                      PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd,
                                      private_data->superblock->block_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if (private_data->block_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de bloques\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el bitmap de inodos
    private_data->inode_bitmap = mmap(NULL, blocks_for_inode_bitmap * BLOCK_SIZE,
                                      PROT_READ | PROT_WRITE, MAP_SHARED, private_data->fd,
                                      private_data->superblock->inode_bitmap_first_block * BLOCK_SIZE);

    // Si no se mapea bien, hay un error
    if (private_data->inode_bitmap == MAP_FAILED)
    {
        printf("Error al mapear el bitmap de inodos\n");
        return EXIT_FAILURE;
    }

    // Mapeamos el primer inodo
    private_data->inode = mmap(NULL, blocks_for_inodes * BLOCK_SIZE,
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

    // Cerramos el fichero
    close(private_data->fd);
    init_block_bitmap();

    (void) create_root(); // Otra barbaridad
    // Joder, esto es una barbaridad
    // Pero no se me ocurre otra forma de hacerlo
    // Y no tengo tiempo para pensar en otra forma
	// análisis parámetros de entrada
    return fuse_main(argc, argv, &basic_oper, NULL);
}