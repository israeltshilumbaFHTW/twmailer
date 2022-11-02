#include <iostream>
#include "FileHandling/File.cpp"
#include "RequestBody/MessageModel.cpp"

//test

int main() {

    File *file = new File("test.csv");
    file->deleteEntry(2);

    return EXIT_SUCCESS;
}