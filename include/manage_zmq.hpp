#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <optional>
#include <zmq.hpp>

// Привязка и отвязка сокета к TCP-адресу
int Bind(zmq::socket_t *socket, int id);
void Unbind(zmq::socket_t *socket, int port);

// Подключение и отключение сокета по заданному порту
void Connect(zmq::socket_t *socket, int port);
void Disconnect(zmq::socket_t *socket, int port);

// Отправка и приём сообщений с обработкой ошибок
bool SendMessage(zmq::socket_t *socket, const std::string& msg);
std::optional<std::string> ReceiveMessage(zmq::socket_t *socket);
