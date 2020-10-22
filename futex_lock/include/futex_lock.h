#ifndef __FUTEX_LOCK_H__
#define __FUTEX_LOCK_H__

#include <errno.h>
#include <linux/futex.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>

class FutexLock
{
public:
    enum LockState
    {
      UNLOCKED,
      LOCKED
    };
    FutexLock() : lock_var(UNLOCKED), N_BACKOFF(1) {}
    FutexLock(const unsigned int backoff) : FutexLock() { N_BACKOFF = backoff; }
    ~FutexLock() {}
    void lock();
    void unlock();
    unsigned int N_BACKOFF;
private:
    int lock_var;
};

#endif
