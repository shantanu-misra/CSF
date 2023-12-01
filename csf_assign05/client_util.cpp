#include <iostream>
#include <string>
#include "connection.h"
#include "message.h"
#include "client_util.h"

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}

int send_message(const std::string tag, Connection *connection, std::string data){
  Message message = Message(tag, data);
  int ret = 0;
  if(!connection->send(message)){
    if(tag == TAG_RLOGIN){
      std::cerr << "Failed to send rlogin message\n";
    }
    else if(tag == TAG_JOIN) {
      std::cerr << "Failed to send join message.\n";
    }
    else if(tag == TAG_SLOGIN) {
        std::cerr << "Failed to send login message.\n";
    }
    else{
      std::cerr << "Failed to send message.\n";
    }
    ret = 1;
  }
  Message response = Message();
  connection->receive(response);
  if (response.tag == TAG_ERR) {
    std::cerr << response.data;
    ret = 1;
  }
  return ret;
}

int handle_command(Connection *connection, std::string line) {
  if(line == "/leave") {
    send_message(TAG_LEAVE, connection, "\n");
    return 0;
  }
  else if(line == "/quit"){
    if(send_message(TAG_QUIT, connection, "\n") == 0){
      return 1;
    }
    else {
      return 0;
    }
  }
  else{ 
    int spaceIndex = line.find(" ");
    if(spaceIndex == -1) {
      std::cerr << "invalid input\n";
      return 0;
    }
    std::string join = line.substr(0, spaceIndex);
    std::string room = line.substr(spaceIndex+1, line.length());
    if(join == "/join") {
      send_message(TAG_JOIN, connection, room + "\n");
    }
    else{
      std::cerr << "invalid input\n";
    }
  }
  return 0;
}

void handle_delivery(std::string line) {
  size_t colon_index = line.find(":");
  // get the room
  std::string room = line.substr(0, colon_index);
  // get the data after the room name
  std::string next = line.substr(colon_index + 1, line.length() - 1);
  colon_index = next.find(":");
  // get the sender and the message
  std::string sender = next.substr(0, colon_index);
  std::string message = next.substr(colon_index + 1, next.length());
  std::cout << sender + ": " + message;
}
