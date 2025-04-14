// Stop file for the print daemon program that signals the printer to shut down

#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main() {
	FILE* fp;
	fp = fopen("sync_info.txt", "r");
	
	// sync_info.txt is expected to be formatted as follows:
	// buffer_size (int) \n (char)
	// buffer_id (int) \n (char)
	// stop_id (int) \n (char)
	// empty_id (int) \n (char)
	// full_id (int) \n (char)
	// mutex (int) \n (char)
	
	int stop_id;
	int i;
	for (i = 0; i < 3; i++) // Since stop_id is on the third line
		fscanf(fp, "%d", &stop_id);

	// Rewind fp to point at the beginning of the file
	rewind(fp);
	int full_id;
	for (i = 0; i < 5; i++) // Since full_id is on the fifth line
		fscanf(fp, "%d", &full_id);

	fclose(fp);

	// Wake printer up in case it's blocked
	v(0, full_id);

	int* stop = (int* ) shmat(stop_id, NULL, SHM_RND);

	// Set stop to 1, which should signal the print daemon to stop
	*stop = 1;

	return 0;
}

v(int s, int sem_id) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = 1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}
