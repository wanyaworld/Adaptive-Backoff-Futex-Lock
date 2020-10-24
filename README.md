
<!-- ABOUT THE PROJECT -->
## About The Project

This is an implementation of [futex](https://man7.org/linux/man-pages/man2/futex.2.html)-based blocking synchronization. In this lock implementation, one first tries
atomic operations ([Compare-And-Swap](https://en.wikipedia.org/wiki/Compare-and-swap)) to test if one can get the lock immediately. If it fails, it calls futex() which makes it (probably) sleep. Later, it is waken up when
the lock holder releases the lock by calling futex() with FUTEX_WAKE operation flag. Even after waking up, it checks if the lock variable represents that lock is "not-held-state" and 
retries futex() if it does not. This prevents [sprious wakeup](https://en.wikipedia.org/wiki/Spurious_wakeup) from negating the correctness of the synchronization.

To test the correctness of our implementation, we conducted a experiment where we create N_THREADS threads and each thread 

1. calls lock() method
2. increases the global variable 
3. calls unlock() method
4. repeats \[1-3\] N_INC times.

Then checks if (N_THREADS * N_INC) is equal to the value of the global variable. In addition to the futex-lock, we also conduct the same test using dummy lock, simple Compare-And-Swap lock and pthread_mutex_lock. The experiment showed that every lock except dummy lock guarantees correct synchronization.

## Performance Evaluation
We run tests to investigate the performace of the futex-lock. In all tests, threads acquire the lock and perform simple global variable increment.

### futex_backoff_test
In our futex lock, we try atomic operations (CAS) for the lock variable before we call futex(). We vary the number of CAS and evaluate the lock performance to investigate how the futex-lock's performance behavior is affected by the number of CAS before blocking.

### locks_test
We evaluate and compare the performance of following synchronizaiton primitives.

1. dummy-lock (which is simply a no-op)
2. CAS-lock
3. pthread_mutex_lock
4. futex-lock



## Getting Started
### Installation
```sh
./install.sh or
./debug_install.sh (for enabling -g compile option)
```
### Run
```sh
build/futex_backoff_test/futex_backoff_test 
build/locks_test/locks_test <# of threads>
```

<!-- CONTRIBUTING -->
## Contributing

We will humbly welcome your contributions!




## License

Distributed under the MIT License.



<!-- CONTACT -->
## Contact
6812skiii@gmail.com





