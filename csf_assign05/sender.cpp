#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];

  // connect to server
  Connection conn;

  conn.connect(server_hostname, server_port);
  // check if connection falied
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }
  // send the login message and check if failed
  if (send_message(TAG_SLOGIN, &conn, username + "\n") != 0) {
    conn.close();
    return 1;
  }
 
  // Main loop for reading commands and sending messages
  std::string line;
  while (std::cin) {
    getline(std::cin, line);
    trim(line);
    //if (line.empty()) continue;
 if (line.at(0) != '/') {
      send_message(TAG_SENDALL, &conn, line + "\n");
      continue;
    }
    if(handle_command(&conn, line) != 0) {
      conn.close();
      return 0;
    }
  }
  return 0; // Successful exit
}