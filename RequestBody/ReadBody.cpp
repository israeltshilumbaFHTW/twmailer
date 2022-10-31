#include <iostream>

class ReadBody {
    public:
        ReadBody(std::string username, int messageNumber) {
            this->username = username;
            this->messageNumber = messageNumber;
        }

    private:
        std::string username;
        int messageNumber;
};