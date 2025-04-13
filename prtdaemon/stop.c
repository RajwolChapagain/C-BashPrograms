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
	fseek(fp, (sizeof(int) + sizeof(char))* 2, SEEK_SET);
	
	int stop_id;
	fscanf(fp, "%d", &stop_id);

	// Rewind fp to point at the beginning of the file
	rewind(fp);
	fseek(fp, (sizeof(int) + sizeof(char))* 5, SEEK_SET);
	int full_id;
	fscanf(fp, "%d", &full_id);

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
