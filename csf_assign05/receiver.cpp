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
  std::string server_port = argv[2];
  std::string username = argv[3];
  std::string room = argv[4];

  Connection conn;
  try {
    int server_port_num = std::stoi(server_port);
    conn.connect(server_hostname, server_port_num);
    if (!conn.is_open()) {
      std::cerr << "Failed to connect to the server.\n";
      return 1;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  // Send rlogin message
  Message rlogin_msg{"rlogin", username};
  if (!conn.send(rlogin_msg)) {
    std::cerr << "Failed to send rlogin message.\n";
    return 1;
  }

  // Wait for server response after rlogin
  Message response;
  if (!conn.receive(response)) {
    std::cerr << "Failed to receive response after rlogin.\n";
    return 1;
  }

  if (response.tag == "err") {
    std::cerr << response.data << "\n";
    return 1;
  }

  // Send join message
  Message join_msg{"join", room};
  if (!conn.send(join_msg)) {
    std::cerr << "Failed to send join message.\n";
    return 1;
  }

  // Wait for server response after join
  if (!conn.receive(response)) {
    std::cerr << "Failed to receive response after join.\n";
    return 1;
  }

  if (response.tag == "err") {
    std::cerr << response.data << "\n";
    return 1;
  }

  // Main loop for reading messages from server
  while (true) {
    if (!conn.receive(response)) {
      std::cerr << "Connection closed by server or error occurred.\n";
      return 1;
    }

    if (response.tag == "delivery") {
      std::istringstream iss(response.data);
      std::string sender, message_text;
      if (!(getline(iss, sender, ':') && getline(iss, message_text))) {
        std::cerr << "Failed to parse delivery message.\n";
        continue;
      }
      std::cout << sender << ": " << message_text << std::endl;
    } else if (response.tag == "err") {
      std::cerr << response.data << "\n";
      if (response.data.find("rlogin:") != std::string::npos || 
          response.data.find("join:") != std::string::npos) {
        return 1;
      }
    }
  }

  conn.close();
  return 0;
}
