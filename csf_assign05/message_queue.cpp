#include <cassert>
#include <ctime>
#include "message_queue.h"

MessageQueue::MessageQueue() {
  // TODO: initialize the mutex and the semaphore
  pthread_mutex_init(&m_lock, nullptr);
  sem_init(&m_avail, 0, 0); // Initialized to 0, as the queue starts empty
}

MessageQueue::~MessageQueue() {
  // TODO: destroy the mutex and the semaphore
  pthread_mutex_destroy(&m_lock);
  sem_destroy(&m_avail);
}

void MessageQueue::enqueue(Message *msg) {
  // TODO: put the specified message on the queue

  // Lock the mutex to protect the queue
  pthread_mutex_lock(&m_lock);
  // Put the specified message on the queue
  m_messages.push_back(msg);
  // Unlock the mutex
  pthread_mutex_unlock(&m_lock);
  // Notify any thread waiting for a message to be available
  sem_post(&m_avail);

  // be sure to notify any thread waiting for a message to be
  // available by calling sem_post
}

Message *MessageQueue::dequeue() {
  struct timespec ts;

  // get the current time using clock_gettime:
  // we don't check the return value because the only reason
  // this call would fail is if we specify a clock that doesn't
  // exist
  clock_gettime(CLOCK_REALTIME, &ts);

  // compute a time one second in the future
  ts.tv_sec += 1;

  // TODO: call sem_timedwait to wait up to 1 second for a message

  //       to be available, return nullptr if no message is available
  // Wait up to 1 second for a message to be available
  if (sem_timedwait(&m_avail, &ts) == -1) {
    // If no message is available, return nullptr
    return nullptr;
  }
  // Lock the mutex to protect the queue
  pthread_mutex_lock(&m_lock);


  // TODO: remove the next message from the queue, return it

  // Remove the next message from the queue
  assert(!m_messages.empty()); // Sanity check to make sure the queue isn't empty
  Message *msg = m_messages.front();
  m_messages.pop_front();

  // Unlock the mutex
  pthread_mutex_unlock(&m_lock);

  return msg;
}
