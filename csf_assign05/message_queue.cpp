#include <cassert>
#include <ctime>
#include <semaphore.h>
#include "csapp.h"
#include "message_queue.h"
#include "guard.h"

MessageQueue::MessageQueue() {
  // TODO: initialize the mutex and the semaphore
  pthread_mutex_init(&m_lock, nullptr);
  sem_init(&m_avail, 0, 0); // Initialized to 0, as the queue starts empty
}

MessageQueue::~MessageQueue() {
  {
    Guard gaurd(m_lock);
    while (!m_messages.empty()) {
      Message* m = m_messages.front();
      m_messages.pop_front();
      free(m);
    }
  }
  // TODO: destroy the mutex and the semaphore
  pthread_mutex_destroy(&m_lock);
  sem_destroy(&m_avail);
}

void MessageQueue::enqueue(Message *msg) {
  // TODO: put the specified message on the queue
  // Lock the mutex to protect the queue
  { 
    Guard guard(m_lock);
      // Put the specified message on the queue
    m_messages.push_back(msg);
  }   
  // Unlock the mutex
  // Notify any thread waiting for a message to be available
  sem_post(&m_avail);
  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post
}

Message *MessageQueue::dequeue() {
  //wait for a new message
  sem_wait(&m_avail);
  Message *msg = nullptr;
  {
    Guard g(m_lock);
    //get the last message from the q
    msg = m_messages.front();
    m_messages.pop_front();
  }

  return msg;
  /*
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 1;
  Message *msg = nullptr;
  if (sem_timedwait(&m_avail, &ts) == -1) {
    return msg;
  }
   sem_wait(&m_avail);
  {
    Guard guard(m_lock);
    msg = m_messages.front();
    m_messages.pop_front();
  }
  return msg;*/
}