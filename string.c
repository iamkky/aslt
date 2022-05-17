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

StBuffer stBufferNew(int alloc)
{
StBuffer self;

	if((self=malloc(CSIZE(StBuffer)))==NULL) return NULL;

	if(alloc <= 0) alloc = 128;

	self->size = 0;
	self->allocated = alloc;
	self->buffer = malloc(sizeof(char) * self->allocated);

	if(self->buffer == NULL){
		free(self);
		return NULL;
	}

	self->buffer[0] = 0;

	return self;
}

int stBufferAppend(StBuffer self, char *str)
{
char *tmp;
int len1, len2, s;

	if(self==NULL) return -1;

	len1 = strlen(self->buffer);
	len2 = strlen(str);

	if(len1 + len2 + 1 >= self->allocated){
		s = self->allocated;
		while(len1 + len2 + 1 > s) s *= 2;
		if((tmp = realloc(self->buffer, sizeof(char *) * s))==NULL) return -1;
		self->allocated = s;
		self->buffer = tmp;
	}

	strcpy(self->buffer + len1, str);
	self->size = len1 + len2;

	return len2;
}

int stBufferAppendf(StBuffer self, char *fmt, ...)
{
va_list va;
char *tmp;
int len1, len2, s;

	if(self==NULL) return -1;

	va_start(va, fmt);

	len1 = strlen(self->buffer);
	len2 = vsnprintf(self->buffer + len1, self->allocated - len1, fmt, va);

	if(len1 + len2 + 1 >= self->allocated){
		s = self->allocated;
		while(len1 + len2 + 1 > s) s *= 2;
		if((tmp = realloc(self->buffer, sizeof(char *) * s))==NULL) return -1;
		self->allocated = s;
		self->buffer = tmp;
	}

	len2 = vsnprintf(self->buffer + len1, self->allocated - len1, fmt, va);
	self->size = len1 + len2;

	return len2;
}

