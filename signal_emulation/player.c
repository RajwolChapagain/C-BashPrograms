#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

int main() {
	int sem_id = semget(IPC_PRIVATE, 1, 0700);
	int dead_id = shmget(IPC_PRIVATE, sizeof(int), 0770);
	int hp_id = shmget(IPC_PRIVATE, sizeof(int), 0770);
	FILE* fp;
	fp = fopen("sem_addr.txt", "w");
	fprintf(fp, "%d\n", sem_id);
	fprintf(fp, "%d\n", dead_id);
	fprintf(fp, "%d\n", hp_id);
	fclose(fp);

	int* dead = (int*) shmat(dead_id, NULL, SHM_RND);
	*dead = 0;

	int* hp = (int*) shmat(hp_id, NULL, SHM_RND);
	*hp = 100;

	int x;
	while (*hp > 0) {
		scanf("%d", &x);
		*hp -= x;
		v(0, sem_id);
	}

	*dead = 1;
	semctl(sem_id, 0, IPC_RMID, 0);
	shmctl(dead_id, IPC_RMID, NULL);
	shmctl(hp_id, IPC_RMID, NULL);
	remove("sem_addr.txt");
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
