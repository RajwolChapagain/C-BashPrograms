#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>

void on_player_hit(int sem_id, int* dead, int* hp);

int main() {
	int sem_id, dead_id, hp_id;
	FILE* fp;
	fp = fopen("sem_addr.txt", "r");
	fscanf(fp, "%d", &sem_id);
	fscanf(fp, "%d", &dead_id);
	fscanf(fp, "%d", &hp_id);
	fclose(fp);

	int* dead = (int*) shmat(dead_id, NULL, SHM_RND);
	int* hp = (int*) shmat(hp_id, NULL, SHM_RND);
	on_player_hit(sem_id, dead, hp);	
	return 0;
}

void on_player_hit(int sem_id, int* dead, int* hp) {
	while (!*dead) {
		p(0, sem_id);
		printf("Player took damage. New hp is %d\n", *hp);
	}
	printf("Player died!\n");
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



