// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 03/12/25
// Assignment: Fastest Sync - A program that spawns n processes including itself where n is a cmd line argument and
// 							prints out n lines per process syncing them via semaphores

#include <stdio.h>
#include <sys/sem.h>

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

	int semid = semget(IPC_PRIVATE, n, 0770);
	if (semid == -1) {
		printf("Error: Could not get semaphore(s).\n");
		return 0;
	}

	int original_pid = getpid();

	int i;
	// Initialize semaphores
	for (i = 0; i < n; i++)
		semctl(semid, i, SETVAL, i == 0);

	char c = 'A';
	// Assign characters
	for (i = 0; i < n; i++) {
		c = 'A' + i;
		if (i == n - 1) break;
		if (fork() != 0) {
			break;
		}
	}

	// Print character and pid synchronously
	for (i = 0; i < n; i++) {
		p(c - 'A', semid);
		printf("%c:%d\n", c, getpid());
		v((c - 'A' + 1) % n, semid);
	}

	sleep(1);
	// Free up reserved semaphore array
	if (getpid() == original_pid)
		semctl(semid, 0, IPC_RMID, 0);
		
	return 0;
}

p(int s,int sem_id)
{
    struct sembuf sops;

    sops.sem_num = s;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    semop(sem_id, &sops, 1);
}

v(int s, int sem_id)
{
    struct sembuf sops;

    sops.sem_num = s;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    semop(sem_id, &sops, 1);
}
