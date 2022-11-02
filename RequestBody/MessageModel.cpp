#include <iostream>
#pragma once

class MessageModel //hat alles was in einem File gespeichert wird 
{
private:
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string message;
    int id;

public:
    MessageModel(std::string sender, std::string receiver, std::string subject, std::string message, int id)
    {
        this->sender = sender;
        this->receiver = receiver;
        this->subject = subject;
        this->message = message;
        this->id = id;
    }

    int getId()
    {
        return this->id;
    }
    std::string getSender()
    {
        return this->sender;
    }

    std::string getReceiver()
    {
        return this->receiver;
    }

    std::string getSubject()
    {
        return this->subject;
    }

    std::string getMessage()
    {
        return this->message;
    }
};