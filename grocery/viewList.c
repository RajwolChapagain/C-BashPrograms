// Name: 		Rajwol Chapagain
// Date: 		02/28/25
// Assignment: 	Grocery View - A program that attaches to a pre-created shared memory and displays its contents in a tabular form

#include <stdio.h>
#include <sys/shm.h>

struct glistStruct {
        int id;
        char name[20];
        int favNum;
        char favFood[30];
};

int main() {
	// Read shared memory id from file
	FILE* fp;
	fp = fopen("/pub/csc460/glist/GLISTID.txt", "r");
	int shmid; 
	fscanf(fp,"%d",&shmid);
	fclose(fp);

	// Attach to shared memory
	struct glistStruct* shmaddr = (struct glistStruct*) shmat(shmid, NULL, SHM_RND);

	// Print table
	printf("HERE IS OUR SHARED GROCERY LIST:\n");
	int i;
	for (i = 0; i < 24; i++) {
		printf("%2d: %20s| %8d| %30s|\n", shmaddr->id, shmaddr->name, shmaddr->favNum, shmaddr->favFood);
		shmaddr++;
	}

	// Detach from shared memory
	shmdt(shmaddr);

	return 0;
}
