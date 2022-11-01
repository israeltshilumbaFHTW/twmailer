#include <iostream>

class ListBody {

    public:
        ListBody(std::string username) {
            this->username = username;
        }

        std::string getUsername() {
            return this->username;
        }
    private:
        std::string username;
};