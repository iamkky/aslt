#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

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
//UU 	rside: item_list rside_or
//UU 
//UU 	rside_or: VBAR rside rside_or| empty
//UU 
//UU 	item_list: item item_list | empty
//UU 
//UU 	item: NONTERMINAL | TERMINAL | OPT_OPEN iten_list OPT_CLOSE | code
//UU 
//UU 	code: CODEBETWEENCURLYBRACES | empty
//UU
//UU
//UU empty is not a token, it's just to made clear it's a rule that accepts empty input
//UU check libabjson for a practical example
//UU 
//

#define RDPP_NONTERMINAL_START	10000

static int lcount = 0;
static int optVerbose = 0;
static int optNames = 0;

char *tokenName(int token)
{
	return token<1000 ? "Invalid" : tokens[token - 1000];
}

int isValidNameChar(int ch)
{
	if(ch=='_') return 1;
	if(ch>='0' && ch<='9') return 1;
	if(ch>='A' && ch<='Z') return 1;
	if(ch>='a' && ch<='z') return 1;
	return 0;
}

char *getValidName(char *p, int *size)
{
char tmp[1024], c;

	for(c=0; isValidNameChar(*p) && c<1023; c++){
		tmp[c] = *p++;
	}
	tmp[c] = 0;
	*size = c;
	return strdup(tmp);
}

//
// These commented lines of code will be converted to strings into rdppgen.embedded.h by instructions on Makefile.
// Respectively as char *emb1, *emb2, both encoded as hex strings.
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
//E1 #ifndef RDPP_NONTERMINAL_START
//E1 #define RDPP_NONTERMINAL_START		10000
//E1 #endif
//E1
//E1 $0 $0New(char *buffer, void *extra);
//E1 void $0Free($0 self);
//E1 int $0Parse($0 self);
//E1
//E1 #endif

//E2
//E2 int static printSnipet(FILE *fp, char *buffer, int start, int end)
//E2 {
//E2 	while(start<end) {
//E2 		switch(buffer[start]){
//E2 		case '\n':	fprintf(fp, "\\n"); break;
//E2 		case '\r':	fprintf(fp, "\\r"); break;
//E2 		case '\\':	fprintf(fp, "\\\\"); break;
//E2 		case '\t':	fprintf(fp, "\\t"); break;
//E2 		default:	fputc(buffer[start], fp);
//E2 		}
//E2 		start++;
//E2 	}
//E2 }
//E2
//E2 int static getNextToken($0 self)
//E2 {
//E2 int c;
//E2
//E2  	self->currTokenCursor = self->cursor;
//E2  	self->currToken = $2(self->buffer, &(self->cursor), &(self->currTokenValue), self->extra);
//E2
//E2 #ifdef TOKENDEBUG
//E2 	if(self->cursor>self->currTokenCursor){
//E2 		fprintf(stderr,"Token:%d, cursor:%d, scanned:%d, snipet:[[", self->currToken, self->currTokenCursor, self->cursor - self->currTokenCursor);
//E2 		printSnipet(stderr, self->buffer, self->currTokenCursor, self->cursor);
//E2 		fprintf(stderr,"]]\n");
//E2 	}else{
//E2 		fprintf(stderr,"Token: %d\n", self->currToken);
//E2 	}
//E2 #endif
//E2
//E2  	return self->currToken;
//E2 }
//E2
//E2 static int stackValue($0 self, int type, $0Type *value)
//E2 {
//E2 //	if(self->valueSp >= selfAllocated){
//E2 //		resize missing
//E2 //	}
//E2 	self->valueType[self->valueSp] = type;
//E2 	memcpy(self->value + self->valueSp, value, sizeof($0Type));
//E2 	self->valueSp++;
//E2 }
//E2
//E2 static int accept($0 self, int token)
//E2 {
//E2 	if(self->currToken == token){
//E2 #ifdef DDEBUG
//E2 		DPREFIX();
//E2 		fprintf(stderr,"Accepted %d [[", token);
//E2 		printSnipet(stderr, self->buffer, self->currTokenCursor, self->cursor);
//E2 		fprintf(stderr,"]]\n");
//E2 #endif
//E2 		stackValue(self, self->currToken, &(self->currTokenValue));
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
//E2 //Backtracking is not implemented, may be in the future.
//E2 static int backtrack($0 self, int restore_cursor)
//E2 {
//E2 	if(self->currTokenCursor == restore_cursor) return 0;
//E2  	self->cursor = restore_cursor;
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
//E2 	if(self->valueType) free(self->valueType);
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
//E2 void static $0TypeDestructor($0Type *self)
//E2 {
//E2 	$4
//E2 }
//E2
//E2 #ifdef DDEBUG
//E2 #ifdef RETURNDEBUG
//E2
//E2 int static returndebug_int(int value)
//E2 {
//E2 	DPREFIX();
//E2 	fprintf(stderr,"Return %d\n", value);
//E2 }
//E2
//E2 #else
//E2 #define returndebug_int(value) do{}while(0)
//E2 #endif
//E2 #else
//E2 #define returndebug_int(value) do{}while(0)
//E2 #endif
//E2 

void printCurrentCode(FILE *fp, char *source)
{
int c;

	for(c=0; c<64 || *source==0; c++) fputc(*source++, fp);
}

void strToUpper(char *str)
{
	while(*str){
		*str = toupper(*str);
		str++;
	}
}

void insertCodeBlock(StBuffer maincode, int start, int end, char *codeblock)
{
	if(codeblock){
		stBufferAppendf(maincode, "{\n");
		stBufferAppendf(maincode, "start = %d;\n", start);
		stBufferAppendf(maincode, "end = %d;\n", end);
		stBufferAppendf(maincode, "%s", codeblock);
		stBufferAppendf(maincode, "\n");
		stBufferAppendf(maincode, "}\n");
	}
}

StBuffer processRules(char *buffer, StList terminals, StList nonterminals, char *parserName, char *default_action)
{
StBuffer	maincode;
char		*value, *nonterminal_define_name;
int		size, cursor, token, last_scanned_cursor, term_count, term_start;
int		side, state, c, opt_level = 0, opt_first = 0;;

	maincode = stBufferNew(2048);

	cursor = 0;
	lcount = 0;
	state = 0;

	do{
		last_scanned_cursor = cursor;
		token=rdpplex(buffer,&cursor,&value,(void *)&lcount);
		if(optVerbose)
			fprintf(stderr,"State: %d Line %d: Token %d (%s): Value >>%s<<\n",
					state, lcount, token, tokenName(token), value);

		switch(state){
		case 0:  // Start of the rule expects a nonterminal
			if(token==-1){
				break;
			}else if(token==NONTERMINAL){
				if(optVerbose) fprintf(stderr,"Line %d: New nonterminal >>%s<<\n", lcount, value);
				opt_level = 0;
				opt_first = 0;
				term_count = 0;
				term_start = 0;
				stListRegister(nonterminals, value);
				nonterminal_define_name = strdup(value);
				strToUpper(nonterminal_define_name);
				stBufferAppendf(maincode, "\n");
				stBufferAppendf(maincode, "// New nonterminal %s\n", value);
				stBufferAppendf(maincode, "int static parse_%s(%s self)\n", value, parserName);
				stBufferAppendf(maincode, "{\n");
				stBufferAppendf(maincode, "%sType result, *terms;\n", parserName);
				stBufferAppendf(maincode, "int *types, start, end, rdpp_bp, rdpp_nt_id;\n");
				stBufferAppendf(maincode, "\n");
				stBufferAppendf(maincode, "\tDSTART;\n");
				stBufferAppendf(maincode, "\trdpp_nt_id = NT_%s;\n", nonterminal_define_name);
				stBufferAppendf(maincode, "\tterms = self->value + self->valueSp;\n");
				stBufferAppendf(maincode, "\ttypes = self->valueType + self->valueSp;\n");
				stBufferAppendf(maincode, "\tmemset(&result, 0, sizeof(%sType));\n", parserName);
				stBufferAppendf(maincode, "\n");
				state = 10;
			}else{
				fprintf(stderr,"Error: line %d: Expected a nonterminal\n", lcount);
				printCurrentCode(stderr,buffer+last_scanned_cursor);
				exit(-1);
			}
			break;
		case 10: // Expects Colon
			if(token==COLON){
				state = 20;
				break;
			}else{
				fprintf(stderr,"Error: line %d: Expected ':'\n", lcount);
				printCurrentCode(stderr,buffer+last_scanned_cursor);
				exit(-1);
			}
		case 20: // first rule term 
			if(token==CODEBETWEENCURLYBRACES){			// If code, just insert the code and go on
				insertCodeBlock(maincode, term_start, term_count, value);
			}else if(token==NONTERMINAL){				// if NONTERMINAL go parse that terminal
				term_count++;
				stBufferAppendf(maincode, "\trdpp_bp = self->valueSp;\n");
				stBufferAppendf(maincode, "\twhile(1){\n");
				stBufferAppendf(maincode, "\t\tif(!parse_%s(self)) break;\n", value);
				state = 30;
			}else if(token==TERMINAL){				// if TERMINAL just call accept
				term_count++;
				stListRegister(terminals, (char *)value);
				stBufferAppendf(maincode, "\trdpp_bp = self->valueSp;\n");
				stBufferAppendf(maincode, "\twhile(1){\n");
				stBufferAppendf(maincode, "\t\tif(!accept(self, %s)) break;\n", value);
				state = 30;
			}else if(token==SEMICOLON){ 				// If SEMICOLON starts a rule, the ruke is empty
										// empty rule is expected to be last rule
				insertCodeBlock(maincode, term_start, term_count, default_action);
				stBufferAppendf(maincode, "\tself->valueSp = rdpp_bp;\n");
				stBufferAppendf(maincode, "\t\tstackValue(self, NT_%s, &result);\n", nonterminal_define_name);
				stBufferAppendf(maincode, "\treturndebug_int(1);\n");
				stBufferAppendf(maincode, "\tDRETURN(1);\n");
				stBufferAppendf(maincode, "}\n");
				state = 0;
			}else{
				fprintf(stderr,"Error: line %d: Unexpected token %d\n", lcount, token);
				printCurrentCode(stderr,buffer+last_scanned_cursor);
				exit(-1);
			}
			break;

		case 30: // extra rule terms
			if(token==CODEBETWEENCURLYBRACES){
				insertCodeBlock(maincode, term_start, term_count, value);
			}else if(token==NONTERMINAL){
				term_count++;
				if(opt_level<=opt_first){
					stBufferAppendf(maincode, "\t\tif(!parse_%s(self)) {\n", value);
					stBufferAppendf(maincode, "\t\t\tself->valueSp = rdpp_bp;\n");
					stBufferAppendf(maincode, "\t\t\treturndebug_int(0);\n");
					stBufferAppendf(maincode, "\t\t\tDRETURN(0);\n");
					stBufferAppendf(maincode, "\t\t}\n");
				}else{
					stBufferAppendf(maincode, "\t\twhile(parse_%s(self)){\n", value);
					opt_first++;
				}
			}else if(token==TERMINAL){
				term_count++;
				stListRegister(terminals, (char *)value);
				if(opt_level<=opt_first){
					stBufferAppendf(maincode, "\t\tif(!expect(self, %s)) {\n", value);
					stBufferAppendf(maincode, "\t\t\tself->valueSp = rdpp_bp;\n");
					stBufferAppendf(maincode, "\t\t\treturndebug_int(0);\n");
					stBufferAppendf(maincode, "\t\t\tDRETURN(0);\n");
					stBufferAppendf(maincode, "\t\t}\n");
				}else{
					stBufferAppendf(maincode, "\t\twhile(accept(self, %s)){\n", value);
					opt_first++;
				}
			}else if(token==VBAR){
				insertCodeBlock(maincode, term_start, term_count, default_action);
				stBufferAppendf(maincode, "\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppendf(maincode, "\t\tstackValue(self, NT_%s, &result);\n", nonterminal_define_name);
				stBufferAppend(maincode, "\t\treturndebug_int(1);\n");
				stBufferAppend(maincode, "\t\tDRETURN(1);\n");
				stBufferAppend(maincode, "\t}\n");
				term_count = 0;
				term_start = 0;
				state = 20;
			}else if(token==SEMICOLON){
				insertCodeBlock(maincode, term_start, term_count, default_action);
				stBufferAppend(maincode, "\t\tself->valueSp = rdpp_bp;\n");
				stBufferAppendf(maincode, "\t\tstackValue(self, NT_%s, &result);\n", nonterminal_define_name);
				stBufferAppend(maincode, "\t\treturndebug_int(1);\n");
				stBufferAppend(maincode, "\t\tDRETURN(1);\n");
				stBufferAppend(maincode, "\t}\n");
				stBufferAppend(maincode, "\tself->valueSp = rdpp_bp;\n");
				stBufferAppend(maincode, "\treturndebug_int(0);\n");
				stBufferAppend(maincode, "\tDRETURN(0);\n");
				stBufferAppend(maincode, "}\n");
				if(opt_level>0){
					fprintf(stderr,"Error: line %d: Missing ']'\n", lcount);
					printCurrentCode(stderr,buffer+last_scanned_cursor);
					exit(-1);
				}
				state = 0;
			}else if(token==OPT_OPEN){
				if(opt_level>0){
					fprintf(stderr,"Error: line %d: Unexpected token '[', only one level of recurrency is accepted\n", lcount);
					printCurrentCode(stderr,buffer+last_scanned_cursor);
					exit(-1);
				}
				insertCodeBlock(maincode, term_start, term_count, default_action);
				term_start = term_count;
				opt_level++;
			}else if(token==OPT_CLOSE){
				if(opt_level==0){
					fprintf(stderr,"Error: line %d: Unexpected token ']', (without a '[')\n", lcount);
					printCurrentCode(stderr,buffer+last_scanned_cursor);
					exit(-1);
				}
				insertCodeBlock(maincode, term_start, term_count, default_action);
				stBufferAppendf(maincode, "\t\tself->valueSp = rdpp_bp + %d;\n", term_start);
				stBufferAppendf(maincode, "\t\t}\n");
				term_start = term_count;
				opt_level--;
				opt_first--;
			}else {
				fprintf(stderr,"Error: line %d: Unexpected token %d\n", lcount, token);
				printCurrentCode(stderr,buffer+last_scanned_cursor);
				exit(-1);
			}
			break;
		}

	}while(token>=0);

	if(*(buffer+cursor)){
		fprintf(stderr,"Error line %d: %20.20s\n", lcount, buffer+cursor);
		return NULL;
	};

	return maincode;
}

char *skipWhiteChars(char *p)
{
	while(*p==' ' || *p=='\t' || *p=='\n' || *p=='\r') p++;
	return p;
}

char *processParamenters(char *source, char *keys[])
{
char *p;
int c, size;

	for(c=0;c<10;c++) keys[c]=NULL;

	source = skipWhiteChars(source);
	keys[0] = getValidName(source, &size);
	source += size;

	source = skipWhiteChars(source);
	keys[1] = getValidName(source, &size);
	source += size;

	source = skipWhiteChars(source);
	keys[2] = getValidName(source, &size);
	source += size;

	source = skipWhiteChars(source);
	for(c=0, p=source; *source!=0; c++, source++){
		if(*source=='%' && *(source+1)=='%') {
			source+=2;
			break;
		}
	}

	if((keys[3] = malloc(sizeof(char) * c + 1))==NULL) return NULL;
	strncpy(keys[3], p, c);
	keys[3][c] = 0;
		
	source = skipWhiteChars(source);
	while(*source=='%'){
		if(!strncmp(source,"%%action",8)){
			source = skipWhiteChars(source + 8);
			size = 0;
			if((keys[5] = getCode(source, &size, NULL)) == NULL){
				fprintf(stderr,"Expected default action code after %%%%action directive\n");
				return NULL;
			}
			source += size + 1;
		}else if(!strncmp(source,"%%destructor",12)){
			source = skipWhiteChars(source + 12);
			size = 0;
			if((keys[4] = getCode(source, &size, NULL)) == NULL){
				fprintf(stderr,"Expected destructor code after %%%%destructor directive\n");
				return NULL;
			}
			source += size + 1;
		}else if(!strncmp(source,"%%names",7)){
			optNames = 1;
			source += 7;
		}else{
			fprintf(stderr,"Unknow directive: %5.5s...\n", source);
			return NULL;
		}
		source = skipWhiteChars(source);
	}	

	if(optVerbose){
		fprintf(stderr,"Parser Name: %s\n", keys[0]);
		fprintf(stderr,"Start nonterminal: %s\n", keys[1]);
		fprintf(stderr,"Lex function: %s\n", keys[2]);
		fprintf(stderr,"ParserType: %s\n", keys[3]);
		fprintf(stderr,"Destructor: %s\n", keys[4]);
		fprintf(stderr,"Action: %s\n", keys[5]);
		fprintf(stderr,"Option Names: %d\n", optNames);
		fprintf(stderr,"\n");
	}

	return source;
}


int processFile(char *filename)
{
StBuffer	maincode;
StList		terminals, nonterminals;
FILE		*fc, *fh, *ft;
char		*buffer, *source, *keys[10];
int		size, c, fdin, argcount;

	// Read the source
	if((fdin = open(filename, O_RDONLY))<0){
		fprintf(stderr,"Could not open input file: %s\n", filename);
		return -1;
	}
	if((buffer = readToBuffer(fdin, 32768, 1, &size))==NULL){
		fprintf(stderr,"Could not alloc memory for source buffer\n");
		return -1;
	};
	buffer[size]=0;
	close(fdin);
	
	// Open output Files
	if((fc = openOutputFile(filename, ".rdpp", ".c"))==NULL) return -1;
	if((fh = openOutputFile(filename, ".rdpp", ".h"))==NULL) return -1;
	if((ft = openOutputFile(filename, ".rdpp", ".tokens.h"))==NULL) return -1;

	// Starts lists for terminals and nonterminals
	if((nonterminals = stListNew(16))==NULL){
		fprintf(stderr,"Could not alloc memory\n");
		return -1;
	}

	if((terminals = stListNew(16))==NULL){
		fprintf(stderr,"Could not alloc memory\n");
		return -1;
	}

	////// Process source and creates .c file 
	// Copy code prefixed in source file up to %% symbol
	source = copyPrecode(fc, buffer);

	// Get parameters from source into keys array
	// $0 - parser name, $1 - first symbol, $2 - lex name, $3 - Node Type, $4 - Destructor
	if((source = processParamenters(source, keys))==NULL) return -1;
	
	// Embed content of ddebug.h file into result .c file
	printHexString(fc, ddebug);

	// main code for the parser
	if((maincode = processRules(source, terminals, nonterminals, keys[0], keys[5]))==NULL) return -1;

	// Write out declarations for static internal parser functions of each nonterminal
	for(c=0; c<nonterminals->size; c++){
		if(nonterminals->list[c] == NULL) break;
		fprintf(fc, "int static parse_%s(%s self);\n", nonterminals->list[c], keys[0]);
	}
	fprintf(fc,"\n");

	if(optNames){
		fprintf(fc, "char *Rdpp_%sNonterminals_Names[] = {\n", keys[0]);
		for(c=0; c<nonterminals->size; c++){
			if(nonterminals->list[c] == NULL) break;
			if(c>0) fprintf(fc, ",\n");
			fprintf(fc, "\t\t\"%s\"", nonterminals->list[c]);
		}
		fprintf(fc, "\n\t};\n\n");

		fprintf(fc, "char *Rdpp_%sTerminals_Names[] = {\n", keys[0]);
		for(c=0; c<terminals->size; c++){
			if(terminals->list[c] == NULL) break;
			if(c>0) fprintf(fc, ",\n");
			fprintf(fc, "\t\t\"%s\"", terminals->list[c]);
		}
		fprintf(fc, "\n\t};\n\n");
	}

	// Embed code commented as E2
	printHexStringReplace(fc, emb2, keys);

	// Write out parser code
	fprintf(fc,"\n\n%s\n\n//EOF\n", maincode->buffer);

	////// Creates .h files
	// Embed code commented as E1 into header file
	printHexStringReplace(fh, emb1, keys);

	////// Creates .token.h files
	fprintf(ft,"#ifndef _Rdpp%s_Tokens_h_\n", keys[0]);
	fprintf(ft,"#define _Rdpp%s_Tokens_h_\n", keys[0]);
	fprintf(ft,"\n");

	if(optNames){
		fprintf(ft, "extern char *Rdpp_%sNonterminals_Names[];\n", keys[0]);
		fprintf(ft, "extern char *Rdpp_%sTerminals_Names[];\n", keys[0]);
		fprintf(ft,"\n");
	}

	fprintf(ft, "enum Rdpp_%sTokens {", keys[0]);
	for(c=0; c<terminals->size; c++){
		if(terminals->list[c] == NULL) break;
		if(c>0) fprintf(ft, ", ");
		if(c==0) 
			fprintf(ft, "%s=1000", terminals->list[c]);
		else
			fprintf(ft, "%s", terminals->list[c]);
	}
	fprintf(ft, "};\n\n");

	stListToupper(nonterminals);

	fprintf(ft, "enum Rdpp_%sNonterminals {", keys[0]);
	for(c=0; c<nonterminals->size; c++){
		if(nonterminals->list[c] == NULL) break;
		if(c>0) fprintf(ft, ", ");
		if(c==0) 
			fprintf(ft, "NT_%s=%d", nonterminals->list[c], RDPP_NONTERMINAL_START);
		else
			fprintf(ft, "NT_%s", nonterminals->list[c]);
	}
	fprintf(ft, "};\n\n");
	fprintf(ft,"\n#endif\n");

	// Closing all output files
	fclose(fc);
	fclose(fh);
	fclose(ft);

	return 0;
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
			optVerbose = 1;
			continue;
		}
		if(processFile(argv[argcount++])){
			fprintf(stderr,"Process of %s file has failed!\n", argv[argcount]);
			exit(-1);
		}
	}
}

