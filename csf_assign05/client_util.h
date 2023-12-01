#ifndef CLIENT_UTIL_H
#define CLIENT_UTIL_H

#include <string>
class Connection;
struct Message;

// this header file is useful for any declarations for functions
// or classes that are used by both clients (the sender and receiver)

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// you can add additional declarations here...

//Send message function and checks for recieves
//Parameters:
//tag: tag of user
//connection: connection of server
//data: rest of input
int send_message(const std::string tag, Connection *connection, std::string data);

//handles command calls
//Parameters:
//connection: connection of server
//input: input command
int handle_command(Connection *connection, std::string input);

//Handles delivery and printing of message
//Parameter: string to print
void handle_delivery(std::string s);

#endif // CLIENT_UTIL_H