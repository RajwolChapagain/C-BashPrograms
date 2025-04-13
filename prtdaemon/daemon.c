#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct Job {
	int owner_id;
	char filename[20];
};

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

	// Get shared memory buffer
	int shm_id = shmget(IPC_PRIVATE, sizeof(struct Job) * buffer_size, 0700);

	// Get semaphores
	int empty_id = semget(IPC_PRIVATE, 1, 0700);
	int full_id = semget(IPC_PRIVATE, 1, 0700);
	int mutex = semget(IPC_PRIVATE, 1, 0700);

	// Initialize semaphores
	semctl(empty_id, 0, SETVAL, buffer_size);
	semctl(full_id, 0, SETVAL, 0);
	semctl(mutex, 0, SETVAL, 1);


	// Clean up
	shmctl(shm_id, IPC_RMID, NULL);
	semctl(empty_id, 0, IPC_RMID, 0);
	semctl(full_id, 0, IPC_RMID, 0);
	semctl(mutex, 0, IPC_RMID, 0);

	return 0;
}
