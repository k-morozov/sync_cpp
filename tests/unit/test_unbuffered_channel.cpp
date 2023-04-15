//
// Created by focus on 4/3/23.
//
#include "gtest/gtest.h"

#include <vector>

#include <common/common.h>
#include <common/unbuffered_executor.h>

class UnbufferedChannel : public ::testing::Test {

};

TEST_F(UnbufferedChannel, just_create_and_close) {
	sync_cpp::unbuffered_channel<int> channel;
	channel.Close();

	CheckClose(channel);
}

TEST_F(UnbufferedChannel, just_simple_data_race) {
	sync_cpp::unbuffered_channel<int> channel;

	CheckDataRace(channel);

	channel.Close();
}

TEST_F(UnbufferedChannel, just_simple_copy) {
	sync_cpp::buffered_channel<std::vector<int>> channel(1);
	CheckCopy(channel);

	channel.Close();
}

TEST_F(UnbufferedChannel, simple_run) {
	UnbufferedExecutor<int> ex(1, 1);
	ex();
}

TEST_F(UnbufferedChannel, senders_2) {
	UnbufferedExecutor<int> ex(2, 1);
	ex();
}

TEST_F(UnbufferedChannel, recv_2) {
	UnbufferedExecutor<int> ex(1, 2);
	ex();
}

TEST_F(UnbufferedChannel, all_4) {
	UnbufferedExecutor<int> ex(4, 4);
	ex();
}

TEST_F(UnbufferedChannel, senders_4_recv_8) {
	UnbufferedExecutor<int> ex(4, 8);
	ex();
}

TEST_F(UnbufferedChannel, senders_32_recv_16) {
	UnbufferedExecutor<int> ex(32, 16);
	ex();
}

TEST_F(UnbufferedChannel, senders_16_recv_32) {
	UnbufferedExecutor<int> ex(16, 32);
	ex();
}