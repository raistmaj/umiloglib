#include "umilog.hpp"
#include <gtest/gtest.h>

TEST(basic_check, test_eq) {
    EXPECT_EQ(1, 1);
    umi::log::logger_local_data loggerData("localhost", 1, true, umi::log::facility::Local_Use_0,
                                           umi::log::severity::Debug);
    std::vector<umi::log::connection> loggerConnection{
            umi::log::connection(umi::log::connection::connection_type::UDP, "10.17.35.125", 5140, std::string())};
    umi::log::logger log(loggerData, loggerConnection);
    log.log(umi::log::facility::Local_Use_0, umi::log::severity::Error, "Test", "AAA", "Hello %d my dear friend %s", 11,
            "jose");
    sleep(2);
}
