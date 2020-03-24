#include "cli.hpp"
namespace chat{
  std::mutex mtx;
  int n_msg_buf;
}
void recvmg(int my_sock) {
  int len;
  char *msg = new char(chat::n_msg_buf);//get max len msg
  while((len = recv(my_sock, msg, chat::n_msg_buf, 0)) > 0) {
    msg[len] = '\0';
    std::cout << msg;
  }
  delete[] msg;
}

inline bool client_init(const char ip[], const int port, int &sock){
  sockaddr_in srvip;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  srvip.sin_port = htons(port);
  srvip.sin_family= AF_INET;
  srvip.sin_addr.s_addr = inet_addr(ip);
  if((connect(sock, (sockaddr *)&srvip, sizeof(srvip))) == -1){
    std::cerr << "connection to socket failed" << std::endl;
    return false;
  }
#ifdef debug
  else std::cout << "conected to socket(" << sock << ")" << std::endl;
#endif 
  std::thread tr(recvmg, sock);
  tr.detach();
  return true;
}

void client_start(const char ip[], const int port, const int n_msg_buf) {
  int sock;
  chat::n_msg_buf = n_msg_buf;
  if(!client_init(ip, port, sock)) return;
  char *send_msg = new char (n_msg_buf);
  std::string str;
  int len;
  while(true) {//eq while true
    std::cin >> std::setw(n_msg_buf) >> str;
    const char *tmp = str.c_str();
    const char *tmp2 = tmp; 
    while(len = write(sock, tmp2, strlen(tmp2)) < str.length()) tmp2 += len;//отправляем серву если он не смог принять
      
    if(len < 0)
      std::cerr << "message not sent" << std::endl;
#ifdef debug
    else std::cout << "len: " << len  << " message {" << str << "} sended" << std::endl;
#endif 
  }
  close(sock);
}
