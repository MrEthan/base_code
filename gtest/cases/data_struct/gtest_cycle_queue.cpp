//
// Created by wxm_e on 2021/12/18.
// 环形缓冲区单元测试
//

#include "data_struct/cycle_queue.h"
#include "gtest/gtest.h"

namespace {
// To use a test fixture, derive a class from testing::Test.
class CycleQueueTest : public testing::Test {
  protected: // You should make the members protected s.t. they can be
             // accessed from sub-classes.
    static void SetUpTestSuite() {}

    static void TearDownTestSuit() {}
    // virtual void SetUp() will be called before each test is run.  You
    // should define it if you need to initialize the variables.
    // Otherwise, this can be skipped.
    void SetUp() override
    {
        q_size = 8;
        q      = cycle_queue_create(q_size);
        ASSERT_TRUE(q != NULL);
    }

    // virtual void TearDown() will be called after each test is run.
    // You should define it if there is cleanup work to do.  Otherwise,
    // you don't have to provide it.
    //
    void TearDown()
    {
        cycle_queue_destroy(q);
        q = NULL;
    }

    // Declares the variables your tests want to use.
    CycleQueue *q;
    uint32_t q_size;
};
// When you have a test fixture, you define a test using TEST_F
// instead of TEST.

// 测试基本读写功能
TEST_F(CycleQueueTest, CycleQueueCase1)
{
    uint8_t buf_write[128] = {"hello"};
    uint32_t len_write     = strlen((char *)buf_write) + 1;
    GTEST_ASSERT_EQ(q_size - 1, q->get_free_size(q));
    GTEST_ASSERT_EQ(0, q->write(q, buf_write, len_write));        // 写测试
    GTEST_ASSERT_EQ(q_size - 1 - len_write, q->get_free_size(q)); // 测试剩余空间计数
    GTEST_ASSERT_EQ(len_write, q->get_data_size(q));              // 测试有效数据计数
    uint8_t buf[128];
    uint32_t len_read = sizeof(buf);
    memset(buf, 0, len_read);
    GTEST_ASSERT_EQ(0, q->read(q, buf, &len_read));       // 读测试
    GTEST_ASSERT_EQ(0, memcmp(buf, buf_write, len_read)); // 读写内容测试
    GTEST_ASSERT_EQ(len_write, len_read);                 // 读长度计数测试
    GTEST_ASSERT_EQ(q_size - 1, q->get_free_size(q));
    GTEST_ASSERT_EQ((uint32_t)0, q->get_data_size(q));
}

// 写满测试
TEST_F(CycleQueueTest, CycleQueueTest2)
{
    // 待写入数据超过缓冲区最大空间 应该写不进去，且返回-1
    uint8_t buf_write[128] = {"123456789"};
    uint32_t len_write     = strlen((char *)buf_write) + 1;
    GTEST_ASSERT_EQ(-1, q->write(q, buf_write, len_write)); // 写测试
    GTEST_ASSERT_EQ(q_size - 1, q->get_free_size(q));
    GTEST_ASSERT_EQ((uint32_t)0, q->get_data_size(q));
}

// 循环读写测试
TEST_F(CycleQueueTest, CycleQueueTest3)
{
    // 待写入数据超过缓冲区最大空间 应该写不进去，且返回-1
    uint8_t buf_write[128] = {"1234567"};
    uint8_t buf_read[128]  = {0};
    uint32_t read_len      = 0;
    for (uint32_t i = 1; i < q_size - 1; i++) {
        GTEST_ASSERT_EQ(0, q->write(q, buf_write, i));
        read_len = sizeof(buf_read);
        GTEST_ASSERT_EQ(0, q->read(q, buf_read, &read_len));
        GTEST_ASSERT_EQ(read_len, i);
        GTEST_ASSERT_EQ(0, memcmp(buf_write, buf_read, i));
    }
}

} // namespace