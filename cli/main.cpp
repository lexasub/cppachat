#include "cli.hpp"

int main(int argc, char **argv){
  std::cout << "ip: ";
  std::string ip;
  std::cin >> ip;
  std::cout << "port: ";
  int port;
  std::cin >> port;
  client_start(((ip == "d")?"127.0.0.1":ip).c_str(), (port == 0)?1000:port,  500); //500 - buflen 


}
