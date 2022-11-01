#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;


#define COLUMN 5

class File
{

private:
    int messageCount;
    string filename;
    string pathname;
    vector<string> row;
    vector<vector<string>> content;
    fstream file;

public:
    File(string filename)
    {
        this->messageCount = this->content.size();
        this->filename = filename;
        this->pathname = "mail-spool-directory/" + this->filename;
    }

    vector<vector<string>> getContent() {
        return this->content;
    }

    void openFile()
    {
        string line, word;
        int numberOfLines = 0;

        file.open(this->pathname, ios::in);

        if (file.is_open())
        {
            while (getline(file, line))
            {
                row.clear();
                stringstream str(line);

                while (getline(str, word, ','))
                {
                    row.push_back(word);
                }
                content.push_back(row);
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
    void addEntry(string sender, string receiver, string subject, string message)
    {
        file.open(this->pathname, std::ios::app);
        file << sender << ",";
        file << receiver << ",";
        file << subject << ",";
        file << message << "\n";
        file.close();
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
        cout << "Deleted Message: " << this->content.at(index).at(3) << endl;
        content.erase(this->content.begin() + index);
        rewriteFile();
        file.close();
        openFile();
    }

    void rewriteFile()
    { // nür für die Nachrichten Tabelle
        file.open(this->pathname, std::ios::out);
        for (int i = 0; i < (int)this->content.size(); i++)
        {
            for (int j = 0; j < (int)this->content.at(i).size(); j++)
            {
                if ((int)this->content.at(i).size() - j == 1)
                { // letzter Eintrag bekommt kein Komma
                    file << i;
                }
                else
                {
                    //cout << "DEBUG \n";
                    file << this->content.at(i).at(j) << ",";
                }
            }
            file << "\n";
        }
        file.close();
    }

    void printFile()
    {
        cout << "Content SIZE: " << this->messageCount << endl;
        for (int i = 0; i < this->messageCount; i++)
        {
            for (int j = 0; j < COLUMN; j++)
            {
                cout << content[i][j] << " ";
            }
            cout << "\n";
        }
    }
};
