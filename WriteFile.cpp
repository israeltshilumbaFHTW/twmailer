#include <fstream>

using namespace std;

class WriteFile
{
public:
    WriteFile(string filename)
    {
        this->filename = filename;
    }

    void addEntry(string receiver, string message, string id)
    {
        ofstream file;
        file.open(this->filename, std::ios::app);

        file << receiver << ",";
        file << message << ",";
        file << id << "\n";
        file.close();
    }

private:
    string filename;
};