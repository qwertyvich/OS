#include <unistd.h>
#include <sys/types.h>
#include <sstream>
#include "tree.hpp"
#include <thread>
#include <chrono>

std::string Node::Ping(int _id) {
    std::string ans = "Ok:0";
    if (_id == id) {
        ans = "Ok:1";
        return ans;
    } else if (auto it = children.find(_id); it != children.end()) {
        std::string msg = "ping " + std::to_string(_id);
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            SendMessage(it->second.get(), msg);
        }
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            if (auto msg_resp = ReceiveMessage(children[_id].get());
                msg_resp.has_value() && *msg_resp == "Ok:1") {
                ans = *msg_resp;
            }
        }
        return ans;
    }
    return ans;
}

std::string Node::Create(int idChild, const std::string& programPath) {
    int newPort = 4040 + idChild;
    int pid = fork();
    
    if (pid == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        execl(programPath.c_str(),
              programPath.c_str(),
              std::to_string(idChild).c_str(),
              std::to_string(newPort).c_str(),
              nullptr);
        _exit(1);
    } else {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        auto childSocket = std::make_unique<zmq::socket_t>(context, ZMQ_REQ);
        Connect(childSocket.get(), newPort);
        children[idChild] = std::move(childSocket);
        childrenPort[idChild] = newPort;
        children[idChild]->set(zmq::sockopt::sndtimeo, 3000);
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            if (!SendMessage(children[idChild].get(), "pid")) {
                return "Error: failed to send pid request";
            }
        }
        std::string pidChild = "Error: couldn't connect to child";
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            if (auto msg = ReceiveMessage(children[idChild].get()); msg.has_value()) {
                pidChild = *msg;
            }
        }
        return "Ok:" + pidChild;
    }
}

std::string Node::Pid() {
    return std::to_string(getpid());
}

std::string Node::Send(const std::string& str, int id) {
    if (children.empty()) {
        return "Error: Not found";
    } else if (auto it = children.find(id); it != children.end()) {
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            if (SendMessage(it->second.get(), str)) {
                std::string ans = "Error: Not found";
                if (auto msg = ReceiveMessage(children[id].get()); msg.has_value()) {
                    ans = *msg;
                }
                return ans;
            }
        }
    } else {
        std::string ans = "Error: Not found";
        for (auto& child : children) {
            std::string msg = "send " + std::to_string(id) + " " + str;
            {
                std::lock_guard<std::mutex> lock(childrenSocketMutex);
                if (SendMessage(child.second.get(), msg)) {
                    if (auto msg_resp = ReceiveMessage(child.second.get()); msg_resp.has_value()) {
                        ans = *msg_resp;
                    }
                }
            }
        }
        return ans;
    }
    return "Error: Not found";
}

std::string Node::Kill() {
    std::string ans;
    for (auto& child : children) {
        std::string msg = "kill";
        {
            std::lock_guard<std::mutex> lock(childrenSocketMutex);
            if (SendMessage(child.second.get(), msg)) {
                if (auto tmp = ReceiveMessage(child.second.get()); tmp.has_value()) {
                    msg = *tmp;
                }
            }
        }
        Unbind(child.second.get(), childrenPort[child.first]);
        child.second->close();
        if (!ans.empty()) {
            ans += " " + msg;
        } else {
            ans = msg;
        }
    }
    children.clear();
    childrenPort.clear();
    return ans;
}

void Node::HeartbeatSender() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(heartbeatInterval));
        std::string heartbeatMsg = "heartbeat " + std::to_string(id);
        // Отправляем heartbeat каждому дочернему узлу
        for (auto& child : children) {
            {
                std::lock_guard<std::mutex> lock(childrenSocketMutex);
                if (SendMessage(child.second.get(), heartbeatMsg)) {
                    // Сразу после отправки ждем ответ, чтобы завершить цикл REQ/REP
                    auto reply = ReceiveMessage(child.second.get());
                    if (!reply.has_value()) {
                        std::cerr << "Heartbeat: no reply from child " << std::endl;
                    }
                }
            }
        }
    }
}

void Node::HeartbeatReceiver() {
    while (running) {
        std::string message;
        {
            std::lock_guard<std::mutex> lock(parentSocketMutex);
            if (auto msg = ReceiveMessage(&parent); msg.has_value()) {
                message = *msg;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
        }
        std::istringstream iss(message);
        std::string cmd;
        iss >> cmd;
        if (cmd == "heartbeat") {
            int senderId;
            iss >> senderId;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Node::setKeyValue(const std::string& key, int value) {
    std::lock_guard<std::mutex> lock(mtx);
    keyValueStore[key] = value;
}

std::optional<int> Node::getKeyValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = keyValueStore.find(key);
    if (it != keyValueStore.end()) {
        return it->second;
    }
    return std::nullopt;
}
