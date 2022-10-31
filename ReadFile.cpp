#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class ReadFile 
{

private:
    string filename;
    vector<string> row;
    vector<vector<string>> content;

public:
    ReadFile(string filename)
    {
        this->filename = filename;
    }

    void openFile()
    {
        string line, word;
        fstream file( /*"mail-spool-directory" +*/ this->filename, ios::in);

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
            }
        }
        else
        {
            perror("Could not open file");
        }
        file.close();
    }

    void printFile()
    {
        for (int i = 0; i < content.size(); i++)
        {
            for (int j = 0; j < content[i].size(); j++)
            {
                cout << content[i][j] << " ";
            }
            cout << "\n";
        }
    }
};