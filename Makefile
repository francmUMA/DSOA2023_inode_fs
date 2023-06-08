montaje=punto_montaje
fichero=filesystem.img

fuse_flags= -D_FILE_OFFSET_BITS=64 -lfuse -pthread

filesystem_fuse : filesystem.o
	gcc -g -o $@ $< ${fuse_flags}
	mkdir -p $(montaje)
	
filesystem.o : filesystem.c data_structures_fs.h
	gcc -g -c -o $@  $< ${fuse_flags}

mount: filesystem_fuse
	./filesystem_fuse $(fichero) $(montaje)

clean:
	fusermount -u $(montaje)
	rm -rf filesystem_fuse filesystem.o punto_montaje 
