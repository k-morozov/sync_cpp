//
// Created by focus on 4/1/23.
//

#include <benchmark/benchmark.h>

int main(int argc, char** argv) {
	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
	return 0;
}
