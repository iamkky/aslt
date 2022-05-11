#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "nrlex.usagestr.h"

// 
// Those lines will be converted to a hex coded string and used to populate file 
// nrlex.usagestr.h (providing char *usagestr)
// 

//UU
//UU nrlex - A Very Simple Non-regex Lex generator
//UU
//UU Generates a C code that can tokenize a buffer according to no-regex rules given in a input file
//UU
//UU
//UU
//UU Accepts characters, escaped characters and '.' as any character
//UU
//UU File format:
//UU
//UU <prefix code>
//UU %%<name> <value type>
//UU <rule> : <code>
//UU <rule> : <code>
//UU
//UU <prefix code> = C source code that will prefix the generated output
//UU <name> = Parser function name
//UU <rule> = Sequence of charactes, special characters or escape sequences
//UU <code> = Action to be performed in case of match (a single line to C code)
//UU <value type> = is the C type used to declare value parameter below.
//UU
//UU <value type> does not accept space. "char*" is a valid type. "char *" is not.
//UU for complex types (structs or unios) best to use a previous typedefed type.
//UU
//UU code can use special local variable "input" to reffer to string matched
//UU
//UU The generator will create a function named <name> protyped as follow:
//UU
//UU int <name>(char *buffer, int *cursor, <value type> *value, void *extra)
//UU
//UU   input is the input buffer.
//UU   cursor is reading cursor and should be load with zero (0) at first call.
//UU   value is a optional return value to be used in code segment.
//UU   extra is an way to preserve state between call.
//UU
//UU   The generated parser provides a few useful variable to be used in <code>
//UU
//UU   input - is always pointing to current character in buffer
//UU   size -  is the size in characters of the current token
//UU
//UU   char *nrDupToken(input, size) - Alloc memory and copy the current token (like strdup)
//UU
//UU   Important Note:
//UU
//UU		Generated code does not contains any include.
//UU		Generated code uses malloc in implementaion of nrDupToken
//UU
//UU Special characters and escaped characters
//UU
//UU  - Special Character
//UU     . - Matchs any character
//UU
//UU  - ASCII Escape Sequences:
//UU
//UU     \b - Backspace (8)
//UU	 \e - ASCII escape code
//UU     \f - Formfeed
//UU	 \n - New Line
//UU	 \r - Carriage Return
//UU     \t - Horizontal Tab
//UU     \v - Vertical Tab
//UU     \s - Matchs white space (32)
//UU
//UU  - Special Escape Sequences:
//UU
//UU     \\ - Matchs \\
//UU     \. - Matchs .
//UU     \: - Matchs :
//UU     \% - Matchs %
//UU     \D - Matchs [0-9]+
//UU     \a - Matchs [a-z]+
//UU     \A - Matchs [A-Z]+
//UU     \X - Matchs [a-zA-Z]+
//UU     \Y - Matchs [0-9a-zA-Z]+
//UU     \I - Matchs [a-zA-Z][0-9a-zA-Z]*
//UU
//UU

static int verbose = 1;

int escapeCode(int ch)
{
	switch(ch){
	case 'b': return 0x08;
	case 'e': return 0x1b;
	case 'f': return 0x0c;
	case 'n': return 0x0a;
	case 'r': return 0x0d;
	case 't': return 0x09;
	case 'v': return 0x0b;
	case '\\': return '\\';
	case '%': return '%';
	case '.': return '.';
	case ':': return ':';
	case 's': return ' ';
	}
	return -1;
}

int getNextChar(char *source, int *size)
{
char ch;

	if(*source==0) {
		*size = 0;
		return -1;
	}
	if(*source=='\\'){
		ch = escapeCode(*source+1);
		if(ch>0) {
			*size = 2;
			return ch;
		}
		return -1;
	}
	*size = 1;
	return *source;
}

int getNextCharInLine(char *source, int *size)
{
	if(*source=='\n' || *source=='\r') {
		*size = 0;
		return -1;
	}
	return getNextChar(source, size);
}

int putLocalFunctions()
{
	fputs(
"\n\n\
static int nrCheckSpecial(int type, int ch) \n\
{ \n\
	switch(type){ \n\
	case 'A': return ch>='A' && ch <='Z'; \n\
	case 'a': return ch>='a' && ch <='z'; \n\
	case 'D': return ch>='0' && ch <='9'; \n\
	case 'X': return (ch>='A' && ch <='Z') || (ch>='a' && ch <='z'); \n\
	case 'Y': return (ch>='A' && ch <='Z') || (ch>='a' && ch <='z') || (ch>='0' && ch <='9'); \n\
	} \n\
	return 0; \n\
} \n\n\
\n\n\
static char *nrDupToken(char *input, int size) \n\
{\n\
char *tmp, *pd;\n\
\n\
	if((tmp=pd=malloc(size+1))==NULL) return NULL; \n\
	while(size-->0){ \n\
		if((*pd++ = *input++) == 0) return tmp; \n\
	} \n\
	*pd=0; \n\
	return tmp; \n\
} \n\n\
\n\n\
", stdout);
}

int printif(int tab, char *fmt, ...)
{
va_list ap;
int	r;

	va_start(ap, fmt);
	while(tab-->0){
		putchar('\t');
	}
	r = vprintf(fmt, ap);
	va_end(ap);
	return r;
}

int isValidNameChar(int ch)
{
	if(ch=='_') return 1;
	if(ch>='0' && ch<='9') return 1;
	if(ch>='A' && ch<='Z') return 1;
	if(ch>='a' && ch<='z') return 1;
	return 0;
}

char *processDefine(char *source, char *defines, int *defcount)
{
char def, ch0, ch1, c;
int  csize;

	source += 7;

	while(*source==' ' || *source=='\t') source++;
	def = *source;
	if((def>='A' && def<='Z') || (def>='a' && def<='z') || (def>='0' && def<='9')){
		defines[*defcount] = def;
		source++;
		*defcount += 1;
	}else{
		fprintf(stderr,"Unexpected define id %d, Expected A-Za-z0-9 as id\n", def);
		exit(-1);
	}
	printif(0,"int static nrlex_check_%c(int ch)\n",def);
	printif(0,"{\n",def);

	while(*source==' ' || *source=='\t') source++;
	while((ch0 = getNextCharInLine(source, &csize)) > 0) {

		if(ch0<0) {
			fprintf(stderr,"Unexpected caracter in the definition of %c\n", def);
			exit(-1);
		}
		source+=csize;

		if(*source=='-'){
			source++;
			ch1 = getNextCharInLine(source, &csize);
			if(ch1<0) {
				fprintf(stderr,"Unexpected caracter in the definition of %c after -\n", def);
				exit(-1);
			}
			if(ch0>ch1){
				fprintf(stderr,"Unvalid character range in define of %c\n", def);
			}
			source+=csize;
			printif(1,"if(ch>=%d && ch<=%d) return 1;\n", ch0, ch1);
		}else{
			printif(1,"if(ch==%d) return 1;\n", ch0);
		}
	}

	printif(1,"return 0;\n");
	printif(0,"}\n");
	printif(0,"\n");

	return source;
}

int checkDefined(char *defines, int ch)
{
	while(*defines){
		if(*defines == ch) return 1;
		defines++;
	}
	return 0;
}

int nrlex(char *source)
{
#define START 30
int state, code;
int label, abortlabel, recurlabel;
int c, st;
int defcount;
char lexname[256],typename[256],defines[256];

	putLocalFunctions();

	// Consume white spaces
	while(*source==' ' || *source=='\t') source++;
	for(c=0; isValidNameChar(*source) && c<255; c++) lexname[c] = *source++;
	lexname[c]=0;

	if(strlen(lexname)==0) {
		fprintf(stderr,"Missing lex name\n");
		exit(-1);
	}

	// Consume white spaces
	while(*source==' ' || *source=='\t') source++;
	for(c=0; (isValidNameChar(*source) || *source=='*') && c<255; c++) typename[c] = *source++;
	typename[c]=0;

	if(strlen(typename)==0) {
		fprintf(stderr,"Missing lex return value type\n");
		exit(-1);
	}

	// Process directives
	defcount = 0;
	while(*source==' ' || *source=='\t' || *source=='\n' || *source=='\r' || *source=='%'){
		if(*source!='%'){
			source++;
			continue;
		}	
		if(strcmp(source,"%define")){
			if(defcount>254){
				fprintf(stderr,"Define limit reached\n");
				exit(-1);
			}
			source = processDefine(source, defines, &defcount);
		}
	}	

	defines[defcount] = 0;

	label=0;
	state = START;

	printif(0,"int %s(char *buffer, int *cursor, %s *value, void *extra)\n", lexname, typename);
	printif(0,"{\n");
	printif(0,"char  *input;\n");
	printif(0,"int   size;\n");
	printif(0,"\n");
	printif(1,"size=0;\n");
	printif(1,"\n");
	printif(1,"LSTART:\n");
	printif(1,"input=buffer+(*cursor);\n");
	printif(1,"\n");

	while(*source){
		switch(state){
		case START:
			abortlabel = label++;
			printif(1,"size=0;\n");
			state=50;
			break;
		case 50: // Consume white spaces
			if(isSpace(*source)){
				source++;
			}else{
				state=100;
			}
			break;
		case 100: // Check character
			if(isSpace(*source)){
				state=150;
			}else if(*source=='\\'){
				state=110;
			}else if(*source=='.'){
				printif(1,"L%06d:\n", label++);
				printif(1,"if(input[size]!=0) ");
				state = 125;
			}else{
				printif(1,"L%06d:\n", label++);
				printif(1,"if(input[size]==%d) ", *source);
				state = 125;
			}
			source++;
			break;
		case 110: // Escape chars and defines
			st = *source;
			code = escapeCode(st);
			if(code>=0){
				// ASCII Escape code and '\.', '\:', '\\'
				printif(1,"L%06d:\n", label++);
				printif(1,"if(input[size]==%d) ", code);
				state = 125;
			}else{
				if(!checkDefined(defines, st)){
					fprintf(stderr,"Error: Undefined sequence \\%c\n", st);
					return -1;
				}
				printif(1,"L%06d:\n", label++);
				printif(1,"if(nrlex_check_%c(input[size])) ", st);
				state = 125;
			}
			source++;
			break;
		case 125: // Check Closure
			if(*source=='*'){
				source++;
				printif(0,"{ size++; goto L%06d; }\n", label-1);
			}else{
				printif(0,"{ size++; } else goto L%06d;\n", abortlabel);
			}
			state = 100;
			break;
		case 150: // Consume white spaces
			if(isSpace(*source)){
				source++;
			}else{
				state=200;
			}
			break;
		case 200: // Check for ':'
			if(*source==':'){
				source++;
				state=300;
				printif(1,"*cursor+=size;\n");
				printif(2,""); // Two tabs
			}else{
				fprintf(stderr,"Error: missing :\n");
				return -1;
			}
			break;
		case 300: // accept user code block
			if(*source!='\n' && *source!=0){
				putchar(*source++);
			}else{
				printif(1,"\n");
				printif(1,"goto LSTART;\n");
				printif(1,"L%06d:\n", abortlabel);
				printif(1,"\n");
				if(*source=='\n') source++;
				state=START;
			}
			break;
		default:
			fprintf(stderr,"State machine panic: Uknow state %d\n", state);
			return -1;
		}
	}
	printif(1,"return -1;\n");

	printif(0,"}\n");

	return 0;
}


int main(int argc, char **argv)
{
int  size;
char *buffer, *rules;

	if(argc>1){
		printHexString(stdout,usagestr);
		exit(0);
	}

	buffer = readToBuffer(0, 32768, 1, &size);
	buffer[size]=0;

	rules = copyPrecode(stdout, buffer);
	nrlex(rules);
}

