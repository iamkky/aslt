#ifndef _NRLEX_UTILS_H_
#define _NRLEX_UTILS_H_

int	unhex(char ch);
int	printHexString(FILE *fp, char *str);
int	printHexStringReplace(FILE *fp, char *str, char *value[]);
int	isSpace(int ch);
char	*copyPrecode(FILE *fp, char *source, int *lcount);
char	*readToBuffer(int fd, int inisize, int extrasize, int *readsize);
FILE	*openOutputFile(char *source, char *sext, char *dext);
char	*getCode(char *input, int *cursor, int *lcount);

#endif
