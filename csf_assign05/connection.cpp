#include <cstddef>
#include <iostream>
#include <sstream>
#include <cctype>
#include <cassert>
#include <string>
#include <unistd.h>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  //call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // call open_clientfd to connect to the server
  std::string port_str = std::to_string(port);
  m_fd = open_clientfd(hostname.c_str(), port_str.c_str());
  rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  Connection::close();
}

bool Connection::is_open() const {
  //return true if the connection is open
  return m_fd > 0;
}

void Connection::close() {
  ::close(m_fd);
}

bool Connection::send(const Message &msg) {
  //send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  //turn the tag into char*
  std::string message = msg.tag + ":" + msg.data;
  const char* charedMessage = message.c_str();
  int error = rio_writen(m_fd, charedMessage, message.length());

  //see if rio_returned error or eof
  if (error == -1) {
    m_last_result = EOF_OR_ERROR;
    return false;
  }

  //tag invalid if too long
  if (message.length() > MAXLINE) {
    m_last_result = INVALID_MSG;
    return false;
  }
  m_last_result = SUCCESS;
  return true;
}

bool Connection::receive(Message &msg) {
  // receive a message, storing its tag and data in msg
  char buf[MAXBUF];
  ssize_t n = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
  std::string buffeds = std::string(buf);
  int colon_index = buffeds.find(":");
  //invalid if no colon to seperate the tag
  if (colon_index == -1) {
    m_last_result = INVALID_MSG;
    return false;
  }
  //get the tag and data
  std::string tag = buffeds.substr(0, colon_index);
  msg.tag = tag;
  msg.data = buffeds.substr(colon_index + 1, buffeds.length() - 1);
  //invalid if eof 
  if (n <= 0) {
    m_last_result = INVALID_MSG;
    return false;
  }
  return true;
}