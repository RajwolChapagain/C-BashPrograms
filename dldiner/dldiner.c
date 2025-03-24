// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 03/21/2025
// Assignment: Deadlock Diner - A program that spawns 5 total processes sharing common resources that eventually run into a deadlock

#include <stdio.h>
#include <sys/sem.h>

void think(int id);
void get_hungry(int id, int semid);
void eat(int id, int semid);
void print_tabs(int num_tabs);
void waste_cpu();

int main() {
	int num_sems = 5;
	// Get semaphores
	int semid = semget(IPC_PRIVATE, num_sems, 0770);

	if (semid == -1) {
		printf("Error: SemGet Failed\n");
		return -1;
	}

	int i;
	// Initialize sempahores to 1
	for (i = 0; i < num_sems; i++) {
		semctl(semid, i, SETVAL, 1);
	}

	int original_pid = getpid();

	// Fork and assign unique ids from 0-4
	for (i = 0; i < 4; i++) {
		if (fork() != 0) break;
	}

	// Set random seed based on process id
	srand(getpid());

	// Main loop
	while (1) {
		think(i);
		get_hungry(i, semid);
		eat(i, semid);
	}

	return 0;
}

void think(int id) {
	print_tabs(id);
	printf("%d THINKING\n", id);
	waste_cpu();
	waste_cpu();
}

void get_hungry(int id, int semid) {
	print_tabs(id);
	printf("%d HUNGRY\n", id);
	p((id - 1 + 5) % 5, semid);
	waste_cpu();
	p((id + 1) % 5, semid);
}

void eat(int id, int semid) {
	print_tabs(id);
	printf("%d EATING\n", id);
	waste_cpu();
	v((id - 1 + 5) % 5, semid);
	waste_cpu();
	v((id + 1) % 5, semid);
}

void waste_cpu() {
	int i, j;
	for (i = 0; i < rand(); i++);
}

void print_tabs(int num_tabs) {
	int i;
	for (i = 0; i < num_tabs; i++)
		printf("\t");
}

p(int s,int sem_id)
{
	struct sembuf sops;

	sops.sem_num = s;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1) printf("%s", "'P' error\n");
}

v(int s, int sem_id)
{
	struct sembuf sops;

	sops.sem_num = s;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	if((semop(sem_id, &sops, 1)) == -1) printf("%s","'V' error\n");
}

