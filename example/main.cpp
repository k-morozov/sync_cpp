//
// Created by focus on 4/4/23.
//

#include <iostream>

#include <atomic>

int main() {
	std::atomic<int> a{10};
	int b = 10;

	a.compare_exchange_weak(b, 2);
	std::cout << a << std::endl;
}