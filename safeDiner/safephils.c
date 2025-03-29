// Name: Rajwol Chapagain
// Class: CSC 460
// Date: 03/28/2025
// Assignment: Safe Diner - A program that spawns 5 total processes that synchronize to use shared resources for 60 seconds

// Define some macros
#define N 5
#define LEFT (id+N-1)%N
#define RIGHT (id+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define DEAD 3

#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>

int utensils_id, mutex_id;
int state_id, clock_id;

void think();
void eat();
void take_forks(int id);
void test(int id);
void put_forks(int id);

int main() {
	// Get semaphores for utensils
	utensils_id = semget(IPC_PRIVATE, N, 0770);
	int i;

	// Initialize utensil semaphores to 1
	for (i = 0; i < N; i++)
		semctl(utensils_id, i, SETVAL, 1);

	// Get mutex semaphore and initialize to 1
	mutex_id = semget(IPC_PRIVATE, 1, 0770);
	semctl(mutex_id, 0, SETVAL, 1);

	// Get shared memory for states
	state_id = shmget(IPC_PRIVATE, N*sizeof(int), 0770);
	
	int* state_addr = (int*) shmat(state_id, NULL, SHM_RND);
	// Initialize states to THINKING
	for (i = 0; i < N; i++)
		state_addr[i] = THINKING;

	// Get shared memory for clock
	clock_id = shmget(IPC_PRIVATE, sizeof(int), 0770);
	int* clock_addr = (int*) shmat(clock_id, NULL, SHM_RND);

	int phil_id = -1;

	// Spawn and assign unique ids to N philosophers
	for (i = 0; i < N; i++) {
		if (fork() != 0) break;
		else phil_id = i;
	}

	if (phil_id == -1) { // Main
		int start_time = (int) time(NULL);
		*clock_addr = 0;
        while (phils_alive()) {
			*clock_addr = (int) time(NULL) - start_time;
            print_states();
            sleep(1);
        }

		// Clean up shared memory and semaphore arrays
		semctl(utensils_id, 0, IPC_RMID, 0);
		semctl(mutex_id, 0, IPC_RMID, 0);
		shmctl(state_id, IPC_RMID, NULL);
		shmctl(clock_id, IPC_RMID, NULL);
	} else { // Philosophers
		srand(getpid());
		while(*clock_addr < 60) {
			think();
			take_forks(phil_id);
			eat();
			put_forks(phil_id);
		}

		// Set state to dead when timer is up
		state_addr[phil_id] = DEAD;
	}

	return 0;
}

void think() {
	sleep((rand() % 5) + 1);
}

void eat() {
	sleep((rand() % 3) + 1);
}

// Take forks depending on how safe it is
void take_forks(int id) {
	p(0, mutex_id);
    int* state_addr = shmat(state_id, NULL, SHM_RND);
	state_addr[id] = HUNGRY;
	test(id);
	v(0, mutex_id);
	v(id, utensils_id);
}

// Put forks back once done eating
void put_forks(int id) {
	p(0, mutex_id);
    int* state_addr = shmat(state_id, NULL, SHM_RND);
	state_addr[id] = THINKING;
	test(LEFT);
	test(RIGHT);
	v(0, mutex_id);
}

// Start eating if it's safe to do so
void test(int id) {
    int* state_addr = shmat(state_id, NULL, SHM_RND);
	if (state_addr[id] == HUNGRY && state_addr[LEFT] != EATING && 
			state_addr[RIGHT] != EATING) {
		state_addr[id] = EATING;
		v(id, utensils_id);
	}
}

// A function that prints the states of all 5 philosphers at a given time
int print_states() {
	int* clock_addr = shmat(clock_id, NULL, SHM_RND);
    printf("%d. ", *clock_addr);
    int* state_addr = shmat(state_id, NULL, SHM_RND);
    int i;
    for (i = 0; i < N; i++) {
        if (state_addr[i] == THINKING)
            printf("thinking ");
        else if (state_addr[i] == HUNGRY)
            printf("hungry ");
        else if (state_addr[i] == EATING)
            printf("eating ");
        else if (state_addr[i] == DEAD)
            printf("dead ");
    }
    printf("\n");
}

// A function that that returns 1 when any of the philosophers are alive and 0 when all are dead
int phils_alive() {
    int i;
    int* state_addr = shmat(state_id, NULL, SHM_RND);
    for (i = 0; i < N; i++)
        if (state_addr[i] != DEAD) return 1;

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
