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
  int server_port_num = std::stoi(server_port);
  conn.connect(server_hostname, server_port_num);
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
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
  if (!conn.receive(response) || response.tag == "err") {
    std::cerr << "Error during rlogin: " << response.data << "\n";
    return 1;
  }

  // Send join message
  Message join_msg{"join", room};
  if (!conn.send(join_msg)) {
    std::cerr << "Failed to send join message.\n";
    return 1;
  }

  // Wait for server response after join
  if (!conn.receive(response) || response.tag == "err") {
    std::cerr << "Error during join: " << response.data << "\n";
    return 1;
  }

  // Main loop for reading messages from server
while (true) {
  if (!conn.receive(response)) {
    std::cerr << "Connection closed by server or error occurred.\n";
    return 1; // Exiting with non-zero status due to error
  }

  if (response.tag == "delivery") {
    // Extracting the sender and message text from the response data
    std::istringstream iss(response.data);
    std::string room, sender, message_text;
    getline(iss, room, ':'); // Assuming the first token is the room, which we ignore
    getline(iss, sender, ':'); // The second token is the sender
    getline(iss, message_text); // The rest is the message text
    std::cout << "[" << sender << "]: " << message_text << "\n";
  } else if (response.tag == "err") {
    std::cerr << "Error: " << response.data << "\n";
    // If it's a join or login error, terminate the program
    if (response.data.find("rlogin:") != std::string::npos || 
      response.data.find("join:") != std::string::npos) {
      return 1; // Exiting with non-zero status due to error
    }
  }
}


  conn.close();
  return 0;
}
