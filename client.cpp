#include "header.h"
///////////////////////////////////////////////////////////////////////////////

#define BUF 1024
#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
   if(argc < 2){
       cerr << "Arguments missing" << endl;
       return EXIT_FAILURE;
   }

   int port = atoi(argv[2]);

   int create_socket;
   char buffer[BUF];
   struct sockaddr_in address;
   int size;
   int isQuit;

   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("Socket error");
      return EXIT_FAILURE;
   }

   // INIT ADDRESS
   memset(&address, 0, sizeof(address)); // init storage with 0
   address.sin_family = AF_INET;         // IPv4
   address.sin_port = htons(port);
   if (argc < 2)
   {
      inet_aton("127.0.0.1", &address.sin_addr);
   }
   else
   {
      inet_aton(argv[1], &address.sin_addr);
   }

   // CREATE A CONNECTION
   if (connect(create_socket,
               (struct sockaddr *)&address,
               sizeof(address)) == -1)
   {
      perror("Connect error - no server available");
      return EXIT_FAILURE;
   }

   // ignore return value of printf
   printf("Connection with server (%s) established\n",
          inet_ntoa(address.sin_addr));

   // RECEIVE DATA
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
      int userRequestType;
      std::string sender;
      char password[256];
      std::cout << "|| 0.Login || 1.Send || 2.List || 3.Read || 4.Delete || 5.Quit ||\n >> ";
      std::cin >> userRequestType;

      if(userRequestType > 5 || userRequestType < 0) {
         std::cout << "Bad Request" << std::endl;
         strcpy(buffer, "");
      } else {

         switch (userRequestType)
         {
            case 0: { //LOGIN
               std::cout << "Username:" << std::endl;
               std::cin >> sender;

               std::string login = "login\n";

               strcpy(password, getpass("Password: "));
               //std::cin >> password;

               login = login + sender + "\n" + password + "\n.";
               strcpy(buffer, login.c_str());

               break;
            }

            case 1:{ //SEND
               std::string sendMessage;

               std::string method = "send\n";
               std::cout << "Receiver\n";
               std::string receiver;
               std::cin >> receiver;

               std::cout << "Subject\n";
               std::string subject(80, '\0');
               std::cin >> subject;

               std::cout << "Message\n";
               std::string message;

               //allow longer messages
               while(true) {
                  std::string line;
                  std::cin >> line;
                  message += line;
                  if(line[line.size() - 1] == '.' ) {
                     break;
                  } else {
                     message += "\n";
                  }
               }

               sendMessage = method + receiver + "\n" + subject + "\n" + message + "\n..";
               strcpy(buffer, sendMessage.c_str());
               break;
            }

            case 2: { //LIST
               std::string listMessage;
               std::string method = "list\n";
               listMessage = method + sender + "\n.";

               strcpy(buffer, listMessage.c_str());
               break;
            }

            case 3: { //READ
               std::string readMessage;
               std::string method = "read\n";

               std::cout << "Message Number:" << std::endl;
               std::string messageNumber;
               std::cin >> messageNumber;

               readMessage = method + messageNumber + "\n";

               strcpy(buffer, readMessage.c_str());
               break;
            }

            case 4: { //DEL
               std::string delMessage;
               std::string method = "del\n";

               std::string messageNumber;
               std::cout << "Type the index of the message you want to delete\n";
               std::cin >> messageNumber;

               delMessage = method + messageNumber + "\n";
               strcpy(buffer, delMessage.c_str());
               break;
            }

            case 5: //QUIT
               strcpy(buffer, "quit");
               break;
            
            default:
               break;
         }
      } 

      int size = strlen(buffer);

      isQuit = strcmp(buffer, "quit") == 0;

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
      }
   } while (!isQuit);

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
