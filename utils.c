#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

// usefull tools

int unhex(char ch)
{
	if(ch>='0' && ch<='9') return ch-'0';
	if(ch>='A' && ch<='F') return ch-'A' + 10;
	if(ch>='a' && ch<='f') return ch-'a' + 10;
	return 0;
}

int printHexString(FILE *fp, char *str)
{
int ch;

	while(*str){
		ch = ((unsigned int)(unhex(*str++)))<<4;
		if(*str==0) break;
		ch += ((unsigned int)(unhex(*str++)));
		fputc(ch, fp);
	}
	return 0;
}

int printHexStringReplace(FILE *fp, char *str, char value[][1024])
{
int ch, c;

	c = 0;
	while(*str){
		ch = ((unsigned int)(unhex(*str++)))<<4;
		if(*str==0) break;
		ch += ((unsigned int)(unhex(*str++)));
		if(c==0){
			if(ch=='$')
				c++;
			else
				fputc(ch, fp);
		}else{
			if(ch>='0' && ch<='9'){
				fputs(value[ch-'0'], fp);
			}else{
				fputc('$',fp);
				fputc(ch, fp);
			}
			c = 0;
		}
	}
	if(c==1) fputc('$', fp);
	return 0;
}

int isSpace(int ch)
{
	return (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r');
}

char *copyPrecode(FILE *fp, char *source)
{
int firstchar = 1;

	while(*source){
		if(firstchar==1){
			if(*source=='%' && *(source+1)=='%') return source+2;
			firstchar = 0;
		}
		if(*source=='\n') firstchar = 1;
		putc(*source++, fp);
	}
	return source;
}


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

