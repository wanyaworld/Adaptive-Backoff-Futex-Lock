#include "utils.h"
#include "bench.h"

int main(int argc, char** argv) {
	int shm_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
	if (shm_id < 0) {
		perror("shmget");
		exit(1);
	}
	shared_data = (int*)shmat(shm_id, NULL, 0);

	if (argc > 1) {
		N_THREADS = atoi(argv[1]);
		if (N_THREADS < 0 || N_THREADS > 10000) N_THREADS = 64;
	}

	std::cout << N_THREADS << " threads" << std::endl;

	do_dummy();
	do_CAS();
	do_pthr_mutex();
	do_futex();

	return 0;
}
