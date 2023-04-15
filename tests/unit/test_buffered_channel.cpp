//
// Created by focus on 4/1/23.
//
#include "gtest/gtest.h"

#include <vector>

#include <channel.h>

#include <common/common.h>
#include <common/buffered_executor.h>

class BufferedChannel : public ::testing::Test {

};

TEST_F(BufferedChannel, just_create_and_close) {
	sync_cpp::buffered_channel<int> channel(1);
	channel.Close();

	CheckClose(channel);
}

TEST_F(BufferedChannel, just_simple_data_race) {
	sync_cpp::buffered_channel<int> channel(1);

	CheckDataRace(channel);

	channel.Close();
}

TEST_F(BufferedChannel, just_simple_copy) {
	sync_cpp::buffered_channel<std::vector<int>> channel(1);
	CheckCopy(channel);

	channel.Close();
}

TEST_F(BufferedChannel, simple_run) {
	BufferedExecutor<int> ex(1, 1, 1);
	ex();
}

TEST_F(BufferedChannel, senders_2) {
	BufferedExecutor<int> ex(2, 1, 1);
	ex();
}

TEST_F(BufferedChannel, recv_2) {
	BufferedExecutor<int> ex(1, 2, 1);
	ex();
}

TEST_F(BufferedChannel, size_2) {
	BufferedExecutor<int> ex(1, 1, 2);
	ex();
}

TEST_F(BufferedChannel, all_4) {
	BufferedExecutor<int> ex(4, 4, 4);
	ex();
}

TEST_F(BufferedChannel, senders_4_recv_8_buffer_2) {
	BufferedExecutor<int> ex(4, 8, 2);
	ex();
}

TEST_F(BufferedChannel, senders_32_recv_16_buffer_4) {
	BufferedExecutor<int> ex(32, 16, 4);
	ex();
}

TEST_F(BufferedChannel, senders_16_recv_32_buffer_4) {
	BufferedExecutor<int> ex(16, 32, 4);
	ex();
}