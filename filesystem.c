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
    long inode = search(path,private_data);
    printf("Buscando el inodo de %s con número %ld\n",path,inode);
    memset(stbuf, 0, sizeof(struct stat));
    if(inode == -1){
        res = -ENOENT;
    }else{
        // Copiamos los datos del inodo al stat
        printf("Hemos entrado en el else\n");
        stbuf->st_mode = private_data->inode[inode].i_permission;
        stbuf->st_nlink = private_data->inode[inode].i_links;
        stbuf->st_uid = private_data->st_uid;
        stbuf->st_gid = private_data->st_gid;
        stbuf->st_atime = private_data->st_atime;
        stbuf->st_mtime = private_data->st_mtime;
        stbuf->st_ctime = private_data->st_ctime;
        stbuf->st_size = private_data->inode[inode].i_tam;
        stbuf->st_blocks = private_data->inode[inode].i_tam / BLOCK_SIZE + 1;
        printf("Hemos copiado los datos del inodo al stat\n");
    }
	return res;
}

/***********************************
 * */

static int readdir_fs(const char *path, void *buf, fuse_fill_dir_t filler,
					  off_t offset, struct fuse_file_info *fi)
{
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    long inode = search(path,private_data);
    // Si el inodo no es directorio, devolvemos error
    // Error de conexión con el otro extremo
    if(inode == -1 && strcmp(private_data->inode[inode].i_type,"d") != 0){
        return -ENOENT;
    }

    struct directory_entry *entry;
    int i = 0;
    while(i < N_DIRECTOS && private_data->inode[inode].i_directos[i] != 0){
        entry = (struct directory_entry *) private_data->block[private_data->inode[inode].i_directos[i]];
        for(int j = 0; j < 128 && (strcmp(entry[j].name, "") != 0 || entry[j].inode != 0); j++){
            if(entry[j].inode == -1) continue;
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
    long inode = search(path,private_data);

    if(inode == -1)
    {
        return -ENOENT;
    } 

    fi->fh = inode;

    printf("--------------------------------------------------------------\n");
    printf("El archivo que abrimos: %s\n", path);
    printf("--------------------------------------------------------------\n");

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

int mkdir_fs(const char *path, mode_t mode){
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    //Comprobamos que no exista ya ese path
    if(search(path,private_data) != -1){
        return -EEXIST;
    }
    touch(path, 'd', private_data);
    return 0;
}

int create_fs(const char *path, mode_t mode,  struct fuse_file_info *fi){
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    //Comprobamos que no exista ya ese path
    long inode = search(path,private_data);
    printf("--------------------------------------------------------------\n");
    printf("El inodo es: %ld\n",inode);
    printf("--------------------------------------------------------------\n");
    if(inode != -1){
        return -EEXIST;
    }

    touch(path, '-', private_data);
    fi->fh = search(path,private_data);
    return 0;
}

static int write_fs(const char *path, const char *buf, size_t size, off_t offset,
				   struct fuse_file_info *fi)
{
    printf("--------------------------------------------------------------\n");
    printf("Vamos a escribir\n");
    printf("--------------------------------------------------------------\n");

    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    long inode = search(path,private_data);

    if(inode < 0) return -ENOENT;

    printf("--------------------------------------------------------------\n");
    printf("Path: %s\n y contenido a añadir: %s\n Offset: %d\n",path, buf,offset);
    printf("--------------------------------------------------------------\n");

    //Comprobamos que el offset no sea mayor que el tamaño del fichero
    if(offset > private_data->inode[inode].i_tam){
        return -EFBIG;
    }

    int res = 0;
    //Escribimos el contenido en el fichero
    if(offset > 0){
        res = append(path, buf, private_data);
    }else{
        res = overwrite(path, buf, private_data);
    }

    if(res < 0)
    {
        printf("--------------------------------------------------------------\n");
        printf("Error al escribir\n");
        printf("--------------------------------------------------------------\n");
        return res;
    }

    printf("Contenido copiado: %s\n",buf);

	return size;
}

int rmdir_fuse (const char *path){  //Ya tenemos una funcion que se llama rmdir_fs
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    long inode = search(path,private_data);
    if(inode == -1){
        return -ENOENT;
    }
    if(private_data->inode[inode].i_type != 'd'){
        return -ENOTDIR;
    }
    if(private_data->inode[inode].i_links > 2){
        return -ENOTEMPTY;
    }
    rmdir_fs(path, private_data);
    return 0;
}

int unlink_fuse(const char *path){
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    long inode = search(path,private_data);
    if(inode == -1){
        return -ENOENT;
    }   
    if(private_data->inode[inode].i_type != '-'){
        return -EISDIR;
    }
    unlink_fs(path, private_data);
    return 0;
}

int rename_fuse(const char *path, const char *path_name, unsigned int flags)
{
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados
    long inode = search(path,private_data);

    if(inode == -1){
        return -ENOENT;
    }

    printf("El path es %s\n", path);
    printf("El name es %s\n", path_name);

    char *new_name = malloc(strlen(path_name));
    memset(new_name, 0, strlen(path_name));
    char *token = strtok(path_name, "/");
    while (token != NULL)
    {
        strcpy(new_name, token);
        token = strtok(NULL, "/");
    }
    printf("El new_name es %s\n", new_name);

    rename_file(path, new_name, private_data);

    return 0;
}

int utimens_fuse(const char *path, const struct timespec *tv, struct fuse_file_info *fi)
{
    return 0;
}

int chmod_fuse (const char *path, mode_t permisos, struct fuse_file_info *fi)
{
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados

    long inode_num = search(path, private_data);

    if(inode_num < 0) return -ENOENT;

    // Si existe, cambiamos los permisos
    private_data->inode[inode_num].i_permission = permisos;

    return 0;
}

int truncate_fuse (const char *path, off_t offset, struct fuse_file_info *fi)
{
    filesystem_t *private_data = (filesystem_t *)fuse_get_context()->private_data; // Obtenemos los datos privados

    long inode_num = search(path, private_data);

    if(inode_num < 0) return -ENOENT;

    private_data->inode[inode_num].i_tam = 0;
    
    return 0;
}


/***********************************
 * operaciones FUSE
 * */
static struct fuse_operations basic_oper = {
	.getattr = getattr_fs,
	.readdir = readdir_fs,
	.open = open_fs,
	.read = read_fs,
    .mkdir = mkdir_fs,
    .create = create_fs,
    .write = write_fs,
    .rmdir = rmdir_fuse,
    .unlink = unlink_fuse,
    .rename = rename_fuse,
    .utimens = utimens_fuse,
    .chmod = chmod_fuse,
    .truncate = truncate_fuse,
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
    // Abrir el fichero con el contenido del filesystem
    private_data->fd = open(private_data->fichero, O_RDWR);

    if (private_data->fd == -1)
    {
        perror("open");
        printf("Error al abrir el fichero\n");
        return EXIT_FAILURE;
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
    printf("El magic number es: %d\n", private_data->superblock->MAGIC_NUM);

    // Si no se mapea bien, hay un error
    if (private_data->superblock == MAP_FAILED || private_data->superblock->MAGIC_NUM != 17062023)
    {
        printf("Error al mapear el superbloque\n");
        return EXIT_FAILURE;
    }

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

    // Inicializamos el sistema de ficheros
    printf("Entramos en fuse_main\n");
    return fuse_main(argc, argv, &basic_oper, private_data); // private_data
}