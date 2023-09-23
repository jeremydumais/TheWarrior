#include "messagePipeline.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace thewarrior::ui::models;

class MessagePipelineSample : public ::testing::Test
{
public:
    MessagePipelineSample()
    {
        pipeline.addMessage(std::make_shared<Message>("test1"));
        pipeline.addMessage(std::make_shared<Message>("test2"));
    }
    MessagePipeline pipeline;
};

TEST(MessagePipeline_DefaultContructor, ReturnSuccess)
{
    MessagePipeline pipeline;
    ASSERT_EQ(0, pipeline.getMessageCount());
}

TEST(MessagePipeline_AddMessage, WithValidMessage_ReturnSuccess)
{
    MessagePipeline pipeline;
    pipeline.addMessage(std::make_shared<Message>("test"));
    ASSERT_EQ(1, pipeline.getMessageCount());
}

TEST_F(MessagePipelineSample, getCurrentMessage_ReturnTest1)
{
    ASSERT_EQ("test1", pipeline.getCurrentMessage()->getMessage());
}

TEST_F(MessagePipelineSample, deleteCurrentMessage_ReturnSuccess)
{
    pipeline.deleteCurrentMessage();
    ASSERT_EQ("test2", pipeline.getCurrentMessage()->getMessage());
}
