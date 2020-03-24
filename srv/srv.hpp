#pragma once
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <mutex>
#include <thread>
#include <tuple>
#include <unistd.h>
//#define debug

void sendtoall(const char *msg, const int &curr);
void recvmg(const int &client_sock);
void runcmd(const std::string msg, const int &sock);
void send(const char text[], const int &sock);
int toid(std::string &str);
inline int server_init(const char ip[], const int port);
void server_start(const char ip[], const int port, const int n_msg_buf);
