#ifndef _RDPP_STRING_H_
#define _RDPP_STRING_H_

#define CSIZE(class)    (sizeof(*(class)0))

typedef struct StList_struct *StList;
struct StList_struct {
	char **list;
	int size;
	StList jj;
};

StList stListNew(int size);
int stListRegister(StList self, char *str);
int stListToupper(StList self);

typedef struct StBuffer_struct *StBuffer;
struct StBuffer_struct {
	char *buffer;
	int  allocated;
	int  size;
};

StBuffer stBufferNew(int alloc);
int stBufferAppend(StBuffer self, char *str);
int stBufferAppendf(StBuffer self, char *fmt, ...);

#endif
