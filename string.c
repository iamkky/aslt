#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#define CSIZE(class)    (sizeof(*(class)0))

typedef struct StList_struct *StList;
struct StList_struct {
	char **list;
	int size;
	StList jj;
};

typedef struct StBuffer_struct *StBuffer;
struct StBuffer_struct {
	char *buffer;
	int  allocated;
	int  size;
};

StList stListNew(int size)
{
StList self;

	if((self=malloc(CSIZE(StList)))==NULL) return NULL;

	self->size = size;
	if((self->list = malloc(sizeof(char *) * self->size)) == NULL){
		free(self);
		return NULL;
	}

	self->list[0] = NULL;

	return self;
}

int stListRegister(StList self, char *str)
{
char **tmp;
int c;

	if(self==NULL) return -1;

	for(c=0; c<=self->size; c++){
		if(self->list[c]==NULL) break;
		if(strcmp(self->list[c],str)==0) return c;
	}

	if(c + 1>=self->size){
		if((tmp = realloc(self->list, sizeof(char *) * self->size * 2))==NULL) return -1;
		self->size *= 2;
		self->list = tmp;
	}

	self->list[c] = strdup(str);
	self->list[c + 1] = NULL;
}

int stListToupper(StList self)
{
char	*p;
int	c;

	for(c=0; c<self->size; c++){
                if(self->list[c] == NULL) break;
		for(p=self->list[c]; *p; p++) *p = toupper(*p);
        }	
	return 0;
}

#define errLogf(msg) fprintf(stderr, "Error: %s\n", msg)

StBuffer stBufferNew(int initialSize)
{
StBuffer self;

        if((self = malloc(sizeof(struct StBuffer_struct))) == NULL) {
                errLogf("Malloc failed in StBuffer\n");
                return NULL;
        }

	self->size = 0;

	if(initialSize == 0){
		self->buffer = NULL;
		self->allocated = 0;
	}else{
		if((self->buffer = malloc(initialSize)) == NULL) {
			free(self);
			errLogf("Malloc failed in StBuffer\n");
			return NULL;
		}
		self->buffer[0] = 0;
		self->allocated = initialSize;
	}

	return self;
}

//Fixit: Can use a more sophisticated size selection to alloc or realloc.
int stBufferCheckExpand(StBuffer self, int extension)
{
        if(self->size + extension >= self->allocated){
                if(self->allocated == 0){
			// Alloc required space plus 128 extra bytes to avoid recorrent calls to realloc
                        self->buffer = malloc(sizeof(char *) * (extension + 128));
			if(self->buffer == NULL) return 0;
                        self->allocated = extension + 128;
                }else{
			// Alloc required space plus 50% of current size to avoid recurrent calls to realloc
                        self->buffer = realloc(self->buffer, sizeof(char *) * (self->allocated + extension + (self->size/2) + 1));
			if(self->buffer == NULL) return 0;
                        self->allocated = self->allocated + extension + (self->size/2) + 1;
                }
        }

	return 1;
}

int stBufferAppendf(StBuffer self, char *fmt, ... )
{
va_list	args;
int	len;

	if(self==NULL) return -1;
	if(fmt==NULL) return -1;

	va_start(args, fmt);
	len = vsnprintf(self->buffer + self->size, self->allocated - self->size, fmt, args);
	va_end(args);

	if(len >= self->allocated - self->size){
		// Overflow
		// errLogf("Overflow: %d", self->allocated);
		if(stBufferCheckExpand(self, len)){
			va_start(args, fmt);
			len = vsnprintf(self->buffer + self->size, self->allocated - self->size, fmt, args);
			va_end(args);
		}
	}

	self->size = self->size + len;

	return 0;
}

StBuffer stBufferAppend(StBuffer self, const char *str)
{
int len;

	if(!self) return NULL;
	if(!str) return NULL;

	if(stBufferCheckExpand(self, len = strlen(str))){
		strcat(self->buffer, str);
		self->size += len;
		return self;
	}

	return NULL;
}

