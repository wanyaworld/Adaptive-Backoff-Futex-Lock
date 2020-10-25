#include <stdlib.h>
#include <string>

extern struct bench *bench;

struct worker {
	unsigned long long int works;
	unsigned long long int heavy_works;
};

struct bench {
	int stop;
	unsigned int duration;
	unsigned int n_workers;
	struct worker* workers;
};

void init_bench(struct bench **bench, const unsigned int n_workers, const unsigned int duration);
void uninit_bench(struct bench *bench);

struct lock_bench_instance {
	void (*lock)(int*);
	void (*unlock)(int*);
	int *lock_var;
	int *lock_var_heavy;
	std::string lock_name;
};
