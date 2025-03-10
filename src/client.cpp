#include <set>
#include <iostream>
#include <sstream>
#include <memory>
#include "../include/tree.hpp"
#include "../include/manage_zmq.hpp"
#include <cstdlib>

// export PROGRAM_PATH="/mnt/c/Users/user/Desktop/Labs/os/lr5-2/build/server"

int main() {
    std::set<int> Nodes;
    char* programPathC = getenv("PROGRAM_PATH");
    if (!programPathC) {
        std::cerr << "Error: PROGRAM_PATH environment variable not set" << std::endl;
        return 1;
    }
    std::string programPath = programPathC;
    Node task(-1);
    Nodes.insert(-1);
    std::string command;
    while (std::cin >> command) {
        if (command == "create") {
            int idChild, idParent;
            std::cin >> idChild >> idParent;
            if (Nodes.find(idChild) != Nodes.end()) {
                std::cout << "Error: Already exists" << std::endl;
            } else if (Nodes.find(idParent) == Nodes.end()) {
                std::cout << "Error: Parent not found" << std::endl;
            } else if (idParent == task.id) { // от управляющего узла
                std::string ans = task.Create(idChild, programPath);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            } else { // пересылаем команду родительскому узлу
                std::string str = "create " + std::to_string(idChild);
                std::string ans = task.Send(str, idParent);
                std::cout << ans << std::endl;
                Nodes.insert(idChild);
            }
        }  else if (command == "heartbit") {
            int newInterval;
            std::cin >> newInterval;
            task.SetHeartbeatInterval(newInterval);
            std::cout << "Ok" << std::endl;
        } else if (command == "ping") {
            int idChild;
            std::cin >> idChild;
            if (Nodes.find(idChild) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            } else if (task.children.find(idChild) != task.children.end()) {
                std::string ans = task.Ping(idChild);
                std::cout << ans << std::endl;
            } else {
                std::string str = "ping " + std::to_string(idChild);
                std::string ans = task.Send(str, idChild);
                if (ans == "Error: Not found") {
                    ans = "Ok:0";
                }
                std::cout << ans << std::endl;
            }
        } else if (command == "exec") {
            int id;
            std::cin >> id;
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            std::string name;
            std::cin >> name;
            std::string remainder;
            std::getline(std::cin, remainder);
            std::istringstream iss(remainder);
            int value;
            if (iss >> value) { // сохранение значения
                std::string msg = "exec " + std::to_string(id) + " " + name + " " + std::to_string(value);
                std::string ans = task.Send(msg, id);
                std::cout << ans << std::endl;
            } else { // загрузка значения
                std::string msg = "exec " + std::to_string(id) + " " + name;
                std::string ans = task.Send(msg, id);
                std::cout << ans << std::endl;
            }
        } else if (command == "kill") {
            int id;
            std::cin >> id;
            std::string msg = "kill";
            if (Nodes.find(id) == Nodes.end()) {
                std::cout << "Error: Not found" << std::endl;
            } else {
                std::string ans = task.Send(msg, id);
                if (ans != "Error: Not found") {
                    std::istringstream ids(ans);
                    int tmp;
                    while (ids >> tmp) {
                        Nodes.erase(tmp);
                    }
                    ans = "Ok";
                    if (task.children.find(id) != task.children.end()) {
                        Unbind(task.children[id].get(), task.childrenPort[id]);
                        task.children[id]->close();
                        task.children.erase(id);
                        task.childrenPort.erase(id);
                    }
                }
                std::cout << ans << std::endl;
            }
            
        } else if (command == "exit") {
            task.Kill();
            return 0;
        }
        
    }
    return 0;
}
