#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Please enter a buffer size from 1-10 as a cmdline argument\n");
		return -1;
	}

	int buffer_size = atoi(argv[1]);

	if (buffer_size < 1 || buffer_size > 10) {
		printf("Error: Please enter a buffer size from 1-10 as a cmdline argument\n");
		return -1;
	}

	return 0;
}
