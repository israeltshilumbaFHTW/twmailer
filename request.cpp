#define CLIENT_REQUEST_H
#define MAX_BUF_LENGTH 642

#include <iostream>
#include <vector>
#include <cstring>
#include <regex>
#include <termios.h>
#include <unistd.h>

void getUserInput(std::string &str, std::string name, int length) {
    std::cout << name;
    std::cin >> str;
    fflush(stdout);
}

char *request_send() {
    char* myarray[642];
    std::string final, sender, recipient, subject, message;

    //TODO - check the input!!!!

    getUserInput(sender, "Sender: ", 8);
    getUserInput(recipient, "Recipient: ", 8);

    std::cout << "Subject: ";
    std::getline(std::cin >> std::ws, subject);

    final.append("SEND\n");
    final.append(sender);
    final.append("\n");
    final.append(recipient);
    final.append("\n");
    final.append(subject);

    std::cout << "Message: ";

    while (getline(std::cin, message)) {
        if (message == ".") {
            break;
        }
        final += "\n" + message;
    }

    strcpy(*myarray, final.c_str());

    return *myarray;
}