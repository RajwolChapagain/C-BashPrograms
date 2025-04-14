#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct Job {
	int owner_id;
	char filename[20];
};

int main() {
	// Read shared resource info from file created by daemon
	FILE* fp;
	fp = fopen("sync_info.txt", "r");

	if (!fp) {
		printf("Error: Could not find sync_info.txt in the current directory. Did you forget to run the print daemon first?\n");
		return -1;
	}

	int pid = getpid();
	char command[512];

	// Download and save unique quote in a unique file
	sprintf(command, "curl -s http://api.quotable.io/random | cut -d ':' -f 3 | cut -d '\"' -f 2 > quotefile_%d", pid);
	system(command);

	// Create a struct containing file metadata
	struct Job print_job;
	print_job.owner_id = pid;
	sprintf(print_job.filename, "quotefile_%d", pid);

	int buffer_size, buffer_id, _stop_id, empty_id, full_id, mutex, rear_id;

	fscanf(fp, "%d", &buffer_size);
	fscanf(fp, "%d", &buffer_id);
	fscanf(fp, "%d", &_stop_id);
	fscanf(fp, "%d", &empty_id);
	fscanf(fp, "%d", &full_id);
	fscanf(fp, "%d", &mutex);
	fscanf(fp, "%d", &rear_id);

	// Attach to shared memory
	struct Job* buffer_addr = (struct Job*) shmat(buffer_id, NULL, SHM_RND);
	int* rear_addr = (int*) shmat(rear_id, NULL, SHM_RND);

	int i, sleep_duration;
	srand(pid);
	for (i = 0; i < 5; i++) {
		sleep_duration = (rand() % 4) + 2;
		printf("User %d is working for %d seconds\n", pid, sleep_duration);
		sleep(sleep_duration);
		printf("User %d is printing %s\n", pid, print_job.filename);
		p(0, empty_id);
		p(0, mutex);
		buffer_addr[*rear_addr].owner_id = print_job.owner_id;
		strcpy(buffer_addr[*rear_addr].filename, print_job.filename);
		*rear_addr = (*rear_addr + 1) % buffer_size;
		v(0, mutex);
		v(0, full_id);
	}

	printf("User %d is logging off\n", pid);

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
