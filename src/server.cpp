#include "../include/tree.hpp"
#include "../include/manage_zmq.hpp"
#include <fstream>
#include <signal.h>
#include <map>
#include <thread>
#include <sstream>
#include <cstdlib>

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Not enough arguments");
        exit(EXIT_FAILURE);
    }
    int nodeId = atoi(argv[1]);
    int parentPort = atoi(argv[2]);
    Node task(nodeId, parentPort);
    Bind(&task.parent, nodeId);
    char* programPathC = getenv("PROGRAM_PATH");
    std::string programPath = (programPathC ? programPathC : "");
    
    while (true) {
        std::string message;
        if (auto msg = ReceiveMessage(&(task.parent)); msg.has_value()) {
            message = *msg;
        } else {
            continue;
        }
        std::istringstream request(message);
        std::string command;
        request >> command;
        if (command == "create") {
            int idChild;
            request >> idChild;
            std::string ans = task.Create(idChild, programPath);
            SendMessage(&task.parent, ans);
        } else if (command == "pid") {
            std::string ans = task.Pid();
            SendMessage(&task.parent, ans);
        } else if (command == "ping") {
            int idChild;
            request >> idChild;
            std::string ans = task.Ping(idChild);
            SendMessage(&task.parent, ans);
        } else if (command == "send") {
            int id;
            request >> id;
            std::string str;
            getline(request, str);
            if (!str.empty() && str[0] == ' ') {
                str.erase(0, 1);
            }
            std::string ans = task.Send(str, id);
            SendMessage(&task.parent, ans);
        } else if (command == "exec") {
            int targetId;
            request >> targetId;
            std::string name;
            request >> name;
            std::string remainder;
            std::getline(request, remainder);
            std::istringstream iss(remainder);
            int value;
            if (iss >> value) { // команда сохранения
                task.setKeyValue(name, value);
                std::string to_send = "Ok:" + std::to_string(targetId);
                SendMessage(&task.parent, to_send);
            } else { // команда загрузки
                auto valueOpt = task.getKeyValue(name);
                if (valueOpt.has_value()) {
                    std::string to_send = "Ok:" + std::to_string(targetId) + ": " + std::to_string(valueOpt.value());
                    SendMessage(&task.parent, to_send);
                } else {
                    std::string to_send = "Ok:" + std::to_string(targetId) + ": '" + name + "' not found";
                    SendMessage(&task.parent, to_send);
                }
            }
        } else if (command == "heartbeat") {
            int senderId;
            request >> senderId;  // Используем 'request', а не 'iss'
            task.updateHeartbeat(senderId);
            SendMessage(&task.parent, "Ok");
        } else if (command == "kill") {
            std::string ans = task.Kill();
            ans = std::to_string(task.id) + " " + ans;
            SendMessage(&task.parent, ans);
            Disconnect(&task.parent, task.parentPort);
            task.parent.close();
            break;
        }
    }
    return 0;
}
