#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions
struct ClientData {
  Connection* conn;
  Server* server;
  bool isSender;
  std::string username;

  // Modify constructor to accept a Server pointer
  ClientData(Connection* c, Server* s, bool sender, std::string user)
    : conn(c), server(s), isSender(sender), username(std::move(user)) {}

  ~ClientData() {
    delete conn; // Ensure to delete the connection when ClientData is destroyed
  }

  // Disable copy constructor and assignment operator
  ClientData(const ClientData&) = delete;
  ClientData& operator=(const ClientData&) = delete;
};



////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

void handle_sender(Connection* conn, Server* server, const std::string& username) {
  // handling a sender client
}

void handle_receiver(Connection* conn, Server* server, const std::string& username) {
  // handling a receiver client
}

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  auto clientData = static_cast<ClientData*>(arg);
  Connection* conn = clientData->conn;
  Server* server = clientData->server; // Use the server pointer from ClientData
  std::string username;

  // TODO: read login message (should be tagged either with
  //       TAG_SLOGIN or TAG_RLOGIN), send response
  Message loginMessage;
  if (conn->receive(loginMessage)) {
    username = loginMessage.data;  // Store username
    clientData->username = username; // Store username in ClientData as well
    // Determine if sender or receiver
    if (loginMessage.tag == TAG_SLOGIN) {
      clientData->isSender = true;
      conn->send(Message(TAG_OK, "Logged in as sender"));
      // TODO: Implement sender interaction loop
    } else if (loginMessage.tag == TAG_RLOGIN) {
      clientData->isSender = false;
      conn->send(Message(TAG_OK, "Logged in as receiver"));
      // TODO: Implement receiver interaction loop
    } else {
      conn->send(Message(TAG_ERR, "Invalid login tag"));
      delete clientData;
      return nullptr;
    }
  } else {
      // Handle error or EOF from receiver
      delete clientData;
      return nullptr;
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities
  //       is a good idea)
  if (clientData->isSender) {
    handle_sender(conn, server, username);
  } else {
    handle_receiver(conn, server, username);
  }

  delete clientData; // Cleanup
  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  pthread_mutex_init(&m_lock, nullptr);
}

Server::~Server() {
  // TODO: destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
  std::string portStr = std::to_string(m_port);
   m_ssock = open_listenfd(portStr.c_str());
  return m_ssock != -1;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while (true) {
    int clientfd = Accept(m_ssock, nullptr, nullptr);
    if (clientfd != -1) {
      Connection* conn = new Connection(clientfd);
      ClientData* clientData = new ClientData(conn, this, false, "");
      pthread_t tid;
      pthread_create(&tid, nullptr, worker, static_cast<void*>(clientData));
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard g(m_lock); // Locking to ensure thread safety
  auto it = m_rooms.find(room_name);
  if (it != m_rooms.end()) {
    return it->second;
  } else {
    Room *newRoom = new Room(room_name);
    m_rooms[room_name] = newRoom;
    return newRoom;
  }
}
