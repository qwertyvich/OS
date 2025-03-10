#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <optional>
#include <memory>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "manage_zmq.hpp"

// Узел включает функциональность локального словаря (exec-команды типа 2)
// и механизм heartbeat для проверки доступности
class Node {
private:
    zmq::context_t context;
    std::map<std::string, int> keyValueStore;
    std::atomic<bool> running;
    std::thread heartbeatThread;
    int heartbeatInterval;
    mutable std::mutex mtx;
    std::condition_variable cv;
    std::mutex parentSocketMutex;
    std::mutex childrenSocketMutex;
    std::unordered_map<int, std::chrono::steady_clock::time_point> lastHeartbeat;
    std::thread heartbeatCheckerThread;

public:

    std::unordered_map<int, std::unique_ptr<zmq::socket_t>> children;
    std::unordered_map<int, int> childrenPort;
    int id;
    zmq::socket_t parent;
    int parentPort;
    void updateHeartbeat(int senderId) {
        std::lock_guard<std::mutex> lock(mtx);
        lastHeartbeat[senderId] = std::chrono::steady_clock::now();
    }

    void StartHeartbeatChecker(int timeInterval) {
        heartbeatCheckerThread = std::thread([this, timeInterval]() {
            while (running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                auto now = std::chrono::steady_clock::now();
                for (auto& [childId, lastTime] : lastHeartbeat) {
                    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime).count();
                    if (diff > 4 * timeInterval) {
                        std::cout << "Heartbit: node " << childId << " is unavailable now" << std::endl;
                    }
                }
            }
        });
    }

    Node(int _id, int _parentPort = -1)
        : id(_id), parent(context, ZMQ_REP), parentPort(_parentPort), running(true), heartbeatInterval(2000) {
        if (_id != -1) {
            Connect(&parent, _parentPort);
        }
        heartbeatThread = std::thread(&Node::HeartbeatSender, this);
    }

    ~Node() {
        running = false;
        cv.notify_all();
        if (heartbeatThread.joinable()) {
            heartbeatThread.join();
        }
        if (heartbeatCheckerThread.joinable()) {
            heartbeatCheckerThread.join();
        }
    }

    // Команды узла
    std::string Ping(int _id);
    std::string Create(int idChild, const std::string& programPath);
    std::string Pid();
    std::string Send(const std::string& str, int _id);
    std::string Kill();
    void SetHeartbeatInterval(int interval) {
        std::lock_guard<std::mutex> lock(mtx);
        heartbeatInterval = interval;
    }
    
    // Heartbeat
    void HeartbeatSender();
    void HeartbeatReceiver();

    // Методы для работы со словарём (exec-команды)
    void setKeyValue(const std::string& key, int value);
    std::optional<int> getKeyValue(const std::string& key) const;
};
