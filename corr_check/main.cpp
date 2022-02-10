#include "bench.h"
#include "futex_lock/include/futex_lock.h"
#include "utils.h"

using corr_check::utils::N_THREADS;
using corr_check::utils::shared_data;
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

  corr_check::utils::do_dummy();
  corr_check::utils::do_CAS();
  corr_check::utils::do_pthr_mutex();
  corr_check::utils::do_futex();

  return 0;
}
