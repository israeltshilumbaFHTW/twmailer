#include <iostream>
#include "ReadFile.cpp"
#include "WriteFile.cpp"
//test

int main() {
    ReadFile *file = new ReadFile("test.csv");
    file -> openFile();
    file ->printFile();

    WriteFile *writefile = new WriteFile("test.csv");
    writefile -> addEntry("4", "add", "gernot");

    

    return EXIT_SUCCESS;
}