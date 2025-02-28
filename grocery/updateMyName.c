// Name: 		Rajwol Chapagain
// Class:		CSC 460
// Date: 		02/28/25
// Assignment: 	Grocery Update Name - A program that takes in a name from the command line and writes it to the appropriate place in shared memory

#include <stdio.h>
#include <sys/shm.h>
#include <string.h>

struct glistStruct {
        int id;
        char name[20];
        int favNum;
        char favFood[30];
};

int main(int argc, char* argv[]) {
	char* name = argv[1];
	// Ensure that the 20th character is a NULL terminator
	name[19] = (char) 0;

	// Read shared memory id from file
	FILE* fp;
	fp = fopen("/pub/csc460/glist/GLISTID.txt", "r");
	int shmid; 
	fscanf(fp,"%d",&shmid);
	fclose(fp);

	// Attach to shared memory
	struct glistStruct* shmaddr = (struct glistStruct*) shmat(shmid, NULL, SHM_RND);

	// Increment shmaddr to point to my struct
	int myid = 4;
	int i;
	for (i = 1; i < myid; i++)
		shmaddr++;

	// Write to name
	strcpy(shmaddr->name, name);

	// Detach from shared memory
	shmdt(shmaddr);

	return 0;
}
