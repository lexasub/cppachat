#include "srv.hpp"

int main(){
  std::cout << "ip: ";
  std::string ip;
  std::cin >> ip;
  std::cout << "port: ";
  int port;
  std::cin >> port;
  server_start(((ip == "d")?"127.0.0.1":ip).c_str(), (port == 0)?1000:port, 500);
}
