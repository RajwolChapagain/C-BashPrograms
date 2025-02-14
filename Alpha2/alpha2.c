// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 02/14/25
// Assignment: Alpha Squared - A program that spawns n processes including itself when n is a cmd line argument

#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("%s\n", "Please include a number between 1 and 26 (inclusive) as a commandline argument");
		return 0;
	}

	int n = atoi(argv[1]);

	if (n < 1 || n > 26) {
		printf("%s\n", "Please include a number between 1 and 26 (inclusive) as a commandline argument");
		return 0;
	}

	int i = 0;
	for (; i < n; i++) {
		int j = 0;
		for (; j < n; j++) {
			printf("%c: %d\n", 'A' + i, getpid());
		}

		if (fork() != 0) {
			break;
		}
	}

	return 0;
}
