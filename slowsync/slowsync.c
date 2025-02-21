// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 02/21/25
// Assignment: Slow Sync - A program that spawns n processes including itself when n is a cmd line argument and
// 							prints out n lines per process syncing them via a file

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

	char c = 'A';

	FILE *fp;
	fp = fopen("syncfile", "w");
	fprintf(fp,"%c",c);
	fclose(fp);

	int i = 0;
	for (; i < n; i++) {
		c = 'A' + i;
		if (i == n - 1) break;
		if (fork() != 0) {
			break;
		}
	}

	int j = 0;
	char turn;
	for (; j < n; j++) {
		do {
			fp = fopen("syncfile", "r");
			fscanf(fp,"%c",&turn);
			fclose(fp);
		} while (turn != c);

		printf("%c:%d\n", c, getpid());
		char new_turn = ++turn;
		new_turn -= 'A';
		new_turn %= n;
		new_turn += 'A';
		fp = fopen("syncfile", "w");
		fprintf(fp,"%c",new_turn);
		fclose(fp);
		
	}

	return 0;
}
