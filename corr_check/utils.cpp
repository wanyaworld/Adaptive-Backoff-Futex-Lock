#include "utils.h"

#include <thread>

#include "bench.h"

namespace corr_check::utils {
static const unsigned int MSEC_TO_USEC = 1000;

int* shared_data;
unsigned int N_THREADS = 64;
unsigned int N_BACKOFF = 10;

std::mutex mutex;
std::mutex mutex_heavy;
int CAS_lock_var;
int CAS_lock_var_heavy;
int futex_lock_var;
int futex_lock_var_heavy;

std::thread::id parent_id;
unsigned int duration = 5;

static void sighandler(int x) { bench->stop = 1; }

void sleep_msec(const unsigned int msec, void (*pf_lock)(int*),
                void (*pf_unlock)(int*), const unsigned int thr_id,
                int* lock_var) {
  for (int i = 0; !bench->stop; i++) {
    pf_lock(lock_var);
    usleep(msec * MSEC_TO_USEC);
    pf_unlock(lock_var);
    (bench->workers[thr_id].heavy_works)++;
  }
}

void inc(void (*pf_lock)(int*), void (*pf_unlock)(int*),
         const unsigned int thr_id, int* lock_var) {
  for (int i = 0; !bench->stop; i++) {
    pf_lock(lock_var);
    (*shared_data)++;
    pf_unlock(lock_var);
    (bench->workers[thr_id].works)++;
  }
}

void print_result(const std::string& lock_method) {
  unsigned long long int sum{}, sum_heavy{};
  for (int i = 0; i < bench->n_workers; i++) {
    sum += bench->workers[i].works;
    sum_heavy += bench->workers[i].heavy_works;
  }

  std::cout << "[";
  std::cout << sum;
  std::cout << "] (";
  std::cout << lock_method;
  std::cout << ")";

  std::string is_correct;

  if (*shared_data == sum)
    is_correct = (std::string) " (correct)";

  else
    is_correct = (std::string) " (NOT correct)";

  std::cout << is_correct << std::endl;
}

int futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
          int* uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void dummy_func(int* dummy) {}
void CAS_lock(int* CAS_lock_var) {
  while (1) {
    int ret = __sync_val_compare_and_swap(CAS_lock_var, 0, 1);
    if (ret == 0) break;
  }
}

void CAS_unlock(int* CAS_lock_var) {
  __sync_val_compare_and_swap(CAS_lock_var, 1, 0);
}

void futex_lock(int* futex_lock_var) {
  for (int i = 0; i < N_BACKOFF; i++) {
    if (0 == __sync_val_compare_and_swap(futex_lock_var, 0, 1)) return;
  }

  while (1) {
    int futex_ret = futex(futex_lock_var, FUTEX_WAIT, 1, NULL, NULL, 0);
    if (futex_ret == 0 && *futex_lock_var != 0) { /* spurious wake-up */
      continue;
    }
    int CAS_ret = __sync_val_compare_and_swap(futex_lock_var, 0, 1);
    if (CAS_ret == 0)
      return;
    else
      continue;
  }
}

void futex_unlock(int* futex_lock_var) {
  __sync_val_compare_and_swap(futex_lock_var, 1, 0);
  futex(futex_lock_var, FUTEX_WAKE, 1, NULL, NULL, 0);
}

void mutex_lock(int* pm) { ((std::mutex*)pm)->lock(); }

void mutex_unlock(int* pm) { ((std::mutex*)pm)->unlock(); }
void perform(struct lock_bench_instance* lb) {
  init_bench(&bench, N_THREADS, duration);

  if (signal(SIGALRM, sighandler) == SIG_ERR) {
    return;
  }

  alarm(bench->duration);

  std::unique_ptr<std::thread> pThr[N_THREADS];

  for (int i = 0; i < N_THREADS; i++) {
    try {
      pThr[i] = std::make_unique<std::thread>(inc, lb->lock, lb->unlock, i,
                                              lb->lock_var);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    }
  }

  for (int i = 0; i < N_THREADS; i++) pThr[i]->join();

  print_result(lb->lock_name);
  uninit_bench(bench);
}

void do_dummy() {
  *shared_data = 0;

  auto plb = std::make_unique<struct lock_bench_instance>();
  plb->lock = dummy_func;
  plb->unlock = dummy_func;
  plb->lock_var = nullptr;
  plb->lock_var_heavy = nullptr;
  plb->lock_name = (std::string) "dummy-lock";

  perform(plb.get());
}

void do_CAS() {
  *shared_data = 0;
  CAS_lock_var = 0;

  auto plb = std::make_unique<struct lock_bench_instance>();
  plb->lock = CAS_lock;
  plb->unlock = CAS_unlock;
  plb->lock_var = &CAS_lock_var;
  plb->lock_var_heavy = &CAS_lock_var_heavy;
  plb->lock_name = (std::string) "CAS-lock";

  perform(plb.get());
}

void do_futex() {
  *shared_data = 0;
  futex_lock_var = 0;

  auto plb = std::make_unique<struct lock_bench_instance>();
  plb->lock = futex_lock;
  plb->unlock = futex_unlock;
  plb->lock_var = &futex_lock_var;
  plb->lock_var_heavy = &futex_lock_var_heavy;
  plb->lock_name = (std::string) "futex-lock";

  perform(plb.get());
}

void do_pthr_mutex() {
  *shared_data = 0;
  mutex.unlock();
  mutex_heavy.unlock();

  auto plb = std::make_unique<struct lock_bench_instance>();
  plb->lock = mutex_lock;
  plb->unlock = mutex_unlock;
  plb->lock_var = (int*)&mutex;
  plb->lock_var_heavy = (int*)&mutex_heavy;
  plb->lock_name = (std::string) "pthread-mutex-lock";

  perform(plb.get());
}

}  // namespace corr_check::utils
