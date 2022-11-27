#include <ldap.h>
#include <sys/types.h>
#include <errno.h>
#include <wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sstream>
#include <termios.h>
#include "FileHandling/File.cpp"
#include "ProcessRequest.cpp"
#include "WriteFile.cpp"
#include "RequestBody/MessageModel.cpp"
#include "RequestBody/ListBody.cpp"

#define METHOD 0
#define RECEIVER 1 