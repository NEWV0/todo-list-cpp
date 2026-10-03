#include <gtest/gtest.h>

#include "task.h"

TEST(TaskTest, Constructor) {
    Task task("Buy milk");

    EXPECT_EQ(task.getDescription(), "Buy milk");
    EXPECT_FALSE(task.isCompleted());
    EXPECT_EQ(task.getPriority(), Priority::Medium);
    EXPECT_TRUE(task.getDeadline().isEmpty());
    EXPECT_TRUE(task.getTags().isEmpty());
}

TEST(TaskTest, ToggleComplete) {
    Task task("Task");

    task.toggleComplete();
    EXPECT_TRUE(task.isCompleted());

    task.toggleComplete();
    EXPECT_FALSE(task.isCompleted());
}

TEST(TaskTest, ImagePath) {
    Task task("Task");

    task.setImagePath("/tmp/image.png");

    EXPECT_EQ(task.getImagePath(), "/tmp/image.png");
}

TEST(TaskTest, Priority) {
    Task task("Task");

    task.setPriority(Priority::High);
    EXPECT_EQ(task.getPriority(), Priority::High);
    EXPECT_EQ(priorityToString(task.getPriority()), "High");

    task.setPriority(Priority::Low);
    EXPECT_EQ(task.getPriority(), Priority::Low);
    EXPECT_EQ(priorityToString(task.getPriority()), "Low");
}

TEST(TaskTest, PriorityParserIsCaseInsensitive) {
    EXPECT_EQ(priorityFromString("HIGH"), Priority::High);
    EXPECT_EQ(priorityFromString("high"), Priority::High);
    EXPECT_EQ(priorityFromString("LoW"), Priority::Low);
    EXPECT_EQ(priorityFromString("medium"), Priority::Medium);
}

TEST(TaskTest, Deadline) {
    Task task("Task");

    task.setDeadline("2026-10-10");

    EXPECT_EQ(task.getDeadline(), "2026-10-10");
}

TEST(TaskTest, Tags) {
    Task task("Task");

    const QStringList tags{"home", "shopping"};
    task.setTags(tags);

    EXPECT_EQ(task.getTags(), tags);
    EXPECT_EQ(task.getTags().size(), 2);
}
