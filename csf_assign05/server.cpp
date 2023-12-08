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
  // Sender interaction loop
  while (true) {
    // Read messages from the sender
    // Determine the type of message and act accordingly
    // Use server->find_or_create_room to get the room object
    // Call room->broadcast_message to send the message to all receivers
    // Send a response back to the sender (either ok or err)
    // Break loop if the sender sends a quit message or disconnects
  }
  // Perform cleanup
}

void handle_receiver(Connection* conn, Server* server, const std::string& username) {
  // Receiver interaction loop
    while (true) {
      // Wait for a message to be available (use MessageQueue::dequeue)
      // Send the message to the receiver
      // Break loop if unable to send message or receiver sends a quit message
    }
    // Perform cleanup if necessary
}

void *worker(void *arg) {
  // Cast the void* arg to a ClientData* type
  auto clientData = static_cast<ClientData*>(arg);

  // Detach the thread
  pthread_detach(pthread_self());

  // Process the login message to determine the client type
  Message loginMessage;
  if (clientData->conn->receive(loginMessage)) {
    // Determine if sender or receiver
    if (loginMessage.tag == TAG_SLOGIN) {
      clientData->isSender = true;
      clientData->conn->send(Message(TAG_OK, "Logged in as sender"));
      handle_sender(clientData->conn, clientData->server, clientData->username);
    } else if (loginMessage.tag == TAG_RLOGIN) {
      clientData->isSender = false;
      clientData->conn->send(Message(TAG_OK, "Logged in as receiver"));
      handle_receiver(clientData->conn, clientData->server, clientData->username);
    } else {
      // Handle invalid login tag
      clientData->conn->send(Message(TAG_ERR, "Invalid login tag"));
    }
  } else {
    // Handle error or EOF from connection receive
  }

  // Cleanup
  delete clientData;
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
