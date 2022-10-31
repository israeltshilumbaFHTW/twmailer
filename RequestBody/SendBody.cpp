#include <iostream>
class SendBody
{
    public:
        SendBody(std::string sender, std::string receiver, std::string subject, std::string message) {
            this->sender = sender;
            this->receiver = receiver;
            this->subject = subject;
            this->message = message;
        }

        std::string getSender() {
            return this->sender;    
        }

        std::string getReceiver() {
            return this->receiver;    
        }
            
        std::string getSubject() {
            return this->subject;    
        }

        std::string getMessage() {
            return this->message;    
        }
private:
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string message;
};