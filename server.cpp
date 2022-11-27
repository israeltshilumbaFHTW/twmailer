#include "header.h"

#define BUF 1024
#define PORT 6543

///////////////////////////////////////////////////////////////////////////////

int abortRequested = 0;
int create_socket = -1;
int new_socket = -1;
bool REQUESTERROR = false;

//HARDCODED
string USERNAME = "if21b088";

bool ERROR = false;
string ERROR_MESSAGE = "internal Server error";
///////////////////////////////////////////////////////////////////////////////

void *clientCommunication(void *data);
void signalHandler(int sig);
int checkLdapLogin(const char ldapPwd[], const char ldapUser[]);

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    //usage ../server <port>
    if (argc < 2) {
        cerr << "Usage: bin/servier <port>" << endl;
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    int reuseValue = 1;

    if (signal(SIGINT, signalHandler) == SIG_ERR)
    {
        perror("signal can not be registered");
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // CREATE A SOCKET
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error"); // errno set by socket()
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // SET SOCKET OPTIONS
    if (setsockopt(create_socket,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &reuseValue,
                   sizeof(reuseValue)) == -1)
    {
        perror("set socket options - reuseAddr");
        return EXIT_FAILURE;
    }

    if (setsockopt(create_socket,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &reuseValue,
                   sizeof(reuseValue)) == -1)
    {
        perror("set socket options - reusePort");
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // INIT ADDRESS
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    ////////////////////////////////////////////////////////////////////////////
    // ASSIGN AN ADDRESS WITH PORT TO SOCKET
    if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("bind error");
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // ALLOW CONNECTION ESTABLISHING
    // Socket, Backlog (= count of waiting connections allowed)
    if (listen(create_socket, 5) == -1)
    {
        perror("listen error");
        return EXIT_FAILURE;
    }

    while (!abortRequested)
    {
        printf("Waiting for connections...\n");

        addrlen = sizeof(struct sockaddr_in);
        if ((new_socket = accept(create_socket,
                                 (struct sockaddr *)&cliaddress,
                                 &addrlen)) == -1)
        {
            if (abortRequested)
            {
                perror("accept error after aborted");
            }
            else
            {
                perror("accept error");
            }
            break;
        }

        /////////////////////////////////////////////////////////////////////////
        // START CLIENT
        // ignore printf error handling
        pid_t cpid, pid;
        pid = getpid();

        //creating new process, parent process stays in while loop
        if((pid = fork() == 0)) {

            printf("Client connected from %s:%d...\n",
                inet_ntoa(cliaddress.sin_addr),
                ntohs(cliaddress.sin_port));
            clientCommunication(&new_socket); // returnValue can be ignored
            new_socket = -1;
        } else {
            cout << "Server still running... Server ID: " << pid << endl;
        }

        //prevent zombies
        while((cpid = waitpid(-1, NULL, WNOHANG))) {
            if((cpid == -1) && (errno != EINTR)) {
                break;
            }
        }
    }

    // frees the descriptor
    if (create_socket != -1)
    {
        if (shutdown(create_socket, SHUT_RDWR) == -1)
        {
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

void *clientCommunication(void *data)
{
    char buffer[BUF];
    int size;
    int *current_socket = (int *)data;
    bool loggedIn = false;

    ////////////////////////////////////////////////////////////////////////////
    // SEND welcome message
    strcpy(buffer, "Welcome to myserver!\r\nPlease enter your commands...\r\n");
    if (send(*current_socket, buffer, strlen(buffer), 0) == -1)
    {
        perror("send failed");
        return NULL;
    }

    do
    {
        /////////////////////////////////////////////////////////////////////////
        // RECEIVE
        size = recv(*current_socket, buffer, BUF - 1, 0);
        if (size == -1)
        {
            if (abortRequested)
            {
                perror("recv error after aborted");
            }
            else
            {
                perror("recv error");
            }
            break;
        }

        if (size == 0)
        {
            printf("Client closed remote socket\n"); // ignore error
            break;
        }

        // remove ugly debug message, because of the sent newline of client
        if (buffer[size - 2] == '\r' && buffer[size - 1] == '\n')
        {
            size -= 2;
        }
        else if (buffer[size - 1] == '\n')
        {
            --size;
        }

        buffer[size] = '\0';

        std::string buffStr = buffer;
        cout << "Client Message: " << buffStr << endl;

        //Put User Request into Vector
        vector<string> userInfo;
        string line;
        stringstream ss;
        ss << buffer << "\n";
        while (getline(ss, line))       //save us in vector
        {
            userInfo.push_back(line);
        }


        if (userInfo[0] == "login")
        {
            if (checkLdapLogin(userInfo[2].c_str(), userInfo[1].c_str()) != EXIT_FAILURE)
            {
                loggedIn = true;
            }
        }

        if (userInfo[0] != "login" && loggedIn == false)
        {
            std::cout << "You must first login to do that!\n";
            ERROR = true;
            ERROR_MESSAGE = "You must first login to do that!\n";
            //send this message to client
        }

        else if (/*loggedIn ==*/ true)
        {
            if (userInfo[METHOD] == "send" || userInfo[METHOD] == "SEND")
            {
                std::cout << "Send start" << std::endl;

                //if not set, return fail message
                cout << "RECEIVER: " << userInfo[RECEIVER] << endl;
                File *file = new File(userInfo[RECEIVER] + ".csv");
                file->updateFileVector();

                MessageModel *sendBody = new MessageModel(USERNAME, userInfo[RECEIVER], userInfo[2], userInfo[3], file->getMessageCount());
                file->addEntry(sendBody->getSender(), sendBody->getReceiver(), sendBody->getSubject(), sendBody->getMessage());
                cout << "New User Entries" << endl;
                file->printFile();

                delete (sendBody);
                delete (file);
            }

            else if (userInfo[METHOD] == "list" || userInfo[METHOD] == "LIST")
            {
                cout << "List start" << endl;

                File *file = new File(USERNAME + ".csv");
                file->updateFileVector();

                std::vector<MessageModel *> content = file->getContent();

                string sendToClient = "Count: " + content.size();
                sendToClient = sendToClient + "\n";

                cout << "Message COUNT " << file->getMessageCount() << endl;
                for (int i = 0; i < file->getMessageCount(); i++)
                {
                    sendToClient = sendToClient + content[i]->getSubject() + " >> " + content[i]->getSender() + "\n";
                }

                cout << "MESSAGE LIST: " << sendToClient << endl;
                if(content.size() > 0) 
                {
                    if (send(*current_socket, sendToClient.c_str(), strlen(sendToClient.c_str()), 0) == -1)
                    {
                        perror("send answer failed");
                        return NULL;
                    }
                } 
                else 
                {
                    ERROR_MESSAGE = "Empty Message List\n";
                }
            }

            else if (userInfo[METHOD] == "read" || userInfo[METHOD] == "READ")
            {
                cout << "Read Request" << endl;
                File *file = new File(USERNAME + ".csv");
                file->updateFileVector();

                int targetMessageId = stoi(userInfo[1]); //

                //check if message id exists

                std::vector<MessageModel *> content = file->getContent();

                // send

                string sendToClient;
                sendToClient = "\nFrom: " + content[targetMessageId]->getSender() +
                            "\nTo: " + content[targetMessageId]->getReceiver() +
                            "\nSubject: " + content[targetMessageId]->getSubject() +
                            "\nMessage: \n" + content[targetMessageId]->getMessage() + "\n";

                if (send(*current_socket, sendToClient.c_str(), strlen(sendToClient.c_str()), 0) == -1)
                {
                    perror("send answer failed");
                    return NULL;
                }
            }

            else if (userInfo[METHOD] == "del" || userInfo[METHOD] == "DEL")
            {
                File *file = new File(USERNAME + ".csv");
                file->deleteEntry(stoi(userInfo[1]));
                delete (file);
            }
            else if (userInfo[METHOD] == "quit" || userInfo[METHOD] == "QUIT")
            {
                break;
            }
        }  
        if (ERROR) 
        {
            if (send(*current_socket, ERROR_MESSAGE.c_str(), strlen(ERROR_MESSAGE.c_str()), 0) == -1)
            {
                perror("send answer failed");
                return NULL;
            }
        } 
        else 
        {
            if (send(*current_socket, "OK", 3, 0) == -1)
            {
                perror("send answer failed");
                return NULL;
            }
        }
    } while (strcmp(buffer, "quit") != 0 && !abortRequested);

    loggedIn = false;

    // closes/frees the descriptor if not already
    if (*current_socket != -1)
    {
        if (shutdown(*current_socket, SHUT_RDWR) == -1)
        {
            perror("shutdown new_socket");
        }
        if (close(*current_socket) == -1)
        {
            perror("close new_socket");
        }
        *current_socket = -1;
    }

    return NULL;
}

void signalHandler(int sig)
{
    if (sig == SIGINT)
    {
        printf("abort Requested... "); // ignore error
        abortRequested = 1;

        if (new_socket != -1)
        {
            if (shutdown(new_socket, SHUT_RDWR) == -1)
            {
                perror("shutdown new_socket");
            }
            if (close(new_socket) == -1)
            {
                perror("close new_socket");
            }
            new_socket = -1;
        }

        if (create_socket != -1)
        {
            if (shutdown(create_socket, SHUT_RDWR) == -1)
            {
                perror("shutdown create_socket");
            }
            if (close(create_socket) == -1)
            {
                perror("close create_socket");
            }
            create_socket = -1;
        }
    }
    else
    {
        exit(sig);
    }
}

int checkLdapLogin(const char ldapPwd[], const char ldapUser[]) 
{
    // LDAP config
    const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
    const int ldapVersion = LDAP_VERSION2; //change to 3

    //set username
    char ldapBindUser[256];
    sprintf(ldapBindUser, "uid=%s,ou=people,dc=technikum-wien,dc=at", ldapUser);
    printf("user set to: %s\n", ldapBindUser);

    // general
    int rc = 0; // return code

    ////////////////////////////////////////////////////////////////////////////
    // setup LDAP connection
    // https://linux.die.net/man/3/ldap_initialize
    LDAP *ldapHandle;
    rc = ldap_initialize(&ldapHandle, ldapUri);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "ldap_init failed\n");
        return -1;
    }
    printf("connected to LDAP server %s\n", ldapUri);

    ////////////////////////////////////////////////////////////////////////////
    // set verison options
    // https://linux.die.net/man/3/ldap_set_option
    rc = ldap_set_option(
        ldapHandle,
        LDAP_OPT_PROTOCOL_VERSION, // OPTION
        &ldapVersion);             // IN-Value
    if (rc != LDAP_OPT_SUCCESS)
    {
        // https://www.openldap.org/software/man.cgi?query=ldap_err2string&sektion=3&apropos=0&manpath=OpenLDAP+2.4-Release
        fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // start connection secure (initialize TLS)
    // https://linux.die.net/man/3/ldap_start_tls_s
    // int ldap_start_tls_s(LDAP *ld,
    //                      LDAPControl **serverctrls,
    //                      LDAPControl **clientctrls);
    // https://linux.die.net/man/3/ldap
    // https://docs.oracle.com/cd/E19957-01/817-6707/controls.html
    //    The LDAPv3, as documented in RFC 2251 - Lightweight Directory Access
    //    Protocol (v3) (http://www.faqs.org/rfcs/rfc2251.html), allows clients
    //    and servers to use controls as a mechanism for extending an LDAP
    //    operation. A control is a way to specify additional information as
    //    part of a request and a response. For example, a client can send a
    //    control to a server as part of a search request to indicate that the
    //    server should sort the search results before sending the results back
    //    to the client.
    rc = ldap_start_tls_s(
        ldapHandle,
        NULL,
        NULL);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return EXIT_FAILURE;
    }

    ////////////////////////////////////////////////////////////////////////////
    // bind credentials
    // https://linux.die.net/man/3/lber-types
    // SASL (Simple Authentication and Security Layer)
    // https://linux.die.net/man/3/ldap_sasl_bind_s
    // int ldap_sasl_bind_s(
    //       LDAP *ld,
    //       const char *dn,
    //       const char *mechanism,
    //       struct berval *cred,
    //       LDAPControl *sctrls[],
    //       LDAPControl *cctrls[],
    //       struct berval **servercredp);

    BerValue bindCredentials;
    bindCredentials.bv_val = (char *)ldapPwd;
    bindCredentials.bv_len = strlen(ldapPwd);
    BerValue *servercredp; // server's credentials
    rc = ldap_sasl_bind_s(
        ldapHandle,
        ldapBindUser,
        LDAP_SASL_SIMPLE,
        &bindCredentials,
        NULL,
        NULL,
        &servercredp);
    if (rc != LDAP_SUCCESS)
    {
        fprintf(stderr, "LDAP bind error: %s\n", ldap_err2string(rc));
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

