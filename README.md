# DSOA2023_inode_fs
# Compilacion
make -f Makefile_format fichero=nombre_fichero
./mk_filesystem nombre_fichero
make -f Makefile fichero=nombre_fichero montaje=punto_montaje
./filesystem_fuse nombre_fichero punto_montaje

