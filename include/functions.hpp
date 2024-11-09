#pragma once
#include <string>

#define READ_END 0 
#define WRITE_END 1 

void CreatePipe(int pipeFd[2]);
pid_t CreateChild();
void Exec(const char* pathToChild);
void processChild();
std::string removeVowels(const std::string& input);
