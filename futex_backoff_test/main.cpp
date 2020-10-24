#include "futex_lock.h"
#include "../locks_test/bench.h"
#include <assert.h>
#include <memory>
#include <vector>

int* shared_data;
const unsigned int N_THREADS = 64;
const unsigned int N_INC = 10000;
std::unique_ptr<FutexLock> pLock;

static const unsigned int N_CASES = 5; /* How many cases we're gonna average. */
static const unsigned int MAX_BACKOFF = N_THREADS / 1;

unsigned int duration = 3;

static void sighandler (int x) {
	bench->stop = 1;
}

void inc(const unsigned int thr_id) {
  for (int i = 0 ; !bench->stop; i++) {
    pLock->lock();
    (bench->workers[thr_id].works)++;
    pLock->unlock();
  }
}

unsigned long long int do_futex_expr() {
  int shm_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
  assert(shm_id >= 0);

  shared_data = (int*)shmat(shm_id, NULL, 0);

  *shared_data = 0;

  pLock->unlock();
	
	init_bench(&bench, N_THREADS, duration);
	
	if (signal(SIGALRM, sighandler) == SIG_ERR) {
		return -1;
	}
	
	alarm(bench->duration);
  
	auto pThr = new std::thread[N_THREADS]();
  for (int i = 0 ; i < N_THREADS ; i++ )
    pThr[i] = std::thread(inc, i);

  for (int i = 0 ; i < N_THREADS ; i++ )
    pThr[i].join();

	unsigned long long int sum{};
	for (int i = 0 ; i < bench->n_workers ; i++)
		sum += bench->workers[i].works;
	
	uninit_bench(bench);
	return sum;
}

int main(int argc, char *argv[])
{
  pLock = std::make_unique<FutexLock>();
  auto *usec_list = new std::vector<unsigned long long int>(MAX_BACKOFF + 1);

  for (int i = 1 ; i <= MAX_BACKOFF || i == 1 ; i++) {
    pLock->N_BACKOFF = i;
    unsigned int accum = 0;
    std::cout << "performing ";
    std::cout << i;
    std::cout << "th test" << std::endl;
    for (int j = 0 ; j < N_CASES ; j++) {
    	std::cout << "	performing ";
    	std::cout << j;
    	std::cout << "th test" << std::endl;
      accum += do_futex_expr();
		}
    (*usec_list)[i] = accum / N_CASES;
  }

  std::cout << "**** Threads: ";
  std::cout << N_THREADS;
  std::cout << "****" << std::endl;
  for (int i = 1 ; i <= MAX_BACKOFF ; i++) {
    std::cout << (*usec_list)[i] << std::endl;
  }

  return 0;
}

