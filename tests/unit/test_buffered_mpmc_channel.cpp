//
// Created by focus on 4/1/23.
//
#include "gtest/gtest.h"

#include <vector>

#include <channel.h>

#include <common/common.h>
#include <common/buffered_mpmc_executor.h>

class BufferedMPMCChannel : public ::testing::Test {

};

TEST_F(BufferedMPMCChannel, just_create_and_close) {
	sync_cpp::buffered_mpmc_channel<int> channel(2);
	channel.Close();

	CheckClose(channel);
}

TEST_F(BufferedMPMCChannel, just_simple_data_race) {
	sync_cpp::buffered_mpmc_channel<int> channel(2);

	CheckDataRace(channel);

	channel.Close();
}

TEST_F(BufferedMPMCChannel, just_simple_copy) {
	sync_cpp::buffered_mpmc_channel<std::vector<int>> channel(2);
	CheckCopy(channel);

	channel.Close();
}

TEST_F(BufferedMPMCChannel, simple_run) {
	BufferedMPMCExecutor<int> ex(1, 1, 2);
	ex();
}

TEST_F(BufferedMPMCChannel, senders_2) {
	BufferedMPMCExecutor<int> ex(2, 1, 2);
	ex();
}

TEST_F(BufferedMPMCChannel, recv_2) {
	BufferedMPMCExecutor<int> ex(1, 2, 2);
	ex();
}

TEST_F(BufferedMPMCChannel, size_2) {
	BufferedMPMCExecutor<int> ex(1, 1, 2);
	ex();
}

TEST_F(BufferedMPMCChannel, all_4) {
	BufferedMPMCExecutor<int> ex(4, 4, 4);
	ex();
}

TEST_F(BufferedMPMCChannel, senders_4_recv_8_buffer_2) {
	BufferedMPMCExecutor<int> ex(4, 8, 2);
	ex();
}

TEST_F(BufferedMPMCChannel, senders_32_recv_16_buffer_4) {
	BufferedMPMCExecutor<int> ex(32, 16, 4);
	ex();
}

TEST_F(BufferedMPMCChannel, senders_16_recv_32_buffer_4) {
	BufferedMPMCExecutor<int> ex(16, 32, 4);
	ex();
}