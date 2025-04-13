#include <stdio.h>
#include <sys/shm.h>

int main() {
	FILE* fp;
	fp = fopen("sync_info.txt", "r");
	
	// Seek to stop id formatted as follows:
	// buffer_size (int) \n (char)
	// buffer_id (int) \n (char)
	// stop_id (int) ...
	fseek(fp, (sizeof(int) + sizeof(char))* 2, SEEK_SET);
	
	int stop_id;
	fscanf(fp, "%d", &stop_id);

	int* stop = (int* ) shmat(stop_id, NULL, SHM_RND);

	// Set stop to 1, which should signal the print daemon to stop
	*stop = 1;

	return 0;
}

