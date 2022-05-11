#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

// Allocs a buffer of <inisize> bytes and reads from fd to the end of file
// doubles alloced buffer every time read overflow
// allocs extrasize unused bytes at the end of buffer
// (usefull to provides space for a null terminator)

char *readToBuffer(int fd, int inisize, int extrasize, int *size);

#endif

