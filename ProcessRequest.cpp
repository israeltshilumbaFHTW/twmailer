#include <iostream>
#include <sstream>

class ProcessRequest
{

private:
    std::string requestString;
    std::vector<std::string> requestList;

public:
            ProcessRequest(std::string &requestString) {
                this->requestString = requestString;
            }

    void initRequest()
    {
    }

    void readRequest()
    {
        std::string entry;
        this->requestList = parseRequest(this->requestString, "\\n");
    }

    void printRequestList()
    {
        for (int i = 0; i < (int)this->requestList.size(); i++)
        {
            std::cout << "Entry: " << this->requestList.at(i) << std::endl;
        }
    }

    vector<string> parseRequest(string s, string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        vector<string> res;

        try {

            while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
            {
                token = s.substr(pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back(token);
            }

        res.push_back(s.substr(pos_start));
        res.back().pop_back();

        } catch (out_of_range& e) {
            cerr << "Out of Range exception" << e.what() << endl;
        }

        return res;
    }
    std::vector<string> getRequest() {
        return requestList;
    }

};
