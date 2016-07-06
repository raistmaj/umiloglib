#include "umilog.hpp"
#include <gtest/gtest.h>

TEST(basic_check, test_eq) {
    EXPECT_EQ(1,0);
}

//int main(int argc, char** argv) {
//    umi::log::logger_local_data loggerData("localhost",1,true,umi::log::facility::Local_Use_0, umi::log::severity::Debug);
//    std::vector<umi::log::connection> loggerConnection;
//    umi::log::logger log(loggerData, loggerConnection);
//
//
//}