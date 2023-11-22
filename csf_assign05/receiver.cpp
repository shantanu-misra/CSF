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
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room = argv[4];

  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }

  // send rlogin message
  Message rlogin_msg{"rlogin", username};
  if (!conn.send(rlogin_msg)) {
    std::cerr << "Failed to send rlogin message.\n";
    conn.close();
    return 1;
  }

  // Wait for server response after rlogin
  Message response;
  if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
    std::cerr << "rlogin failed: no response or error from server.\n";
    conn.close();
    return 1;
  }

  if (response.tag == "err") {
    std::cerr << "Error during rlogin: " << response.data << "\n";
    conn.close();
    return 1;
  }

  // send join message
  Message join_msg{"join", room};
  if (!conn.send(join_msg)) {
    std::cerr << "Failed to send join message.\n";
    conn.close();
    return 1;
  }

  // Wait for server response after join
  if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
    std::cerr << "Join failed: no response or error from server.\n";
    conn.close();
    return 1;
  }

  if (response.tag == "err") {
    std::cerr << "Error during join: " << response.data << "\n";
    conn.close();
    return 1;
  }

  // Main loop for reading messages from server
  while (true) {
    if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
      std::cerr << "Connection closed by server or error occurred.\n";
      conn.close();
      return 1;
    }

    if (response.tag == "err") {
      std::cerr << "Error: " << response.data << "\n";
      continue; // Continue listening for messages
    } else if (response.tag == "delivery") {
      std::cout << response.data << "\n"; // Print delivered message
    }
  }

  conn.close();
  return 0;
}
