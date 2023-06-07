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
    filesystem_t *private_data = (filesystem_t *) fuse_get_context()->private_data; // Obtenemos los datos privados
    // Buscamos el inodo
    int res = 0; // Valor a devolver para mostrar que funciona correctamente
    struct inode_fs *inode = search(path, private_data);

    memset(stbuf, 0, sizeof(struct stat));
    if(inode == NULL){
        res = -ENOENT;
    }else{
        printf("Path: %s, Inodo: %s",path,inode->entry->name);
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
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    struct inode_fs *inode = search(path,private_data);
    // Si el inodo no es directorio, devolvemos error
    // Error de conexión con el otro extremo
    if(inode == NULL && strcmp(inode->i_type,"d") != 0){
        return -ENOENT;
    }

    struct directory_entry *entry;
    int i = 0;
    while(i < N_DIRECTOS && &inode->i_directos[i] != NULL){
        entry = (struct directory_entry *) private_data->block[inode->i_directos[i]];
        for(int j = 0; j < 128 && &private_data->inode[entry[j].inode] != NULL; j++){
            if(filler(buf, entry[j].name, NULL, 0) != 0) return -ENOMEM;
        }
        i++;
    }
    
	return 0;
}

/***********************************
 * */
static int open_fs(const char *path, struct fuse_file_info *fi)
{
    filesystem_t *private_data = (filesystem_t *) fuse_get_context()->private_data; // Obtenemos los datos privados
	/* completar */
    struct inode_fs *inode = search(path,private_data);

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
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    int res = fi->fh;
    size_t len;

    if(res < 0) return -ENOENT;

    char *content = read_file(path,private_data);
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
    filesystem_t *private_data = malloc(sizeof(filesystem_t));

	if ((argc < 3) || (argv[argc - 2][0] == '-') || (argv[argc - 1][0] == '-'))
		return -1;

	private_data->fichero = strdup(argv[argc - 2]); // fichero 
	argv[argc - 2] = argv[argc - 1];
	argv[argc - 1] = NULL;
	argc--;
	// análisis parámetros de entrada
    // Nos traemos todo el contenido del fichero en private_data

    // Inicializamos el sistema de ficheros
    printf("Entramos en fuse_main\n");
    return fuse_main(argc, argv, &basic_oper, private_data); // private_data
}