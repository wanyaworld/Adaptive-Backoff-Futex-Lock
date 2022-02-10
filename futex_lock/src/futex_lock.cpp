#include "futex_lock.h"

#include <iostream>

int futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
          int* uaddr2, int val3) {
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

void FutexLock::lock() {
  for (int i = 0; i < N_BACKOFF; i++) {
    if (0 == __sync_val_compare_and_swap(&lock_var, 0, 1)) return;
  }

  while (1) {
    int futex_ret = futex(&lock_var, FUTEX_WAIT, 1, NULL, NULL, 0);
    if (futex_ret == 0 && lock_var != 0) { /* spurious wake-up */
      continue;
    }
    int CAS_ret = __sync_val_compare_and_swap(&lock_var, 0, 1);
    if (CAS_ret == 0)
      return;
    else
      continue;
  }
}

void FutexLock::unlock() {
  __sync_val_compare_and_swap(&lock_var, 1, 0);
  futex(&lock_var, FUTEX_WAKE, 1, NULL, NULL, 0);
}
