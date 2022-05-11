#include <stdlib.h>
#include <unistd.h>
#include "test_utils.h"


int main(int argc, char **argv)
{
char *buffer;
int  size;

	buffer = readToBuffer(0, 16, 1, &size);
	buffer[size]=0;

	write(1, buffer, size);
}

