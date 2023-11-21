#include <sstream>
#include <cctype>
#include <cassert>
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
  // call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // call open_clientfd to connect to the server
  m_fd = Open_clientfd(hostname.c_str(), std::to_string(port).c_str());

  // call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  // close the socket if it is open
  if (is_open()) {
    Close(m_fd);
  }
}

bool Connection::is_open() const {
  // return true if the connection is open
  return m_fd != -1;
}

void Connection::close() {
  // close the connection if it is open
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;
  }
}

bool Connection::send(const Message &msg) {
  // send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  // Create a string representation of the message
  std::string message_string = msg.tag + ":" + msg.data + "\n";
  // Send the message
  ssize_t bytes_written = rio_writen(m_fd, message_string.c_str(), message_string.length());
  m_last_result = (bytes_written == static_cast<ssize_t>(message_string.length())) ? SUCCESS : EOF_OR_ERROR;

  return m_last_result == SUCCESS;
}

bool Connection::receive(Message &msg) {
  // receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  char buffer[MAXLINE];
  ssize_t bytes_read = Rio_readlineb(&m_fdbuf, buffer, MAXLINE);
  if (bytes_read <= 0) {
    m_last_result = bytes_read == 0 ? EOF_OR_ERROR : EOF_OR_ERROR;
    return false;
  }
  // Parse the message into tag and data
  std::istringstream iss(buffer);
  getline(iss, msg.tag, ':');
  getline(iss, msg.data);
  msg.data.pop_back(); // Remove the newline character
  m_last_result = SUCCESS;
  return true;
}
