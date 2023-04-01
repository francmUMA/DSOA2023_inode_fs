#include "data_structures_fs.h"
#include <stdio.h>
#include <stdint.h>

//Busqueda del inodo "target" en cuestion en el directorio "directory"
static struct inode *inode_search(char *target, struct inode directory){
    /*  
     * found -> booleano que indica que ha encontrado el inodo
     *
     * 1. Inicializar variable a cero
     * 2. Bucle que por iteración traiga un bloque hasta que traiga todos los bloques o encuentre el inodo
     * 3. Recorrer el bloque de forma recursiva y si encontramos un directorio, nos metemos dentro a buscar
     * con _inode_search(*target, ..., &found)
     * 4. Una vez se encuentra, se devuelve el inodo. En caso contrario, se sigue buscando hasta que termina y se devulve NULL.
     */
    uint8_t found = 0;
    return _inode_search(target,directory,&found);
}

static struct inode *_inode_search(char *target, struct inode directory, uint8_t *found){
    int i = 0;
    directory_entry entry;
    while(i < N_DIRECTOS && !found && directory.i_directos[i] != NULL){
        // Mapeo del bloque 
        
        // Recorremos el bloque
        // Para cada elemento comprobar si es target
        // Si es target hacemos return del inodo. Si no es, comprobamos si es directorio o archivo. En caso de ser directorio
        // entramos en el mismo. Continuamos en caso contrario.

    }
    return NULL;
}
