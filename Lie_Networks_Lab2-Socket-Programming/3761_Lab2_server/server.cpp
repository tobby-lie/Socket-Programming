#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <typeinfo>
#include <fstream>
#include <sstream>
#include <ctype.h>
#define MYPORT 3490 // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 100 // max size of data

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

/*
   Function: file_exists

   Description: checks to see if a file exists

   Parameters: const std::string& name - filename to check

   Pre-Conditions: None

   Post-Conditions: None

   Returns: bool
*/
bool file_exists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
} // end file_exists

/*
   Function: convert_to_string

   Description: Given character pointer with provided size
    convert it to a strings

   Parameters: char* a - character pointer to be converted
    int size - size of char buffer

   Pre-Conditions: None

   Post-Conditions: char converted to string

   Returns: std::string
*/
std::string convert_to_string(char* a, int size)
{
    std::string s = ""; // string to carry full message
    for (int i = 0; i < size; i++)
    {
        s = s + a[i]; // add each character to s
    } // end for
    return s;
} // end convert_to_string

/*
   Function: split_string

   Description: Given a string, a vector of each word is returned.

   Parameters: std::string str - string to be split.

   Pre-Conditions: None

   Post-Conditions: String is split into vector.

   Returns: std::vector<std::string>
*/
std::vector<std::string> split_string(std::string str)
{
    std::istringstream ss(str); // string stream contains str
    std::vector<std::string> vec; // vector to hold words
    
    do
    {
        std::string word;
        ss >> word; // ss input to word
        
        vec.push_back(word); // put word into vector
    }
    while (ss); // keep going until ss is empty
    
    return vec;
} // end split_string

/*
   Function: handle_connection

   Description: Function provided to pthread in order
    to handle incoming connections from multiple clients.

   Parameters: void* p_new_fd - new incoming file descriptor
    from client

   Pre-Conditions: Should have new connection from client.

   Post-Conditions: Client will disconnect when finished
    closing the socket.

   Returns: None
*/
void* handle_connection(void* p_new_fd)
{
    int numbytes; // number of bytes returned from recv
    char status[MAXDATASIZE]; // holds status of response from client
    
    int new_fd = *((int*)p_new_fd); // must convert to int
    free(p_new_fd); // don't need this anymore, don't want to forget to deallocate
    int reader = 0; // number of bytes returned from recv
    
    std::vector<std::string> compare_vec; // holds split up message string
    std::string compare_string = ""; // holds string returned from recv buffer
    char buff[1000]; // buffer to hold recv buffer
    
    // while receiving from new_fd
    while (reader = recv(new_fd, buff, MAXDATASIZE-1, 0) > 0)
    {
        int prefix_length = (char)buff[0]; // prefix to understand length of message
        prefix_length = prefix_length - '0'; // get int representation of prefix character
        
        compare_string = convert_to_string(buff, strlen(buff)); // convert buff to a string
        
        if (compare_string == "") // if compare_string empty continue to beginning of loop
        {
            continue;
        } // end if
        
        compare_vec = split_string(compare_string); // split string and put into vector
        std::string request_string = ""; // holds message of request
        
        // concatenate compare_vec elements to create request_string
        for (int i = 1; i < compare_vec.size(); i++)
        {
            request_string += compare_vec[i];
            request_string += " ";
        } // end for
        
        // display request from client
        std::cout << "* server: received request -> " << request_string << std::endl << std::endl;
        
        // if request is ls
        if (compare_vec[1] == "ls")
        {
            FILE *fpipe; // file pointer
            std::string command = "ls"; // command requested is ls
            char c = 0; // holds characters of file one by one
            std::string cstr = ""; // will hold full string or response
            
            //get command into file pointer
            if (0 == (fpipe = (FILE*)popen(command.c_str(), "r")))
            {
                perror("popen() failed.");
                exit(1);
            } // end if

            // put characters of response into cstr
            while (fread(&c, sizeof c, 1, fpipe))
            {
                cstr += c;
            } // end while
            
            std::vector<std::string> vec = split_string(cstr); // split string into vector
            int words = vec.size(); // get number of words in message
            write(new_fd, &words, sizeof(int)); // send number of words to client
            
            printf("* server: sending data \n \n");
            
            // loop through all elements of vec and write each word to client
            for (int i = 0; i < words; i++)
            {
                write(new_fd, vec[i].c_str(), MAXDATASIZE-1);
            } // end for
            
            std::cout << "* server: message sent: " << std::endl << cstr << std::endl << std::endl;
            
            pclose(fpipe); // close file pointer
        } // end if
        else if (compare_vec[1] == "pwd") // command is pwd
        {
            FILE *fpipe; // file pointer
            std::string command = "pwd"; // command is pwd
            char c = 0; // holds each character of command
            std::string cstr = ""; // holds full message
            
            // file pointer to command response
            if (0 == (fpipe = (FILE*)popen(command.c_str(), "r")))
            {
                perror("popen() failed.");
                exit(1);
            } // end if

            while (fread(&c, sizeof c, 1, fpipe))
            {
                cstr += c;
            } // end while
            
            printf("* server: sending data \n \n");
            
            if (send(new_fd, cstr.c_str(), MAXDATASIZE-1, 0) == -1) // sending data to client
            {
                perror("send");
            } // end if
                
            std::cout << "* server: message sent: " << std::endl << cstr << std::endl << std::endl;
            
            pclose(fpipe); // close file pointer
        } // end else if
        else if (compare_vec[1] == "bye") // command is bye
        {
            if (send(new_fd, "Internet copy client is down!", 50, 0) == -1) // sending data to client
            {
                perror("send");
            } //  end if
            
            printf("* server: file copy server is down! \n \n");
            
            close(new_fd); // close socket
        }
        else if (compare_vec[1] == "download") // command is download
        {
            if (file_exists(compare_vec[2])) // file exists
            {
                
                if (send(new_fd, "1", MAXDATASIZE-1, 0) == -1) // sending data to client
                {
                    perror("send");
                } // end if

                FILE *f; // file pointer

                char str_buffer[100]; // holds buffer of message
                f = fopen(compare_vec[2].c_str(), "r"); // open source file specified in read mode
                
                int lines = 0; // count number of lines in str_buffer
                while(fgets(str_buffer, 100, f) != NULL)
                {
                    lines++;
                } // end while
                
                write(new_fd, &lines, sizeof(int)); // write number of lines to client
                rewind(f); // rewind file pointer
                
                // while there is still data to transmit, write to client
                while(fgets(str_buffer, 100, f) != NULL)
                {
                    write(new_fd, str_buffer, 512);
                } // end while

                fclose(f); // close file pointer
                
                printf("* server: The file has been successfully sent! \n \n");
            } // end if
            else
            {
                // no message to send, file does not exist
                if (send(new_fd, "0", MAXDATASIZE-1, 0) == -1) // sending data to client
                {
                    perror("send");
                } // end if
                printf("* server: The file does not exist! \n \n");
            } // end else
        }
        else if (compare_vec[1] == "upload") // command is upload
        {
            // first recieve status from client about existence of file
            if ((numbytes=recv(new_fd, status, 1, 0)) == -1)
            { // receiving data from client
                perror("recv");
                exit(1);
            } // end if
            
            std::string str_status = convert_to_string(status, 1); // convert status to a string
            
            if (str_status == "0") // file doesn't exist
            {
                printf("* server: File does not exist on client for upload! \n \n");
            } // end if
            else if (str_status == "1") // file exists
            {
                char buffer[512]; // buffer to hold message
                bool f_exists = false; // bool if file exists

                FILE *fp; // file pointer
                
                // check to see if file currently exists in server directory
                if (file_exists(compare_vec[3].c_str()))
                {
                    f_exists = true;
                } // end if
                else
                {
                    f_exists = false;
                } // end else

                int ch = 0; // compared against number of words
                fp = fopen(compare_vec[3].c_str(), "w"); // w flag set to overwrite in cases where file exists
                
                int words = 0; // number of words
                read(new_fd, &words, sizeof(int)); // read number of words from client

                // while number of words has not been reached yet
                while(ch != words)
                {
                    read(new_fd, buffer, 512); // read message buffer
                    fprintf(fp, "%s", buffer); // write to file pointer
                    ch++;
                } // end while
                
                // if file exists it will be overwritten
                if (f_exists)
                {
                    printf("* server: %s has been overwritten! \n \n", compare_vec[3].c_str());
                } // end if
                
                fclose(fp); // close file pointer

                printf("* server: File has been received successfully! \n \n");
            } // end else if
        } // end if
    } // end while

    // client disconnected
    if (reader == 0)
    {
        printf("* server: client disconnected \n \n");
        fflush(stdout);
    } // end if
    // error
    else if (reader == -1)
    {
        perror("recv");
        exit(1);
    } // end else if
} // end handle_connection

int main(void)
{
    int sockfd, new_fd;  // listen on sockfd, new connection on new_fd
    struct sockaddr_in my_addr; // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    
    // IPv4 family, TCP type, 0 protocol
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // get the socket file
    {
        perror("socket");
        exit(1);
    }
    
    printf("* server: creating socket file \n \n");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }
    
    my_addr.sin_family = AF_INET;        // host byte order -> IPv4
    my_addr.sin_port = htons(MYPORT);    // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
    
    // bind socket to host machine ip address and port number
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1)
    {
        perror("bind");
        exit(1);
        
    }
    printf("* server: binding the socket file to the local machine \n \n");
    
    // unique to TCP servers, pass sockfd from socket made above an dput into passive
    // state wait for connections rather than initiate a connection
    // Can only accpet up to 10 connections at the same time -> BACKLOG = 10
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("* server: listening for incomming connections \n \n");
    
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    // main accept() loop
    while((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, \
    &sin_size)))
    {
        printf("* server: got connection from %s \n \n", inet_ntoa(their_addr.sin_addr));
        
        sin_size = sizeof their_addr;
        
        pthread_t t;
        int *pclient = (int*)malloc(sizeof(int));
        *pclient = new_fd;
        
        // pass new connection to new thread with handle_connection function
        if (pthread_create(&t, NULL, &handle_connection, pclient) < 0)
        {
            perror("Thread_Create: pthread_create");
        } // end if
        printf("* server: handler assigned \n \n");
        
        if (new_fd < 0)
        {
            perror("accept");
        } // end if
    }
    return 0;
}
