#include "futex_lock.h"
#include <assert.h>
#include <memory>
#include <vector>

int* shared_data;
const unsigned int N_THREADS = 64;
const unsigned int N_INC = 10000;
std::unique_ptr<FutexLock> pLock;

static const unsigned int N_CASES = 10; /* How many cases we're gonna average. */
static const unsigned int MAX_BACKOFF = N_THREADS / 1;

void inc() {
  for (int i = 0 ; i < N_INC ; i++) {
    pLock->lock();
    (*shared_data)++;
    pLock->unlock();
  }
}

unsigned int do_futex_expr() {
  int shm_id = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);
  assert(shm_id >= 0);

  shared_data = (int*)shmat(shm_id, NULL, 0);

  *shared_data = 0;

  pLock->unlock();

  auto pThr = new std::thread[N_THREADS]();
  for (int i = 0 ; i < N_THREADS ; i++ )
    pThr[i] = std::thread(inc);

  auto start = std::chrono::steady_clock::now();
  for (int i = 0 ; i < N_THREADS ; i++ )
    pThr[i].join();

  auto end = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();  
}

int main(int argc, char *argv[])
{
  pLock = std::make_unique<FutexLock>();
  auto *usec_list = new std::vector<unsigned int>(MAX_BACKOFF + 1);

  for (int i = 1 ; i <= MAX_BACKOFF || i == 1 ; i++) {
    pLock->N_BACKOFF = i;
    unsigned int accum = 0;
    std::cout << "performing ";
    std::cout << i;
    std::cout << "th test" << std::endl;
    for (int j = 0 ; j < N_CASES ; j++)
      accum += do_futex_expr();
    (*usec_list)[i] = accum / N_CASES;
  }

  std::cout << "**** Threads: ";
  std::cout << N_THREADS;
  std::cout << "****" << std::endl;
  for (int i = 1 ; i <= MAX_BACKOFF ; i++) {
    std::cout << "[";
    std::cout << i;
    std::cout << "]: ";
    std::cout << (*usec_list)[i] << std::endl;
  }

  return 0;
}

