#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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

int printHexStringReplace(FILE *fp, char *str, char *value[])
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
				if(value[ch-'0']) fputs(value[ch-'0'], fp);
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

FILE *openOutputFile(char *source, char *sext, char *dext)
{
FILE *fout;
char *output;
int  slen, sextlen, dextlen;

	slen = strlen(source);
	sextlen = strlen(sext);
	if(slen<=sextlen) return NULL;

	dextlen = strlen(dext);

	if((output = malloc(slen - sextlen + dextlen + 1))==NULL) return NULL;
	strncpy(output, source, slen - sextlen);
	strcpy(output + slen - sextlen, dext);	
	
	fout=fopen(output,"w");
	if(fout==NULL) fprintf(stderr,"Could not open output file: %s\n", output);

	free(output);
	
	return fout;
}

char *getCode(char *input, int *cursor, int *lcount)
{
char *tmp, *p, last;
int  count, level, string, character, escaped;

	if(*(p=input++)!='{') return NULL;

	count = level = string = character = escaped = 0;

	while((last = *input++)){
		count++;
		if(last=='\n' && lcount!=NULL) *lcount = *lcount + 1;
		if(escaped){
			escaped = 0;
		}else if(character==0 && last=='"'){
			if(string == 0) string = 1; else string = 0;
		}else if(string==1 && last=='\\'){
			escaped = 1;
		}else if(string==0 && last=='\''){
			if(character == 0) character = 1; else character = 0;
		}else if(character==1 && last=='\\'){
			escaped = 1;
		}else if(!string && !character && last=='{'){
			level++;
		}else if(!string && !character && last=='}'){
			if(level--==0) break;
		}
	}

	if(last>0){
		if((tmp = malloc(count))==NULL) return NULL;
		strncpy(tmp, p+1, count-1);
		tmp[count-1] = 0;
	}else{
		tmp=strdup("");
	}

	(*cursor) += count;
	return tmp;
}

