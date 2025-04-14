#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct Job {
	int owner_id;
	char filename[20];
};

int main(int argc, char* argv[]) {
	FILE* fp;
	fp = fopen("sync_info.txt", "r");

	if (fp) {
		printf("Error: sync_info.txt already exists in the current directory. Is another print daemon running?\n");
		fclose(fp);
		return -1;
	}

	if (argc < 2) {
		printf("Error: Please enter a buffer size from 1-10 as a cmdline argument\n");
		return -1;
	}

	int buffer_size = atoi(argv[1]);

	if (buffer_size < 1 || buffer_size > 10) {
		printf("Error: Please enter a buffer size from 1-10 as a cmdline argument\n");
		return -1;
	}

	// Get shared memory
	int shm_id = shmget(IPC_PRIVATE, sizeof(struct Job) * buffer_size, 0700);
	int stop_id = shmget(IPC_PRIVATE, sizeof(int), 0700);
	int rear_id = shmget(IPC_PRIVATE, sizeof(int), 0700);

	// Get semaphores
	int empty_id = semget(IPC_PRIVATE, 1, 0700);
	int full_id = semget(IPC_PRIVATE, 1, 0700);
	int mutex = semget(IPC_PRIVATE, 1, 0700);

	// Initialize buffer front and rear
	int front = 0;
	int* rear_addr = (int*) shmat(rear_id, NULL, SHM_RND);
	*rear_addr = 0;	

	// Initialize semaphores
	semctl(empty_id, 0, SETVAL, buffer_size);
	semctl(full_id, 0, SETVAL, 0);
	semctl(mutex, 0, SETVAL, 1);

	// Save info to file
	fp = fopen("sync_info.txt", "w");
	fprintf(fp, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n", buffer_size, shm_id, stop_id, empty_id, full_id, mutex, rear_id);
	fclose(fp);

	int *stop = (int *) shmat(stop_id, NULL, SHM_RND);
	*stop = 0;

	struct Job* buffer_addr = (struct Job*) shmat(shm_id, NULL, SHM_RND);
	// Print loop
	while (!*stop) {
		p(0, full_id);
		if (*stop) break; // stop.c could've woken it up
		p(0, mutex);
		printf("Printing %d's file...\n", buffer_addr[front].owner_id);
		char command[512];
		sprintf(command, "cat %s", buffer_addr[front].filename);
		system(command);
		front = (front + 1) % buffer_size;
		v(0, mutex);
		v(0, empty_id);
	}

	// Clean up
	shmctl(shm_id, IPC_RMID, NULL);
	shmctl(stop_id, IPC_RMID, NULL);
	shmctl(rear_id, IPC_RMID, NULL);
	semctl(empty_id, 0, IPC_RMID, 0);
	semctl(full_id, 0, IPC_RMID, 0);
	semctl(mutex, 0, IPC_RMID, 0);
	remove("sync_info.txt");
	system("rm quotefile* &> /dev/null");

	printf("Printer shutting down.\n");
	return 0;
}

p(int s,int sem_id) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = 1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}
