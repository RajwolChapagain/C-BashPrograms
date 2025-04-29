// Name:		Rajwol Chapagain
// Class:		CSC 460
// Assignment:	Memory Manager - A program that simulates a memory management system that takes in requests from processes wanting to be put into RAM, 
// 				places them using the first fit algorithm for contiguous memory allocation, and simulates executing them.
// Date: 		04/24/2025
// Producer code that simulates processes requesting to be put into RAM

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct Process {
	char id;
	int pid;
	int size;
	int time;
	int proc_sem_id;
	int start_time;
	int ram_address;
	int status;
};

int main(int argc, char* argv[]) {
	// Read shared resource info from file created by consumer
	FILE* fp;
	fp = fopen("sync_info.txt", "r");

	if (!fp) {
		printf("Error: Could not find sync_info.txt in the current directory. Did you forget to run the consumer first?\n");
		return 1;
	}

	if (argc < 3) {
		printf("Error: Inadequate number of arguments provided.\nPlease enter two command-line arguments in the following order:\n");
		printf("size time\n");
		return 1;
	}

	int size = atoi(argv[1]);
	int time = atoi(argv[2]);

	int buffer_size, buffer_id, stop_id, empty_id, full_id, mutex, rear_id, rows, cols;

	fscanf(fp, "%d", &buffer_size);
	fscanf(fp, "%d", &buffer_id);
	fscanf(fp, "%d", &stop_id);
	fscanf(fp, "%d", &empty_id);
	fscanf(fp, "%d", &full_id);
	fscanf(fp, "%d", &mutex);
	fscanf(fp, "%d", &rear_id);
	fscanf(fp, "%d", &rows);
	fscanf(fp, "%d", &cols);
	fclose(fp);

	if (size < 1 || size > rows*cols) {
		printf("Error: Process size must be between 1 and %d\n", rows*cols);
		return 1;
	}

	if (time < 1 || time > 30) {
		printf("Error: Process time must be between 1 and 30\n");
		return 1;
	}

	// Get semaphore used for waiting for execution complete signal
	int proc_sem_id = semget(IPC_PRIVATE, 1, 0700);
	semctl(proc_sem_id, 0, SETVAL, 0);

	// Create a struct containing process metadata
	struct Process process;
	process.pid = getpid();
	process.size = size;
	process.time = time;
	process.proc_sem_id = proc_sem_id;

	// Attach to shared memory
	struct Process* buffer_addr = (struct Process*) shmat(buffer_id, NULL, SHM_RND);
	int* rear_addr = (int*) shmat(rear_id, NULL, SHM_RND);
	int* stop = (int*) shmat(stop_id, NULL, SHM_RND);

	printf("%d is requesting %d blocks of RAM for %d seconds\n", process.pid, size, time);
	p(0, empty_id);
	if (!*stop) {
		p(0, mutex);
		int i;
		for (i = 0; i < buffer_size; i++)
			if (buffer_addr[i].status != 0 && buffer_addr[i].status != 1)
				break;

		*rear_addr = i;

		buffer_addr[*rear_addr].pid = process.pid;
		buffer_addr[*rear_addr].size = process.size;
		buffer_addr[*rear_addr].time = process.time;
		buffer_addr[*rear_addr].proc_sem_id = process.proc_sem_id;
		v(0, mutex);
		v(0, full_id);
		
		// Wait for process request to finish execution
		p(0, proc_sem_id);

		if (!*stop)
			printf("%d finished my request of %d blocks for %d seconds\n", process.pid, size, time);
	}

	// Clean up the waiting semaphore
	semctl(proc_sem_id, 0, IPC_RMID, 0);

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
