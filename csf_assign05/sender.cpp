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
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }

  // send slogin message
  Message slogin_msg{"slogin", username};
  if (!conn.send(slogin_msg)) {
    std::cerr << "Failed to send login message.\n";
    conn.close();
    return 1;
  }

  // Wait for server response after slogin
  Message response;
  if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
    std::cerr << "Login failed: no response or error from server.\n";
    conn.close();
    return 1;
  }

  if (response.tag == "err") {
    std::cerr << "Error during login: " << response.data << "\n";
    conn.close();
    return 1;
  }

  // Main loop for reading commands and sending messages
  std::string line;
  while (getline(std::cin, line)) {
    trim(line);
    if (line.empty()) continue;

    Message msg;
    if (line[0] == '/') {
      std::string cmd = line.substr(1);
      size_t spaceIndex = cmd.find(' ');
      msg.tag = spaceIndex != std::string::npos ? cmd.substr(0, spaceIndex) : cmd;
      msg.data = spaceIndex != std::string::npos ? cmd.substr(spaceIndex + 1) : "";

      if (!conn.send(msg)) {
        std::cerr << "Failed to send command: " << msg.tag << "\n";
        conn.close();
        return 1;
      }

      if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
        std::cerr << "Error or disconnection after command: " << msg.tag << "\n";
        conn.close();
        return 1;
      }

      if (response.tag == "err") {
        std::cerr << "Error: " << response.data << "\n";
        conn.close();
        return 1; // Close and exit if any command error
      } else if (response.tag != "ok") {
        std::cerr << "Unexpected response from server: " << response.data << "\n";
        conn.close();
        return 1;
      }

      if (msg.tag == "quit") break; // Exit the loop after 'quit'

    } else {
      // Handling a regular message, not a command
      msg.tag = "sendall";
      msg.data = line;
      
      if (!conn.send(msg)) {
        std::cerr << "Failed to send message: " << line << "\n";
        conn.close();
        return 1; // Exit on send failure
      }

      // Regular messages do not expect a server response according to the flowchart
    }
  }

  conn.close(); // Close the connection
  return 0; // Successful exit
}
