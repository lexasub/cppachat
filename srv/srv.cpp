#include "srv.hpp"
namespace chat{
  std::mutex mtx;
  std::vector<int> clients;
  int n_msg_buf;
  const char ext[] = "exit";
}

void sendtoall(const char *msg, const int &curr) {
  std::lock_guard<std::mutex> lg(chat::mtx);
  int len;
  if(!strcmp(msg, "exit") && (curr != -1))
    return;//не могут отправлять юзеры
  for(std::vector<int>::iterator it = chat::clients.begin(); it != chat::clients.end(); ++it)
    if(*it != curr) {
      std::cout << "[" <<  *it << " " << msg << " " << strlen(msg) << "]" << std::endl;

      if((len = write(*it, msg, strlen(msg))) < 0) 
        std::cerr << "(sending to all) " << len << " symbols send from client[" << curr
                  << "] to client[" << *it << "]" 
                  << " message:{" << msg << "} failed" << std::endl;
#ifdef debug
      else
        std::cout << "(sending to all) " << len << " symbols send from client[" <<  curr
                  << "] to client[" << *it << "]"
                  << " message:{" << msg << "} successed" << std::endl;
#endif
    }
}

void recvmg(const int &client_sock){
  int sock = client_sock;
  char *msg = new char[chat::n_msg_buf];//get max msg len
  int len;
  while((len = recv(sock, msg, chat::n_msg_buf, 0)) > 0) {
    msg[len] = '\0';
#ifdef debug
    std::cout << "(recieved) from:" << client_sock << " len:" << len << " message:{" << msg << "}" << std::endl;
#endif
    if(msg[0] == '*')//to any
      sendtoall(msg + 1, sock); 
    else if(msg[0] == 's')
      runcmd(std::string(msg + 1), sock);
    else if(msg[0] == 'c')
      send(msg + 1, sock);
    else
      std::cerr << "wrong msg format" << std::endl;
  }
  delete[] msg;
}

void shell(){
  std::string st;
  while(1){
    std::cout << "$> ";
    std::cin >> st;
    if(st == "exit"){
      for(std::vector<int>::iterator it = chat::clients.begin(); it != chat::clients.end(); ++it){
        sendtoall(chat::ext, -1);
      }
      exit(0);
    }
    if(st[0] == '*')//to any
      sendtoall(st.substr(1).c_str(), -1);
    else if(st[0] == 'c')
      send(st.substr(1).c_str(), -1);
    else {
      runcmd(st, -1);
    }
  } 
}

void runcmd(const std::string cmd, const int &sock) {
  
  
}

void send(const char text[], const int &sock) {
  std::string str(text);
  int i = str.find('$');
  if(i == std::string::npos){
    std::cerr << "wrong format for id" << std::endl;
    return;
  }
  std::string uid = str.substr(0, i);//determiner of id(may be nick, may be ip, may be id)
  std::string msg = str.substr(i);
  int id = toid(uid);
  int len;
  if((len = write(id, msg.c_str(), msg.length())) < 0)
    std::cerr << "(sending) " << len << " symbols from client[" << sock << "] to client[" << id << "] message{" << msg << "} failed" << std::endl;
  else
    std::cout << "(sending) " << len << " symbols from client[" << sock << "] to client[" << id << "] message{" << msg << "}" << std::endl;
}

int toid(std::string &str){
  return std::stoi(str);//пока что так, надо будет добавить юзернеймы
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
    if((client_sock = accept(sock, (sockaddr *)nullptr, nullptr)) < 0)
      std::cerr << "accept failed" << std::endl;
    chat::mtx.lock();
    std::cout << "(new connection) " << client_sock << std::endl; 
    chat::clients.push_back(client_sock);
    std::thread th(recvmg, client_sock), th1(shell);
    chat::mtx.unlock();
    th.detach();
    th1.detach();
  }
}


