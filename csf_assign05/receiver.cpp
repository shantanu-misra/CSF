#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port_num = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room = argv[4];

  Connection conn;

  conn.connect(server_hostname, server_port_num);
  // check if connection falied
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }
  // send the login message and check if failed
  if (send_message(TAG_RLOGIN, &conn, username + "\n") != 0) {
    conn.close();
    return 1;
  }

//Sends join message and waits for join and responce
  if (send_message(TAG_JOIN, &conn, room + "\n") != 0) {
    conn.close();
    return 1;
  }

  // Main loop for reading messages from server
  Message main_messages = Message();
  while (conn.is_open() && conn.receive(main_messages)) {
    if(main_messages.tag == TAG_DELIVERY  && conn.get_last_result() == Connection::SUCCESS) {
      handle_delivery(main_messages.data);
    }
    else if(main_messages.tag == TAG_LEAVE) {
      conn.close();
      return 0;
    }
    else if(main_messages.tag == TAG_ERR){
      std::cerr << main_messages.data;
    }
    else if(conn.get_last_result() == Connection::EOF_OR_ERROR) {
      conn.close();
      std::cerr << "EOF or error\n";
      return 1;
    }
  }
  return 0;
}