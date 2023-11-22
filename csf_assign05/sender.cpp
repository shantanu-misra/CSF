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

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // connect to server
  Connection conn; // Using the Connection class
  conn.connect(server_hostname, server_port); // Connect to the server
  if (!conn.is_open()) {
    std::cerr << "Failed to connect to the server.\n";
    return 1;
  }

  // send slogin message
  Message slogin_msg;
  slogin_msg.tag = "slogin";
  slogin_msg.data = username;
  if (!conn.send(slogin_msg)) {
    std::cerr << "Failed to send login message.\n";
    return 1;
  }

  // Wait for server response after slogin
  Message response;
  if (!conn.receive(response) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
    std::cerr << "Login failed: no response or error from server.\n";
    return 1;
  }
  if (response.tag == "err") {
    std::cerr << "Error during login: " << response.data << "\n";
    return 1;
  }

  // Main loop for reading commands and sending messages
  std::string line;
  while (getline(std::cin, line)) {
    trim(line); // Function to remove leading and trailing whitespace, make sure it is defined
    if (line.empty()) continue;

    Message msg;
    if (line[0] == '/') { // Command
      std::string cmd = line.substr(1);
      size_t spaceIndex = cmd.find(' ');
      msg.tag = spaceIndex != std::string::npos ? cmd.substr(0, spaceIndex) : cmd; // Extract the command
      msg.data = spaceIndex != std::string::npos ? cmd.substr(spaceIndex + 1) : ""; // Extract the data if present

      if (msg.tag == "quit") {
        if (!conn.send(msg)) {
          std::cerr << "Failed to send quit command.\n";
          break;
        }
        // Wait for quit ack before closing
        if (!conn.receive(msg) || msg.tag != "ok") {
          std::cerr << "Quit not acknowledged by server.\n";
          break;
        }
        break;
      }
    } else { // Regular message
      msg.tag = "sendall";
      msg.data = line;
    }

    // Send the message to the server
    if (!conn.send(msg)) {
      std::cerr << "Failed to send message: " << line << "\n";
      break; // Exit the loop if send fails
    }

    // Wait for server response
    if (!conn.receive(msg) || conn.get_last_result() == Connection::EOF_OR_ERROR) {
      std::cerr << "Server error or connection closed.\n";
      break;
    }

    // Check if the server response is an error
    if (msg.tag == "err") {
      std::cerr << "Error: " << msg.data << "\n";
      continue; // Continue to process new commands
    } else if (msg.tag != "ok") {
      std::cerr << "Unexpected response from server: " << msg.data << "\n";
      continue; // Continue to process new commands
    }

    // If an ok response is received, continue to process new commands
  }

  conn.close(); // Close the connection
  return 0; // Successful exit
}
