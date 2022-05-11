#include <stdlib.h>
#include <unistd.h>

int basic_lex_test001(char *buffer, int *cursor, void *value);

char *readToBuffer(int fd, int inisize, int extrasize, int *readsize)
{
int bsize, r, rsize;
char *buffer, *tmp;

	bsize=inisize;
	if((buffer = malloc(bsize + extrasize))==NULL) return NULL;

	rsize=0;

	do{
		if(rsize>=bsize){
			bsize += bsize;
			if((tmp = realloc(buffer, bsize + extrasize))==NULL){
				*readsize = 0;
				free(buffer);
				return NULL;
			}
			buffer = tmp;
		} 
		r = read(fd, buffer+rsize, bsize - rsize);
		rsize += r;
	}while(r>0);

	*readsize = rsize;
	return buffer;
}

