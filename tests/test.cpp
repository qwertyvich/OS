#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <parent.hpp>

// функция для чтения файла в строку
static std::string readFileContent(const std::string &filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(MmapLabTest, BasicCheck) {
    // пути к исполняемым файлам child1/child2 (из переменных окружения)
    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");
    ASSERT_NE(pathToChild1, nullptr) << "CHILD1_PATH is not set";
    ASSERT_NE(pathToChild2, nullptr) << "CHILD2_PATH is not set";

    std::string file1 = "test_out1.txt"; // временные файлы
    std::string file2 = "test_out2.txt";
    std::remove(file1.c_str());
    std::remove(file2.c_str());

    //вход. данные для parent process
    std::istringstream input_stream(
        file1 + "\n" +                  // сразу передаю имя файлов
        file2 + "\n" +
        "Hello\n" +
        "World\n" +
        "AEIOU\n" +
        "xyz\n"
        "\n"  // завершил ввод
    );

    // запускаю родителя, передав поток
    ParentProcess(pathToChild1, pathToChild2, input_stream);

    // читаю что оказалось в файлах
    std::string c1 = readFileContent(file1);
    std::string c2 = readFileContent(file2);
    std::string total = c1 + c2;
    //проверка на удаление гласных
    EXPECT_TRUE(total.find("Hll")  != std::string::npos);
    EXPECT_TRUE(total.find("Wrld") != std::string::npos);

    for (char v : std::string("AEIOUaeiou")) {
        EXPECT_EQ(total.find(v), std::string::npos);
    }
    EXPECT_TRUE(total.find("xyz") != std::string::npos);
    std::remove(file1.c_str());
    std::remove(file2.c_str());
}

// если ввод пустлй (проверка)
TEST(MmapLabTest, EmptyInput) {
    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");
    ASSERT_NE(pathToChild1, nullptr);
    ASSERT_NE(pathToChild2, nullptr);

    // Два временных файла
    std::string file1 = "test_out1.txt";
    std::string file2 = "test_out2.txt";
    std::remove(file1.c_str()); // c_str() - преобразование объекта std::string в указатель const char*
    std::remove(file2.c_str()); // тк remove принимает строку в си формате

    std::istringstream input_stream( // строковой поток ввода +  eof
        file1 + "\n" +
        file2 + "\n"
    );

    ParentProcess(pathToChild1, pathToChild2, input_stream);

    // Проверяем, что оба файла пусты
    std::string c1 = readFileContent(file1);
    std::string c2 = readFileContent(file2);
    EXPECT_TRUE(c1.empty());
    EXPECT_TRUE(c2.empty());

    std::remove(file1.c_str());
    std::remove(file2.c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
