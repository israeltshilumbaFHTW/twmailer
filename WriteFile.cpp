#include <fstream>

using namespace std;

class WriteFile
{
public:
    WriteFile(string filename)
    {
        this->filename = filename;
    }

    void addEntry(string message)
    {
        ofstream file;
        file.open(this->filename, std::ios::app);

        file << message << "\n";
        file.close();
    }

private:
    string filename;
};