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

  ClientData(Connection* conn, Server* server)
    : conn(conn)
    , server(server) {}
};



////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {

// handle_sender function processes messages from the sender clients
void handle_sender(Connection* conn, Server* server, const std::string& username) {
  Message msg;
  while (conn->receive(msg)) { // Loop until disconnection or quit message
    if (msg.tag == TAG_SENDALL) {
      Room* room = server->find_or_create_room("general");
      room->broadcast_message(username, msg.data);
      conn->send(Message(TAG_OK, "Message sent\n"));
    } else if (msg.tag == TAG_JOIN) {
      // Handle room joining logic
    } else if (msg.tag == TAG_LEAVE) {
      // Handle room leaving logic
    } else if (msg.tag == TAG_QUIT) {
      conn->send(Message(TAG_OK, "Goodbye\n"));
      break; // Exit loop on quit message
    } else {
      conn->send(Message(TAG_ERR, "Unknown command\n"));
    }
  }
  // Cleanup before exiting
}

// handle_receiver function manages messages for receiver clients
void handle_receiver(Connection* conn, Server* server, const std::string& username) {
  User* user = new User(username); // This needs proper user management to avoid memory leaks
  Room* room = server->find_or_create_room("general"); // Room management needs proper synchronization
  room->add_member(user);

  Message* msg;
  while ((msg = user->mqueue.dequeue()) != nullptr) { // Loop until disconnection
    if (!conn->send(*msg)) {
      break; // Exit loop on send failure
    }
    delete msg; // Free message after sending
  }
  // Cleanup before exiting
  room->remove_member(user);
  delete user;
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
      clientData->conn->send(Message(TAG_OK, "Logged in as sender"));
      handle_sender(clientData->conn, clientData->server, loginMessage.data);
    } else if (loginMessage.tag == TAG_RLOGIN) {
      clientData->conn->send(Message(TAG_OK, "Logged in as receiver"));
      handle_receiver(clientData->conn, clientData->server, loginMessage.data);
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
      ClientData* clientData = new ClientData(conn, this);
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
