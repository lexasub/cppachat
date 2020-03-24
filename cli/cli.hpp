#pragma once
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <iomanip>
//#define debug

void recvmg(int my_sock);
std::string getmsg();
inline bool client_init(const char ip[], const int port, int &sock);
void client_start(const char ip[], const int port, const int n_msg_buf);
