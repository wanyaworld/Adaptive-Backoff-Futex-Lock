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

using Time = std::chrono::time_point<std::chrono::steady_clock>;
extern int* shared_data;
extern const unsigned int N_THREADS;
extern const unsigned int N_INC;
extern unsigned int N_BACKOFF;

extern std::mutex mutex;
extern int CAS_lock_var;
extern int futex_lock_var;

void do_dummy();
void do_CAS();
void do_pthr_mutex();
void do_futex();
unsigned int do_futex_ret();
