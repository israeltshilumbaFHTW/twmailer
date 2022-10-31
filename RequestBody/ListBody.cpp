#include <iostream>

class ListBody {

    public:
        ListBody(std::string username) {
            this->username = username;
        }
    private:
        std::string username;
};