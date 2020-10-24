#include <stdlib.h>

extern struct bench *bench;

struct worker {
	unsigned int works;
};

struct bench {
	int stop;
	unsigned int duration;
	unsigned int n_workers;
	struct worker* workers;
};

void init_bench(struct bench **bench, const unsigned int n_workers, const unsigned int duration);
void uninit_bench(struct bench *bench);
