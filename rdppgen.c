#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utils.h"
#include "string.h"

#include "rdpplex.h"
#include "rdppgen.embedded.h"

//UU 
//UU rdpp - A very basic recursive descendent predictive parser generator for LL(1)
//UU 
//UU rdppgen [-v] <file>
//UU
//UU  -v	verbose
//UU  <file>	grammar input file
//UU 
//UU Grammar input File format:
//UU 
//UU <prefix code>
//UU %%<ParserName> <start nonterminal> <lex function>
//UU <node type definition>
//UU %%
//UU <rules>
//UU 
//UU Accepted syntax for grammar rules:
//UU 
//UU 	rule: nonterminal COLON rside SEMICOLON
//UU
//UU 	rside: item_list code rside_or
//UU 
//UU 	rside_or: VBAR rside | empty
//UU 
//UU 	item_list: item item_list | empty
//UU 
//UU 	item: NONTERMINAL | TERMINAL
//UU 
//UU 	code: CODEBETWEENCURLYBRACES | empty
//UU
//UU
//UU empty is not a token, it's just to made clear it's a rule that accepts empty input
//UU check libabjson for a practical example
//UU 

static int lcount = 0;
static int verbose = 0;

enum {LEFTHAND, RIGHTHAND};

char *tokenName(int token)
{
	return token<1000 ? "Invalid" : tokens[token - 1000];
}

int fprintc(FILE *fp, char *fmt, ...)
{
va_list va;

	va_start(va, fmt);
	fprintf(fp, "// ");
	vfprintf(fp, fmt, va);
	va_end(va);
}

int isValidNameChar(int ch)
{
	if(ch==' ' || ch=='\t' || ch=='_') return 1;
	if(ch>='0' && ch<='9') return 1;
	if(ch>='A' && ch<='Z') return 1;
	if(ch>='a' && ch<='z') return 1;
	return 0;
}

//
// Those commented code will be converted to strings into rdppgen.embedded.h by instructions on Makefile
//
// respectively char *emb1, *emb2
//

//E1 #ifndef _Rdpp$0_h_
//E1 #define _Rdpp$0_h_
//E1
//E1 typedef $3 $0Type;
//E1
//E1 typedef struct {
//E1 	char    *buffer;		// input buffer
//E1 	int	cursor;			// current cursos position in buffer for next token, passed to lex
//E1 	int	currToken;		// last Token ID returned by lex
//E1 	$0Type	currTokenValue; 	// last Token associated value returned by lex
//E1 	int	currTokenCursor;	// cursor of begining of current token
//E1 	$0Type	*value; 		// A stack of terminal/non-terminal associated values
//E1 	int	*valueType; 		// A stack of terminal/non-terminal value types
//E1 	int	valueAllocated;		// Allocated size of value stack
//E1 	int	valueSp;		// Stack pointer for value stack
//E1 	int	lcount;			// Convenient built-in line count 
//E1 	void	*extra;			
//E1 } *$0;
//E1
//E1 $0 $0New(char *buffer, void *extra);
//E1 void $0Free($0 self);
//E1 int $0Parse($0 self);
//E1
//E1 //void $0TypeDeallocator($0Type *node);
//E1
//E1 #endif

//E2
//E2 int static getNextToken($0 self)
//E2 {
//E2  	self->currTokenCursor = self->cursor;
//E2  	self->currToken = $2(self->buffer, &(self->cursor), &(self->currTokenValue),self->extra);
//E2  	return self->currToken;
//E2 }
//E2
//E2 static int stackValue($0 self, $0Type *value)
//E2 {
//E2 	memcpy(self->value + self->valueSp, value, sizeof($0Type));
//E2 	self->valueSp++;
//E2 }
//E2
//E2 static int accept($0 self, int token)
//E2 {
//E2 	if(self->currToken == token){
//E2 		stackValue(self, &(self->currTokenValue));
//E2 		getNextToken(self);
//E2 		return 1;
//E2 	}
//E2 	return 0; 
//E2 }
//E2
//E2 static int expect($0 self, int token)
//E2 {
//E2 	if(accept(self, token)) return 1;
//E2 	fprintf(stderr,"Expected: token: %d\n", token);
//E2  	return 0;
//E2 }
//E2 
//E2 static int backtrack($0 self, int cursor)
//E2 {
//E2 	if(self->currTokenCursor == restore_cursor) return 0;
//E2  	self->cursor = cursor;
//E2 	getNextToken(self);
//E2 	return 1;
//E2 }
//E2 
//E2 $0 $0New(char *buffer, void *extra)
//E2 {
//E2 $0 self;
//E2 
//E2 	if((self=malloc(sizeof(*($0)0)))==NULL) return NULL;
//E2
//E2 	self->buffer = buffer;
//E2 	self->cursor = 0;
//E2 	self->valueSp = 0;
//E2 	self->valueAllocated = 256;
//E2 	self->extra = extra;
//E2
//E2 	if((self->value = malloc(sizeof($0Type) * self->valueAllocated))==NULL){
//E2 		free(self);
//E2 		return NULL;
//E2 	}
//E2
//E2 	if((self->valueType = malloc(sizeof(int) * self->valueAllocated))==NULL){
//E2		free(self->value);
//E2 		free(self);
//E2 		return NULL;
//E2 	}
//E2
//E2 	return self;
//E2 }
//E2 
//E2 void $0Free($0 self)
//E2 {
//E2 	if(self==NULL) return;
//E2 	if(self->value) free(self->value);
//E2 	free(self);
//E2 }
//E2 
//E2 int $0Parse($0 self)
//E2 {
//E2 int ret;
//E2
//E2 	getNextToken(self);
//E2 	ret = parse_$1(self);
//E2
//E2 //	if(ret==0){
//E2 //		for(c=0; c<self->valueSp; c++){
//E2 //			$0TypeDeallocator(self->value+c);
//E2 //		}
//E2 //	}
//E2
//E2 	return ret;
//E2 }
//E2 


StBuffer generateParser(char *buffer, StList terminals, StList nonterminals, char *parserName)
{
StBuffer	maincode;
char		*value;
int		size, cursor, token;
int		side, state, c;

	maincode = newStBuffer(2048);
	if(terminals==NULL){
		fprintf(stderr,"Could not alloc memory\n");
		exit(-1);
	}

	cursor = 0;
	lcount = 0;

	state = 0;

	do{
		token=rdpplex(buffer,&cursor,&value,(void *)&lcount);
		if(verbose)
			fprintf(stderr,"Line %d: Token %d (%s): Value >>%s<<\n",
					lcount, token, tokenName(token), value);

		switch(state){
		case 0:  // Start of the rule expects a nonterminal
			if(token==-1){
				break;
			}else if(token==NONTERMINAL){
				if(verbose) fprintf(stderr,"Line %d: New nonterminal >>%s<<\n", lcount, value);
				stBufferAppend(maincode, "\n");
				stBufferAppendf(maincode, "//New nonterminal %s\n", value);
				stBufferAppendf(maincode, "int static parse_%s(%s self)\n{\n", value, parserName);
				stBufferAppendf(maincode, "\tDSTART;\n", value);
				stBufferAppendf(maincode, "\t%sType	result, *terms;\n", parserName);
				stBufferAppendf(maincode, "\tint	rdpp_bp = self->valueSp;\n", value);
				stBufferAppendf(maincode, "\tterms = self->value + rdpp_bp;\n", value);
				stBufferAppendf(maincode, "\tmemset(&result, 0, sizeof(%sType));\n", parserName);
				stListRegister(nonterminals, (char *)value);
				state = 10;
			}else{
				fprintf(stderr,"Error: line %d: Expected NONTERMINAL\n", lcount);
				exit(-1);
			}
			break;

		case 10: // Expects Colon
			if(token==COLON){
				state = 20;
				break;
			}else{
				fprintf(stderr,"Error: line %d: Expected COLON\n", lcount);
				exit(-1);
			}
		case 20: // first rule term 
			if(token==CODEBETWEENCURLYBRACES){			// If code, just insert the code and go on
				stBufferAppendf(maincode, "\t{\n");
				stBufferAppendf(maincode, "%s\n", value);
				stBufferAppendf(maincode, "\t}\n");
			}else if(token==SEMICOLON){ 				// If SEMICOLON starts a rule, the ruke is empty
				stBufferAppend(maincode, "{\n");
				stBufferAppend(maincode, "\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppend(maincode, "\t\tstackValue(self, &result);\n");
				stBufferAppend(maincode, "\t\tDRETURN(1);\n");
				stBufferAppend(maincode, "\t}\n");
				//stBufferAppend(maincode, "\tself->valueSp = rdpp_bp;\n");
				//stBufferAppend(maincode, "\tDRETURN(0);\n");
				stBufferAppend(maincode, "}\n");
				state = 0;
			}else if(token==NONTERMINAL){				// if NONTERMINAL go parse that terminal
				stBufferAppendf(maincode, "\tif(parse_%s(self)){\n", value);
				state = 30;
			}else if(token==TERMINAL){				// if TERMINAL just call accept
				stListRegister(terminals, (char *)value);
				stBufferAppendf(maincode, "\tif(accept(self, %s)){\n", value);
				state = 30;
			}
			break;

		case 30: // extra rule terms
			if(token==CODEBETWEENCURLYBRACES){
				stBufferAppendf(maincode, "\t\t{\n");
				stBufferAppendf(maincode, "%s\n", value);
				stBufferAppendf(maincode, "\t\t}\n");
			}else if(token==SEMICOLON){
				stBufferAppend(maincode, "\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppend(maincode, "\t\tstackValue(self, &result);\n");
				stBufferAppend(maincode, "\t\tDRETURN(1);\n");
				stBufferAppend(maincode, "\t}\n");
				stBufferAppend(maincode, "\tself->valueSp = rdpp_bp;\n");
				stBufferAppend(maincode, "\tDRETURN(0);\n");
				stBufferAppend(maincode, "}\n");
				state = 0;
			}else if(token==VBAR){
				stBufferAppend(maincode, "\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppend(maincode, "\t\tstackValue(self, &result);\n");
				stBufferAppend(maincode, "\t\tDRETURN(1);\n");
				stBufferAppend(maincode, "\t}\n");
				state = 20;
			}else if(token==NONTERMINAL){
				stBufferAppendf(maincode, "\t\tif(!parse_%s(self)) {\n", value);
				stBufferAppendf(maincode, "\t\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppendf(maincode, "\t\t\t DRETURN(0);\n", value);
				stBufferAppendf(maincode, "\t\t}\n", value);
			}else if(token==TERMINAL){
				stListRegister(terminals, (char *)value);
				stBufferAppendf(maincode, "\t\tif(!expect(self, %s)) {\n", value);
				stBufferAppendf(maincode, "\t\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppendf(maincode, "\t\t\t DRETURN(0);\n", value);
				stBufferAppendf(maincode, "\t\t}\n", value);
			}
			break;
		}

	}while(token>=0);

	if(*(buffer+cursor)){
		fprintf(stderr,"Error line %d: %20.20s\n", lcount, buffer+cursor);
	};

	return maincode;
}

int processFile(char *filename)
{
FILE	*fc, *fh, *ft;
StList	terminals, nonterminals;
StBuffer maincode;
char	*buffer, *rules, keys[10][1024];
int	size, c, fdin, argcount;

	// Open Files and read source
	fc = openOutputFile(filename, ".rdpp", ".c");
	fh = openOutputFile(filename, ".rdpp", ".h");
	ft = openOutputFile(filename, ".rdpp", ".tokens.h");

	if(fc==NULL || fh==NULL || ft==NULL) exit(-1);

	fdin = open(filename, O_RDONLY);
	if(fdin<0){
		fprintf(stderr,"Could not open input file: %s\n", filename);
		exit(-1);
	}

	buffer = readToBuffer(fdin, 32768, 1, &size);
	buffer[size]=0;

	// Starts lists for terminals and nonterminals
	nonterminals = newStList(16);
	if(nonterminals==NULL){
		fprintf(stderr,"Could not alloc memory\n");
		exit(-1);
	}

	terminals = newStList(16);
	if(terminals==NULL){
		fprintf(stderr,"Could not alloc memory\n");
		exit(-1);
	}

	// Creates <parsername>.c and <parsername>.h files

	// Copy code prefixed in source file up to %% symbol
	rules = copyPrecode(fc, buffer);

	// get keys
	// $0 - parser name, $1 - first symbol, $2 - lex name, $3 - Node Type
	for(c=0;c<10;c++) keys[c][0]=0;

	while(*rules==' ' || *rules =='\t') rules++;
	for(c=0; isValidNameChar(*rules) && c<1023; c++, rules++){
		if(*rules==' ' || *rules=='\t') break;
		keys[0][c] = *rules;
	}
	keys[0][c]=0;

	while(*rules==' ' || *rules =='\t') rules++;
	for(c=0; isValidNameChar(*rules) && c<1023; c++, rules++){
		if(*rules==' ' || *rules=='\t') break;
		keys[1][c] = *rules;
	}
	keys[1][c]=0;

	while(*rules==' ' || *rules =='\t') rules++;
	for(c=0; isValidNameChar(*rules) && c<1023; c++, rules++){
		if(*rules==' ' || *rules=='\t') break;
		keys[2][c] = *rules;
	}
	keys[2][c]=0;

	while(*rules==' ' || *rules =='\t') rules++;
	for(c=0; *rules!=0 && c<1023; c++, rules++){
		if(*rules=='%' && *(rules+1)=='%') {
			rules+=2;
			break;
		}
		keys[3][c] = *rules;
	}
	keys[3][c]=0;


	while(*rules==' ' || *rules=='\t' || *rules=='\n' || *rules=='\r') rules++;

	while(*rules=='%'){
		if(strcmp(rules,"%%destructor")){
			rules+=12;
			printf("destructor defined\n");
			//rules = processDefine(rules, defines, &defcount);
			while(*rules==' ' || *rules=='\t' || *rules=='\n' || *rules=='\r') rules++;
		}
	}	

	// Copy embed into .h file code from E1 comments
	printHexStringReplace(fh, emb1, keys);

	// Include content of ddebug.h file in .c file
	printHexString(fc, ddebug);

	// Creates the parser itself
	maincode = generateParser(rules, terminals, nonterminals, keys[0]);

	// Write out declarations for static internal parser functions of each nonterminal
	for(c=0; c<nonterminals->size; c++){
		if(nonterminals->list[c] == NULL) break;
		fprintf(fc, "int static parse_%s(%s self);\n", nonterminals->list[c], keys[0]);
	}
	fprintf(fc,"\n");

	// Embedd code commented as E2
	printHexStringReplace(fc, emb2, keys);

	// Write out parser code
	fprintf(fc,"\n\n%s", maincode->buffer);


	// Creating <parsername>.token.h files

	fprintf(ft,"#ifndef _Rdpp%s_Tokens_h_\n", keys[0]);
	fprintf(ft,"#define _Rdpp%s_Tokens_h_\n", keys[0]);
	fprintf(ft,"\n");

	fprintf(ft, "enum Rdpp%sTokens {", keys[0]);
	for(c=0; c<terminals->size; c++){
		if(terminals->list[c] == NULL) break;
		if(c>0) fprintf(ft, ", ");
		if(c==0) 
			fprintf(ft, "%s=1000", terminals->list[c]);
		else
			fprintf(ft, "%s", terminals->list[c]);
	}
	fprintf(ft, "};\n");
	fprintf(ft,"\n");

	stListToupper(nonterminals);

	fprintf(ft, "enum Rdpp%sNonterminals {", keys[0]);
	for(c=0; c<nonterminals->size; c++){
		if(nonterminals->list[c] == NULL) break;
		if(c>0) fprintf(ft, ", ");
		if(c==0) 
			fprintf(ft, "NT_%s=%d", nonterminals->list[c], 2000 + ((terminals->size)/1000)*1000);
		else
			fprintf(ft, "NT_%s", nonterminals->list[c]);
	}
	fprintf(ft, "};\n");
	fprintf(ft,"#endif\n");

	// Closing

	fclose(fc);
	fclose(fh);
	fclose(ft);
}

int main(int argc, char **argv)
{
int	argcount;

	if(argc<2){
		printHexString(stdout, usagestr);
		exit(-1);
	}
	argcount=1;

	while(argcount<argc){
		if(!strcmp(argv[argcount],"-v")){
			argcount++;
			verbose = 1;
			continue;
		}
		processFile(argv[argcount++]);
	}
}

