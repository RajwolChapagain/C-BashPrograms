// Name:		Rajwol Chapagain
// Class:		CSC 460
// Assignment:	Memory Manager - A program that simulates a memory management system that takes in requests from processes wanting to be put into RAM, 
// 				places them using the first fit algorithm for contiguous memory allocation, and simulates executing them.
// Date: 		04/24/2025

#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <limits.h>

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
	FILE* fp;
	fp = fopen("sync_info.txt", "r");

	if (fp) {
		printf("Error: sync_info.txt already exists in the current directory. Is another consumer running?\n");
		fclose(fp);
		return 1;
	}

	if (argc < 4) {
		printf("Error: Inadequate number of arguments provided.\nPlease enter three command-line arguments in the following order:\n");
		printf("rows cols buffersize\n");
		printf("\t where 1 <= rows <= 20, and 1 <= cols <= 50, 1 <= buffersize <= 26\n");
		return 1;
	}

	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	int buffer_size = atoi(argv[3]);

	if (rows < 1 || rows > 20) {
		printf("Error: Please enter a row size from 1-20\n");
		return 1;
	}

	if (cols < 1 || cols > 50) {
		printf("Error: Please enter a column size from 1-50\n");
		return 1;
	}

	if (buffer_size < 1 || buffer_size > 26) {
		printf("Error: Please enter a buffer size from 1-26\n");
		return 1;
	}

	// Get shared memory
	int shm_id = shmget(IPC_PRIVATE, sizeof(struct Process) * buffer_size, 0700);
	int stop_id = shmget(IPC_PRIVATE, sizeof(int), 0700);
	int rear_id = shmget(IPC_PRIVATE, sizeof(int), 0700);

	// Get semaphores
	int empty_id = semget(IPC_PRIVATE, 1, 0700);
	int full_id = semget(IPC_PRIVATE, 1, 0700);
	int mutex = semget(IPC_PRIVATE, 1, 0700);

	// Initialize buffer rear
	int* rear_addr = (int*) shmat(rear_id, NULL, SHM_RND);
	*rear_addr = 0;	

	// Initialize semaphores
	semctl(empty_id, 0, SETVAL, buffer_size);
	semctl(full_id, 0, SETVAL, 0);
	semctl(mutex, 0, SETVAL, 1);

	// Save info to file
	fp = fopen("sync_info.txt", "w");
	fprintf(fp, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", buffer_size, shm_id, stop_id, empty_id, full_id, mutex, rear_id, rows, cols);
	fclose(fp);

	int *stop = (int *) shmat(stop_id, NULL, SHM_RND);
	*stop = 0;

	struct Process* buffer_addr = (struct Process*) shmat(shm_id, NULL, SHM_RND);

	// Initialize shmem buffer
	int l;
	for (l = 0; l < buffer_size; l++) {
		buffer_addr[l].status = INT_MAX;
	}

	int orig_id = getpid();
	if (fork() != 0) {  // Parent process responsible for receiving RAM requests
		char id = 'A';
		while (!*stop) {
			p(0, full_id);
			if (*stop) break; // stop.c could've woken it up
			p(0, mutex);
			buffer_addr[*rear_addr].id = id;
			buffer_addr[*rear_addr].start_time = (int) time(NULL);
			buffer_addr[*rear_addr].ram_address = -1;
			buffer_addr[*rear_addr].status = 0;
			id++;
			id = (char) ('A' + ((id - 'A') % 26));
			v(0, mutex);
		}
	} else { // Child process responsible for placing processes into RAM
			 // 	and displaying RAM status
		
		char ram[rows*cols];
		char* ram_ptr = ram;
		int i, j;
		// Initialize ram with .
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				*ram_ptr = '.';
				ram_ptr++;
			}
		}

		while (!*stop) {
			// Update RAM table by looking at every process in the buffer
			for (i = 0; i < buffer_size; i++) {
				if (buffer_addr[i].status == 1) {  // If a process is in RAM
					if (buffer_addr[i].time <= 0) { // If a process has finished executing
						buffer_addr[i].status = -1;
						int counter;
						for (counter = 0; counter < buffer_addr[i].size; counter++)
							ram[buffer_addr[i].ram_address + counter] = '.';

						v(0, buffer_addr[i].proc_sem_id);
						v(0, empty_id);
					} else {  // If a process is still executing
						buffer_addr[i].time--;
					}
				} else if (buffer_addr[i].status == 0) { // If a process is waiting to get into RAM
					// Use first fit to place it in RAM
					int index = 0, empty_len = 0;
					for (; index < rows * cols; index++) {
						if (ram[index] == '.') empty_len++;
						else empty_len = 0;
						if (buffer_addr[i].size == empty_len) break;
					}

					if (index != rows*cols) { // An empty slot was found
						int empty_addr = index - empty_len + 1;
						buffer_addr[i].ram_address = empty_addr;
						int current_index;
						for (current_index = 0; current_index < buffer_addr[i].size; current_index++)
							ram[empty_addr + current_index] = buffer_addr[i].id;

						buffer_addr[i].status = 1;
					}
				}
			}
			
			// Display RAM table
			printf("\e[1;1H\e[2J");
			printf("%-10s%-10s%-10s%-10s\n", "ID", "PID", "SIZE", "TIME");
			for (i = 0; i < buffer_size; i++) {
				if (buffer_addr[i].status == 0 || buffer_addr[i].status == 1)
					printf("%-10c%-10d%-10d%-10d\n", buffer_addr[i].id, buffer_addr[i].pid, buffer_addr[i].size, buffer_addr[i].time);
			}

			printf("\nRAM Table:\n");
			for (i = 0; i < cols; i++)
				printf("-");
			printf("\n");

			int k = 0;
			for (i = 0; i < rows; i++) {
				for (j = 0; j < cols; j++) {
					printf("%c", ram[k]);
					k++;
				}
				printf("\n");
			}
			sleep(1);
		}
	}

	if (getpid() == orig_id) {
		int i;
		// Wake all dormant producers up
		for (i = 0; i < buffer_size; i++) {
			if (buffer_addr[i].status == 1 || buffer_addr[i].status == -1 || buffer_addr[i].status == 0)
					v(0, buffer_addr[i].proc_sem_id);
		}

		// Clean up
		shmctl(shm_id, IPC_RMID, NULL);
		shmctl(stop_id, IPC_RMID, NULL);
		shmctl(rear_id, IPC_RMID, NULL);
		semctl(empty_id, 0, IPC_RMID, 0);
		semctl(full_id, 0, IPC_RMID, 0);
		semctl(mutex, 0, IPC_RMID, 0);
		remove("sync_info.txt");

		printf("Consumer shutting down.\n");
	}

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
