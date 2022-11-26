#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "../RequestBody/MessageModel.cpp"

#pragma once

using namespace std;

#define COLUMN 5
#define NUMBEROFROWS 5

class File
{

private:
    int messageCount;
    string filename;
    string pathname;
    vector<string> row;
    vector<MessageModel *> content;
    fstream file;

public:
    File(string filename)
    {
        cout << "DEBUG: File constructor" << endl;
        this->messageCount = this->content.size();
        this->filename = filename;
        this->pathname = "mail-spool-directory/" + this->filename;
    }

    vector<MessageModel *> getContent()
    {
        return this->content;
    }

    void updateFileVector() // gibt Daten aus dem File in einen 2D Vector
    {
        string line, word;
        int numberOfLines = 0;

        string sender, receiver, subject, message, id;

        //fstream file( /*"mail-spool-directory" +*/ this->pathname, ios::in);
        //debug
        fstream file(this->pathname, ios::in);
        if (file.is_open())
        {
            while (getline(file, line))
            {
                row.clear();
                stringstream str(line);

                getline(str, sender, ',');
                getline(str, receiver, ',');
                getline(str, subject, ',');
                getline(str, message, ',');

                MessageModel *messageModel = new MessageModel(sender, receiver, subject, message, numberOfLines);
                content.push_back(messageModel);
                numberOfLines++;
            }
        }
        else
        {
            perror("Could not open file");
        }
        file.close();

        this->messageCount = numberOfLines;
    }

    int getMessageCount()
    {
        return this->messageCount;
    }

    void addEntry(string sender, string receiver, string subject, string message) // appends an entry
    {
        cout << "Add Entry" << endl;
        file.open(this->pathname, std::ios::app);
        file << sender << ",";
        file << receiver << ",";
        file << subject << ",";
        file << message << ",";
        file << this->messageCount << "\n";
        file.close();
        updateFileVector();
    }

    void addDummyData()
    {

        file.open(this->pathname, std::ios::app);
        for (int i = 0; i < 5; i++)
        {
            file << "Sender"
                 << ",";
            file << "Receiver"
                 << ",";
            file << "Subject"
                 << ",";
            file << "Message"
                 << ",";
            file << i << "\n";
        }
        file.close();
    }

    void deleteEntry(int index)
    {
        updateFileVector();
        cout << "Deleted Message: " << this->content[index]->getSubject() << endl;
        content.erase(this->content.begin() + index);
        rewriteFile();
        file.close();
    }

    void rewriteFile() // file bekommt Vector Einträge, falls sich etwas geändert hat -> delete zum Beispiel
    {                  // nür für die Nachrichten Tabelle
        file.open(this->pathname, std::ios::out);
        for (int i = 0; i < (int)this->content.size(); i++)
        {
            file << content[i]->getSender() << ",";
            file << content[i]->getSender() << ",";
            file << content[i]->getSubject() << ",";
            file << content[i]->getMessage() << ",";
            file << content[i]->getId();
            file << "\n";
        }
        file.close();
    }

    void printFile()
    {
        updateFileVector();
        cout << "Content SIZE: " << this->messageCount << endl;
        for (int i = 0; i < this->messageCount; i++)
        {
            cout << this->content[i]->getSender() << ",";
            cout << this->content[i]->getReceiver() << ",";
            cout << this->content[i]->getSubject() << ",";
            cout << this->content[i]->getMessage() << ",";
            cout << this->content[i]->getId() << ",";
            cout << "\n";
        }
    }
};
