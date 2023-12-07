#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // TODO: initialize the mutex
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // TODO: destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // TODO: add User to the room
  Guard guard(lock); // Lock the mutex to protect the members set
  members.insert(user); // Add the user to the room
}

void Room::remove_member(User *user) {
  // TODO: remove User from the room
  Guard guard(lock); // Lock the mutex to protect the members set
  members.erase(user); // Remove the user from the room
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  Guard guard(lock); // Lock the mutex before iterating over members

  // Iterate over all members and enqueue the message to their message queue
  for (User* user : members) {
    if (user->username != sender_username) { // Don't send the message to the sender
      // Create the message to be broadcasted on the heap
      Message* message = new Message(TAG_DELIVERY, sender_username + ":" + message_text);
      user->mqueue.enqueue(message);
    }
  }
}
