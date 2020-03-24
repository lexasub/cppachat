#include "srv.hpp"
namespace chat{
  std::mutex mtx;
  std::vector<int> clients;
  int n_msg_buf;
}

void sendtoall(char msg[], int curr) {
  chat::mtx.lock();
  for(std::vector<int>::iterator it = chat::clients.begin(); it != chat::clients.end(); ++it)
    if(*it != curr)
      if(send(*it, msg, strlen(msg), 0) < 0)
        std::cerr << "send to client[" << std::distance(chat::clients.begin(), it) << "]"
          << "failed" << std::endl;
  chat::mtx.unlock();
}

void recvmg( int client_sock){
  int sock = client_sock;
  char *msg = new char[chat::n_msg_buf];//get max msg len
  int len;
  while((len = recv(sock, msg, chat::n_msg_buf , 0)) > 0) {
    msg[len] = '\0';
    sendtoall(msg, sock); 
  }
  delete[] msg;
}

inline int server_init(const char ip[], const int port) {
  sockaddr_in srvip;
  int sock;
  srvip.sin_family = AF_INET;srvip.sin_port = htons(port);
  srvip.sin_addr.s_addr = inet_addr(ip);
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(bind(sock, (sockaddr *)&srvip, sizeof(srvip)) == -1) {
    std::cerr << "cannot bind, error!!" << std::endl; 
    return 0;
  }
  else
    std::cout << "Server started" << std::endl;
  if(listen(sock, 20) == -1){
    std::cerr << "accept failed" << std::endl;
    return 0;
  }
  return sock;
}

void server_start(const char ip[], const int port, const int n_msg_buf) {
  chat::n_msg_buf = n_msg_buf;
  int i = port;
  int sock = server_init(ip, port);
  while(sock == 0) {++i; sock = server_init(ip, i);}
  std::cout << i << std::endl;
  int client_sock;
  while(1){
    std::cout << "test" << std::endl;
    if((client_sock = accept(sock, (sockaddr *)nullptr, nullptr)) < 0)
      std::cerr << "accept failed" << std::endl;
    std::cout << "test2" << std::endl;
    chat::mtx.lock();
    std::cout << "test3" << std::endl;
    chat::clients.push_back(client_sock);
    std::cout << "test4" << std::endl;
    std::thread th(recvmg, client_sock);
    chat::mtx.unlock();
    std::cout << "test5" << std::endl;
  }
}
