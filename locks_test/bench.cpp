#include "bench.h"

struct bench *bench{};

void init_bench(struct bench **bench, const unsigned int n_workers, const unsigned int duration) {
	(*bench) = new struct bench;
	(*bench)->duration = duration;
	(*bench)->n_workers = n_workers;
	(*bench)->workers = new struct worker[n_workers];
}

void uninit_bench(struct bench *bench) {
	delete [] bench->workers;
	delete bench;
}
