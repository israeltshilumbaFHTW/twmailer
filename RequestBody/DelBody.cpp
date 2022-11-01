#include <iostream>

class DelBody {
    public:
        DelBody(std::string username, int messageNumber) {
            this->username = username;
            this->messageNumber = messageNumber;
        }
    
    private:
        std::string username;
        int messageNumber;
};