#include "file.h"
#include "defs.h"
#include "proc.h"
#include "types.h"

struct file filepool[FILEPOOLSIZE];

void fileclose(struct file *f)
{
	if (f->ref < 1)
		panic("fileclose");
	if (--f->ref > 0) {
		return;
	}

	if (f->type == FD_PIPE) {
		pipeclose(f->pipe, f->writable);
	}
	f->off = 0;
	f->readable = 0;
	f->writable = 0;
	f->ref = 0;
	f->type = FD_NONE;
}

struct file *filealloc()
{
	for (int i = 0; i < FILEPOOLSIZE; ++i) {
		if (filepool[i].ref == 0) {
			filepool[i].ref = 1;
			return &filepool[i];
		}
	}
	return 0;
}
