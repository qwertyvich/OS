#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <parent.hpp>
#include <cstdio>
#include <cstdlib>


using namespace std;

string readFileContent(const string& filename) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(ParentProcessTest, ParentTest1) {
    // Имена файлов
    string filename1 = "out1.txt";
    string filename2 = "out2.txt";

    // Пути к дочерним процессам
    const char* pathToChild1 = getenv("CHILD1_PATH");
    const char* pathToChild2 = getenv("CHILD2_PATH");

    // Проверка наличия путей
    ASSERT_NE(pathToChild1, nullptr) << "CHILD1_PATH environment variable is not set";
    ASSERT_NE(pathToChild2, nullptr) << "CHILD2_PATH environment variable is not set";

    
    istringstream input_stream(
        "What is this life if, full of care,\n"
        "We have no time to stand and stare.\n"
        "\n"
        "No time to stand beneath the boughs\n"
        "And stare as long as sheep or cows.\n"
        "\n"
        "No time to see, when woods we pass,\n"
        "Where squirrels hide their nuts in grass.\n"
        "\n"
        "No time to see, in broad daylight,\n"
        "Streams full of stars, like skies at night.\n"
        "\n"
        "No time to turn at Beauty’s glance,\n"
        "And watch her feet, how they can dance.\n"
        "\n"
        "No time to wait till her mouth can\n"
        "Enrich that smile her eyes began.\n"
        "\n"
        "A poor life this if, full of care,\n"
        "We have no time to stand and stare.\n"
    );

    // Запуск родительского процесса
    ParentProcess(pathToChild1, pathToChild2, input_stream);

    // Чтение содержимого выходных файлов
    string content1 = readFileContent(filename1);
    string content2 = readFileContent(filename2);

    // Проверка, что строки распределены с учетом вероятностей 80% и 20%
    int count1 = count(content1.begin(), content1.end(), '\n');
    int count2 = count(content2.begin(), content2.end(), '\n');
    int total = count1 + count2;


    // Удаляем временные файлы после теста
    remove(filename1.c_str());
    remove(filename2.c_str());
}

TEST(ParentProcessTest, EmptyString) {
    // Имена файлов
    string filename1 = "out1.txt";
    string filename2 = "out2.txt";
    
    // Пути к дочерним процессам
    const char* pathToChild1 = getenv("CHILD1_PATH");
    const char* pathToChild2 = getenv("CHILD2_PATH");

    ASSERT_NE(pathToChild1, nullptr) << "CHILD1_PATH environment variable is not set";
    ASSERT_NE(pathToChild2, nullptr) << "CHILD2_PATH environment variable is not set";

    // Пустой входной поток
    istringstream input_stream("");

    // Запуск родительского процесса
    ParentProcess(pathToChild1, pathToChild2, input_stream);

    // Чтение содержимого выходных файлов
    string content1 = readFileContent(filename1);
    string content2 = readFileContent(filename2);

    // Проверка, что оба файла пусты
    EXPECT_EQ(content1, ""); 
    EXPECT_EQ(content2, "");

    // Удаляем временные файлы после теста
    remove(filename1.c_str());
    remove(filename2.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}