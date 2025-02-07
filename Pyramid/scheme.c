// Name: Rajwol Chapagain
// Class: CSC460
// Date: Feb 7, 2025
// Assignment: Pyramid Scheme: Generate a pyramid-like tree structure by forking processes based on commandline argument

#include <stdio.h>

int main(int argc, char * argv[]) {
	int n = atoi(argv[1]);

	if (n < 0) {
		printf("%s\n", "Error: N needs to be greater than or equal to 0");
		return 1;
	}

	if (n > 5) {
		printf("%s\n", "Error: N needs to be smaller than or equal to 5");
		return 1;
	}

	printf("%-10s%-10s%-10s\n", "Gen", "PID", "PPID");
	printf("%-10d%-10d%-10d\n", 0, getpid(), getppid());

	int i = 1;
	for (; i <= n; i++) {
		int j = 0;

		int a = -1;
		for (; j < i; j++) {
			a = fork();
			if (a == 0) {
				printf("%-10d%-10d%-10d\n", i, getpid(), getppid());
				break;
			} 
		}

		if (a != 0) break;
	}

	sleep(1);
	return 0;
}
