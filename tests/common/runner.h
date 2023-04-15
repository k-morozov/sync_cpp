//
// Created by focus on 4/9/23.
//

#pragma once

#include <cstdlib>

namespace test_space {

template<class T>
void run(T & counter, const size_t times) {
	for (size_t i=0; i<times; i++) {
		counter.Increment();
	}
}

}