#ifndef FILE_H
#define FILE_H

#include "proc.h"
#include "types.h"

#define PIPESIZE (512)
#define FILEPOOLSIZE (NPROC * FD_BUFFER_SIZE)

struct pipe {
	char data[PIPESIZE];
	uint nread; // number of bytes read
	uint nwrite; // number of bytes written
	int readopen; // read fd is still open
	int writeopen; // write fd is still open
};

struct file {
	enum { FD_NONE = 0, FD_PIPE } type;
	int ref; // reference count
	char readable;
	char writable;
	struct pipe *pipe; // FD_PIPE
	uint off; // FD_INODE
};

extern struct file filepool[FILEPOOLSIZE];

int pipealloc(struct file *, struct file *);
void pipeclose(struct pipe *, int);
int piperead(struct pipe *, uint64, int);
int pipewrite(struct pipe *, uint64, int);
void fileclose(struct file *);
struct file *filealloc();

#endif //FILE_H