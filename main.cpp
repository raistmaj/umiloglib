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

TEST(basic_check, test_eq_2) {
    using st_data_test = umi::log::structured_data;
    using st_sd_element = umi::log::structured_data::sd_element;
    EXPECT_EQ(1, 1);
    st_sd_element el1("HI"); el1.add_param("popo", "caca"); el1.add_param("papa","ruru");
    st_data_test data_test_1; data_test_1.add_element(el1);
    umi::log::logger_local_data loggerData("localhost", 1, true, umi::log::facility::Local_Use_0,
                                           umi::log::severity::Debug);
    std::vector<umi::log::connection> loggerConnection{
            umi::log::connection(umi::log::connection::connection_type::UDP, "10.17.35.125", 5140, std::string())};
    umi::log::logger log(loggerData, loggerConnection);

    log.log(umi::log::facility::Local_Use_0, umi::log::severity::Error, "Test", "AAA",
    data_test_1,
      "Hello %d my dear friend %s", 11,
    "jose");
    sleep(2);
}