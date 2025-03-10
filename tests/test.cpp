


// export PROGRAM_PATH="/mnt/c/Users/user/Desktop/Labs/os/lr5/build/server"
#include <gtest/gtest.h>
#include <set>
#include <cstdlib>
#include <string>
#include "../include/tree.hpp"
#include "../include/manage_zmq.hpp"

TEST(Variant48Tests, CreateAndPingTest) {
    std::string programPath = getenv("PROGRAM_PATH") ? getenv("PROGRAM_PATH") : "";
    ASSERT_FALSE(programPath.empty());
    Node root(-1);
    std::set<int> Nodes;
    Nodes.insert(-1);
    std::string createAns = root.Create(1, programPath);
    
    Nodes.insert(1);
    ASSERT_NE(createAns.find("Ok:"), std::string::npos);
    std::string pingAns = root.Send("ping 1", 1);
    EXPECT_EQ(pingAns, "Ok:1");
    pingAns = root.Send("ping 2", 2);
    EXPECT_EQ(pingAns, "Error: Not found");
    std::string killAns = root.Send("kill", 1);
    EXPECT_TRUE(killAns.find("Error") == std::string::npos);
}

TEST(Variant48Tests, ExecDictTest) {
    std::string programPath = getenv("PROGRAM_PATH") ? getenv("PROGRAM_PATH") : "";
    ASSERT_FALSE(programPath.empty());
    Node root(-1);
    std::set<int> Nodes;
    Nodes.insert(-1);
    std::string createAns = root.Create(10, programPath);
    Nodes.insert(10);
    ASSERT_NE(createAns.find("Ok:"), std::string::npos);
    std::string execAns = root.Send("exec 10 MyVar 5", 10);
    EXPECT_EQ(execAns, "Ok:10");
    execAns = root.Send("exec 10 MyVar", 10);
    EXPECT_EQ(execAns, "Ok:10: 5");
    execAns = root.Send("exec 10 MyVar 7", 10);
    EXPECT_EQ(execAns, "Ok:10");
    execAns = root.Send("exec 10 MyVar", 10);
    EXPECT_EQ(execAns, "Ok:10: 7");
    root.Kill();
}

TEST(Variant48Tests, MultipleNodesTest) {
    std::string programPath = getenv("PROGRAM_PATH") ? getenv("PROGRAM_PATH") : "";
    ASSERT_FALSE(programPath.empty());
    Node root(-1);
    std::set<int> Nodes;
    Nodes.insert(-1);
    std::string ans = root.Create(1, programPath);
    Nodes.insert(1);
    ASSERT_NE(ans.find("Ok:"), std::string::npos);
    ans = root.Send("create 2 1", 1);
    Nodes.insert(2);
    ASSERT_NE(ans.find("Ok:"), std::string::npos);
    ans = root.Send("ping 1", 1);
    EXPECT_EQ(ans, "Ok:1");
    ans = root.Send("ping 2", 2);
    EXPECT_EQ(ans, "Ok:1");
    ans = root.Send("exec 2 X 100", 2);
    EXPECT_EQ(ans, "Ok:2");
    ans = root.Send("exec 2 X", 2);
    EXPECT_EQ(ans, "Ok:2: 100");
    root.Kill();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
