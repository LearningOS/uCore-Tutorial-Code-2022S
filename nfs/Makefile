.DEFAULT_GOAL = fs.img

U := ../user
USER_BIN_DIR := target/bin

FS_FUSE := fs

$(FS_FUSE): fs.c fs.h types.h

fs.img: $(FS_FUSE)
	./$(FS_FUSE) $@ $(wildcard $(U)/$(USER_BIN_DIR)/*)

clean:
	rm *.img $(FS_FUSE)