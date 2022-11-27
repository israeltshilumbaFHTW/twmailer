#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include "mypw.h"
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////

#define BUF 1024
#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
   int create_socket;
   char buffer[BUF];
   struct sockaddr_in address;
   int size;
   int isQuit;

   ////////////////////////////////////////////////////////////////////////////
   // CREATE A SOCKET
   // https://man7.org/linux/man-pages/man2/socket.2.html
   // https://man7.org/linux/man-pages/man7/ip.7.html
   // https://man7.org/linux/man-pages/man7/tcp.7.html
   // IPv4, TCP (connection oriented), IP (same as server)
   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("Socket error");
      return EXIT_FAILURE;
   }

   ////////////////////////////////////////////////////////////////////////////
   // INIT ADDRESS
   // Attention: network byte order => big endian
   memset(&address, 0, sizeof(address)); // init storage with 0
   address.sin_family = AF_INET;         // IPv4
   // https://man7.org/linux/man-pages/man3/htons.3.html
   address.sin_port = htons(PORT);
   // https://man7.org/linux/man-pages/man3/inet_aton.3.html
   if (argc < 2)
   {
      inet_aton("127.0.0.1", &address.sin_addr);
   }
   else
   {
      inet_aton(argv[1], &address.sin_addr);
   }

   ////////////////////////////////////////////////////////////////////////////
   // CREATE A CONNECTION
   // https://man7.org/linux/man-pages/man2/connect.2.html
   if (connect(create_socket,
               (struct sockaddr *)&address,
               sizeof(address)) == -1)
   {
      // https://man7.org/linux/man-pages/man3/perror.3.html
      perror("Connect error - no server available");
      return EXIT_FAILURE;
   }

   // ignore return value of printf
   printf("Connection with server (%s) established\n",
          inet_ntoa(address.sin_addr));

   ////////////////////////////////////////////////////////////////////////////
   // RECEIVE DATA
   // https://man7.org/linux/man-pages/man2/recv.2.html
   size = recv(create_socket, buffer, BUF - 1, 0);
   if (size == -1)
   {
      perror("recv error");
   }
   else if (size == 0)
   {
      printf("Server closed remote socket\n"); // ignore error
   }
   else
   {
      buffer[size] = '\0';
      printf("%s", buffer); // ignore error
   }

   do
   {
      //char message[BUF];
      int userRequestType;
      std::string sender;
      std::cout << "0. Login 1.Send 2.Read 3.Delete 4.Quit" << std::endl;
      std::cin >> userRequestType;
      
      switch (userRequestType)
      {
         case 0: {
            std::cout << "Username:" << std::endl;
            std::cin >> sender;
            std::string login = "login\n";
            std::cout << "Password: \n";
            char password[256];
            //strcpy(password, getPass());
            std::cin >> password;
            login = login + sender + "\n" + password + "\n.";
            strcpy(buffer, login.c_str());

            break;
         }

         case 1:{

            std::string sendMessage = "send\n";
            std::cout << "Receiver\n";
            std::string receiver;
            std::cin >> receiver;

            std::cout << "Subject\n";
            std::string subject;
            std::cin >> subject;

            std::cout << "Message\n";
            std::string message;
            std::cin >> message;

            sendMessage = sendMessage + sender + "\n" + subject + "\n" + message + "\n..";
            //strcpy ???
            strcat(buffer, sendMessage.c_str());
            break;
         }

         case 2: {

            std::string listMessage = "list\n";
            listMessage = listMessage + sender + "\n.";
            //strcpy ???
            strcat(buffer, listMessage.c_str());
            break;
         }

         case 3: {

            std::string readMessage = "read\n";
            std::string messageNumber;
            std::cin >> messageNumber;
            readMessage = readMessage + messageNumber + "\n.";
            //strcpy ???
            strcat(buffer, readMessage.c_str());
            break;
         }

         case 4: {

            std::string delMessage = "del\n";
            std::string messageNumber;
            std::cin >> messageNumber;
            delMessage = delMessage + sender + "\n" + messageNumber + "\n.";
            //strcpy ???
            strcat(buffer, delMessage.c_str());
            break;

         }
         case 5:
            strcpy(buffer, "quit");
            break;
         
         default:
            break;
      }
      int size = strlen(buffer);

      // remove new-line signs from string at the end
      // if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n')
      // {
      //    size -= 2;
      //    buffer[size] = 0;
      // }
      // else if (buffer[size - 1] == '\n')
      // {
      //    --size;
      //    buffer[size] = 0;
      // }
      isQuit = strcmp(buffer, "quit") == 0;

      //////////////////////////////////////////////////////////////////////
      // SEND DATA
      // https://man7.org/linux/man-pages/man2/send.2.html
      // send will fail if connection is closed, but does not set
      // the error of send, but still the count of bytes sent
      if ((send(create_socket, buffer, size, 0)) == -1) 
      {
         perror("send error");
         break;
      }

      size = recv(create_socket, buffer, BUF - 1, 0);
      if (size == -1)
      {
         perror("recv error");
         break;
      }
      else if (size == 0)
      {
         printf("Server closed remote socket\n"); // ignore error
         break;
      }
      else
      {
         buffer[size] = '\0';
         printf("<< %s\n", buffer); // ignore error
/*
         if (strcmp("OK", buffer) != 0)
         {
            fprintf(stderr, "<< Server error occured, abort\n");
            break;
         }

*/
         }
   } while (!isQuit);

   ////////////////////////////////////////////////////////////////////////////
   // CLOSES THE DESCRIPTOR
   if (create_socket != -1)
   {
      if (shutdown(create_socket, SHUT_RDWR) == -1)
      {
         // invalid in case the server is gone already
         perror("shutdown create_socket"); 
      }
      if (close(create_socket) == -1)
      {
         perror("close create_socket");
      }
      create_socket = -1;
   }

   return EXIT_SUCCESS;
}
