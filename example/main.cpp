//
// Created by focus on 4/4/23.
//

#include <iostream>

#include <channel.h>

int main() {
	sync_cpp::buffered_mpmc_channel<int> chan(2);
	chan << 17;

	std::optional<int> value;
	chan >> value;
	std::cout << value.value_or(-1) << std::endl;
}