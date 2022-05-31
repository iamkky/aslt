#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "nrlex.usagestr.h"

// 
// Those lines will be converted to a hex coded string and used to populate file nrlex.usagestr.h
// (providing char *usagestr)
// 

//UU
//UU nrlex - A Very Simple Non-regex Lex generator
//UU
//UU
//UU 	Generates a C code that can tokenize a buffer according to no-regex rules given in a input file
//UU
//UU 	invocation: nrlex [-H <header file>] [-v]
//UU
//UU 	nrlex reads from standard input, and output to standard output
//UU 
//UU File format:
//UU ^^^^^^^^^^^^
//UU
//UU <prefix code>
//UU %%<name> <value type>
//UU <defines>
//UU <rule> : <code>
//UU <rule> : <code>
//UU
//UU <prefix code> = C source code that will prefix the generated output
//UU
//UU <name> = Parser function name
//UU
//UU <value type> = is the C type used to declare value parameter below.
//UU
//UU 	<value type> does not accept space. "char*" is a valid type. "char *" is not.
//UU 	for complex types (structs or unios) best to use a previous typedefed type.
//UU
//UU <defines> = Option defines in the form
//UU
//UU 	%%define <letter>	<char list>
//UU
//UU 	where <char list> is a list of valid charactes or character ranges
//UU	defined characters can be used as special escapes in <rule>
//UU
//UU <rule> = Sequence of charactes, special characters, escape sequences or escaped defined characters
//UU
//UU <code> = Action to be performed in case of match (a single line to C code)
//UU
//UU 	code can use special local variable "input" to reffer to string matched
//UU
//UU Information About Generated code
//UU ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
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
//UU     \% - Matchs %
//UU     \. - Matchs .
//UU     \: - Matchs :
//UU     \- - Matchs -
//UU
//UU

static char *opt_header_name = NULL;
static int opt_verbose = 1;
static int opt_generate_header = 0;

#define	DEFINE_INVALID	-1
#define DEFINE_RANGE	0x7fff

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
	case 's': return ' ';
	case '\\': return '\\';
	case '%': return '%';
	case '.': return '.';
	case ':': return ':';
	case '-': return '-';
	}
	return -1;
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

int getNextDefChar(char *source, int *size)
{
char ch;

	if(*source==0) {
		*size = 0;
		return DEFINE_INVALID;
	}
	if(*source=='-') return DEFINE_RANGE;
	if(*source=='\\'){
		ch = escapeCode(*(source+1));
		if(ch>0) {
			*size = 2;
			return ch;
		}
		return DEFINE_INVALID;
	}
	*size = 1;
	return *source;
}

int getNextDefCharInLine(char *source, int *size)
{
	if(*source=='\n' || *source=='\r') {
		*size = 0;
		return DEFINE_INVALID;
	}
	return getNextDefChar(source, size);
}

char *processDefine(char *source, char *defines, int *defcount)
{
int	def, ch0, ch1, c, range0;
int	csize;

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
	while((ch0 = getNextDefCharInLine(source, &csize)) > 0) {

		if(ch0==DEFINE_INVALID) {
			fprintf(stderr,"Unexpected caracter in the definition of %c\n", def);
			exit(-1);
		}

		if(ch0==DEFINE_RANGE){
			source+=csize;
			ch1 = getNextDefCharInLine(source, &csize);
			if(ch1==DEFINE_INVALID || ch1==DEFINE_RANGE) {
				fprintf(stderr,"Unexpected character in the definition of %c after '-'\n");
				fprintf(stderr," Note: '-' is used for range definition\n", def);
				fprintf(stderr," Note: for literal '-' use instead  '\\-'\n", def);
				exit(-1);
			}
			if(range0>ch1){
				fprintf(stderr,"Invalid character range in define of %c\n", def);
				exit(-1);
			}
			printif(1,"if(ch>=%d && ch<=%d) return 1;\n", range0, ch1);
		}else{
			range0 = ch0;
			printif(1,"if(ch==%d) return 1;\n", ch0);
		}
		source+=csize;
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

char advance(char **source)
{
	return *((*source)++);
}

void printCurrentCode(FILE *fp, char *source)
{
int c;

	for(c=0; c<64 || *source==0; c++) fputc(*source++, fp);
}

int nrlex(char *source, FILE *fc, FILE *fh)
{
#define START 30
int state, code;
int label, abortlabel, recurlabel;
int c, st;
int defcount;
char ch,lexname[256],typename[256],defines[256];

	putLocalFunctions();

	// Consume white spaces
	ch = *source;
	while(ch==' ' || ch=='\t') ch=advance(&source);

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
				printCurrentCode(stderr,source);
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

	if(fh){
		fprintf(fh,"#ifndef _Nrlex_%s_h_\n", lexname);
		fprintf(fh,"#define _Nrlex_%s_h_\n", lexname);
		fprintf(fh,"\n");
		fprintf(fh,"\tint %s(char *buffer, int *cursor, %s *value, void *extra);\n", lexname, typename);
		fprintf(fh,"\n");
		fprintf(fh,"#endif\n");
	}

	return 0;
}


int main(int argc, char **argv)
{
int  size, argcount, err;
FILE *fc, *fh; 
char *buffer, *rules;

	for(argcount=1; argcount<argc; argcount++){

		if(!strcmp(argv[argcount],"-h")){
			printHexString(stdout,usagestr);
			exit(-1);
			continue;
		}

		if(!strcmp(argv[argcount],"-H")){
			argcount++;
			if(argcount>=argc){
				printHexString(stdout,usagestr);
				exit(-1);
			}
			opt_generate_header = 1;
			opt_header_name = strdup(argv[argcount]);
			argcount++;
			continue;
		}

		if(!strcmp(argv[argcount],"-v")){
			opt_verbose = 1;
			continue;
		} 

		if(argv[argcount][0]=='-'){
			printHexString(stdout,usagestr);
			fprintf(stderr,"Error: Unknow option: %s\n", argv[argcount]);
			exit(-1);
		} 

		break;
	}

	if(opt_generate_header){
		fc = stdout;
		fh = openOutputFile(opt_header_name, ".h", ".h");
		buffer = readToBuffer(0, 32768, 1, &size);
		buffer[size]=0;
		//printHexString(stdout,usagestr);
		//exit(-1);
	}else{
		fc = stdout;
		fh = NULL;
		buffer = readToBuffer(0, 32768, 1, &size);
		buffer[size]=0;
	}

	rules = copyPrecode(stdout, buffer);
	err = nrlex(rules, fc, fh);

	if(fh) fclose(fh);

	if(err) exit(-1);
}

