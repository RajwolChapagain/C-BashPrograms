// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 02/24/25
// Assignment: Faster Sync - A program that spawns n processes including itself when n is a cmd line argument and
// 							prints out n lines per process syncing them via shared memory

#include <stdio.h>
#include <sys/shm.h>

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

	int shmid = shmget(IPC_PRIVATE, sizeof(char), 0770);
	if (shmid == -1) {
		printf("Error: Could not get shared memory.\n");
		return 0;
	}

	int* shmaddr = shmat(shmid, NULL, SHM_RND);
	*shmaddr = c;

	int original_pid = getpid();

	int i = 0;
	for (; i < n; i++) {
		c = 'A' + i;
		if (i == n - 1) break;
		if (fork() != 0) {
			break;
		}
	}

	int j = 0;
	for (; j < n; j++) {
		while (*shmaddr != c);

		printf("%c:%d\n", c, getpid());
		char new_turn = ++(*shmaddr);
		new_turn -= 'A';
		new_turn %= n;
		new_turn += 'A';
		*shmaddr = new_turn;
	}

	if (getpid() == original_pid)
		shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
