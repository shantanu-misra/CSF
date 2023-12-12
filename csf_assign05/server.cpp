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
void handle_sender(Connection* conn, Server* server, const std::string username, Message *message) {
  User * newUser = new User(username.substr(0, username.length()-1));
  conn->send(*message);
  Room* room = nullptr;
  while (1) { // Loop until disconnection or quit message
    conn->receive(*message);
    if (message->tag == TAG_SENDALL) {
      //Checks if message is done correctly
      if(message->data.length() > Message::MAX_LEN || message->data.empty() || message->data.at(message->data.length()-1) != '\n') {
        conn->send(Message(TAG_ERR, "Message is not in right format\n"));
      }
      //sends message out
      else{
        room->broadcast_message(newUser->username, message->data);
        conn->send(Message(TAG_OK, "Message was sent succesfully\n"));
      }
    } else if (message->tag == TAG_JOIN) {
        //Joins room or creats one and sends message
        room = server->find_or_create_room(message->data.substr(0, message->data.length()-1));
        conn->send(Message(TAG_OK, "Joined room\n"));
    } else if (message->tag == TAG_LEAVE) {
      //leaves room
      conn->send(Message(TAG_OK, "Left was succesful\n"));
    } else if (message->tag  == TAG_QUIT) {
      conn->send(Message(TAG_OK, "Goodbye\n"));
      break; // Exit loop on quit message
    } else {
      conn->send(Message(TAG_ERR, "Unknown tag\n"));
    }
  }
  delete(newUser);
  delete(room);
  delete(conn);
  delete(message);
  // Cleanup before exiting
}

// handle_receiver function manages messages for receiver clients
void handle_receiver(Connection* conn, Server* server, const std::string username, Message *message) {
  User* user = new User(username.substr(0, username.length()-1)); // This needs proper user management to avoid memory leaks
  Room *room;
  conn->send(*message);
  conn->receive(*message);
  if(message->tag == TAG_JOIN) {
    room = server->find_or_create_room(message->data.substr(0, message->data.length() - 1)); // Room management needs proper synchronization
    room->add_member(user);
  }
  message->tag = TAG_OK;
  message->data = "Joined the room\n";
  conn->send(*message);
  //Message* msg;
  while ((message = user->mqueue.dequeue()) != nullptr) { // Loop until disconnection
    if (!conn->send(*message)) {
      break; // Exit loop on send failure
    }
    delete message; // Free message after sending
  }
  // Cleanup before exiting
   //room->remove_member(user);
}

void *worker(void *arg) {
  // Detach the thread
  pthread_detach(pthread_self());
  ClientData *currClient = (ClientData*)arg;
  // Process the login message to determine the client type
  Message *loginMessage = new Message();
  if (currClient->conn->receive(*loginMessage)) {
    // Determine if sender or receiver
    if (loginMessage->tag == TAG_SLOGIN) {
      handle_sender(currClient->conn, currClient->server, loginMessage->data, new Message(TAG_OK, "Logged in as sender\n"));
    } else if (loginMessage->tag == TAG_RLOGIN) {
      handle_receiver(currClient->conn, currClient->server, loginMessage->data, new Message(TAG_OK, "Logged in as receiver\n"));
    } else {
     //Handle invalid login tag
      currClient->conn->send(Message(TAG_ERR, "Invalid login tag\n"));
    }
  } 
  // Cleanup
  delete(loginMessage);
  delete currClient;
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
  return m_ssock >= 0;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  while (true) {
    int clientfd = Accept(m_ssock, nullptr, nullptr);
    Connection* conn = new Connection(clientfd);
    ClientData* clientData = new ClientData(conn, this);
    pthread_t tid;
    if(pthread_create(&tid, nullptr, worker, clientData) != 0) {
      std::cerr<<"Error creating thread\n";
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  {
    Guard guard(m_lock); // Locking to ensure thread safety
    int found = m_rooms.count(room_name);
    if (found) {
      //returns found room
      return m_rooms.at(room_name);
    } else {
      //create new room if it does not exit
      Room *newRoom = new Room(room_name);
      m_rooms.insert({room_name, newRoom});
      return newRoom;
    }
  }
}