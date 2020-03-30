#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#define PORT 3490 // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

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
   Function: return_char_count

   Description: Given a string, return the number of characters
    in string.

   Parameters: std::string str -  string to be counted for chars

   Pre-Conditions: Input string exists and is non-empty

   Post-Conditions: characters counted

   Returns: int
*/
int return_char_count(std::string str)
{
    int count = 0;
    // go through each character of str
    for (auto x: str)
    {
        count++;
    } // end for
    return count;
} // end return_char_count

/*
   Function: menu

   Description: Returns menu selections from user

   Parameters: None

   Pre-Conditions: None

   Post-Conditions: Selection made

   Returns: std::string
*/
std::string menu()
{
    // list of options displayed to user
    std::cout << "***********************************************" << std::endl;
    std::cout << "*               Command options:              *" << std::endl;
    std::cout << "***********************************************" << std::endl;
    std::cout << "* catalog                                     *" << std::endl;
    std::cout << "* spwd                                        *" << std::endl;
    std::cout << "* download [source-filename] [dest-filename]  *" << std::endl;
    std::cout << "* upload [source-filename] [dest-filename]    *" << std::endl;
    std::cout << "* bye                                         *" << std::endl;
    std::cout << "***********************************************" << std::endl;
    
    std::string command = ""; // holds command
    std::cout << ">"; // output for input line
    std::getline(std::cin, command); // utilize getline to take in input with possible spaces
    
    std::vector<std::string> split_command = split_string(command); // split up string words
    int command_char_count = return_char_count(command); // count number of characters to be sent
    
    std::cout << "***********************************************" << std::endl;
    
    std::string message = ""; // holds message to be returned for multi argument message
    if (split_command[0] == "catalog") // catalog
    {
        return "2 ls"; // send ls as message with prefix for message length
    } // end if
    else if (split_command[0] == "spwd") // spwd
    {
         return "3 pwd"; // return pwd with prefix
    } // end else if
    else if (split_command[0] == "download") // download
    {
        if (split_command.size() != 4) // check for prefix, command, source and dest
        {
            return "-1";
        } // end if
        
        // convert char count to string
        std::string return_string = std::to_string(command_char_count);
        return_string.append(" "); // add space
        return_string.append(command); // add command
    
        return return_string;
    } // end else if
    else if (split_command[0] == "upload") // upload
    {
        if (split_command.size() != 4) // check for prefix, command, source and dest
        {
            return "-1";
        } // end if
        
        // convert char count to string
        std::string return_string = std::to_string(command_char_count);
        return_string.append(" "); // add space
        return_string.append(command); // add command
    
        return return_string;
    } // end else if
    else if (split_command[0] == "bye") // bye
    {
        return "3 bye"; // return bye with prefix
    } // end else if
    else
    {
        return "-1";
    } // end else
} // end menu

int main(int argc, char *argv[])
{
    int sockfd, numbytes; // listen on sockfd, numbytes holds returned number of bytes from socket calls
    char buf[MAXDATASIZE]; // holds buffer for messages
    char status[MAXDATASIZE]; // holds status of messages from server
    struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information
    
    if (argc != 2) // check for hostname
    {
        fprintf(stderr,"* usage: client hostname \n \n");
        exit(1);
    } // end if
    
    if ((he=gethostbyname(argv[1])) == NULL) // get host by name provided
    {  // get the host info
        herror("gethostbyname");
        exit(1);
    } // end if
    printf("* client: get the server info \n \n");
    
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) // create socket
    {   // get the socket file
        perror("socket");
        exit(1);
    } // end if
    printf("* client: creating socket file \n \n");
    
    their_addr.sin_family = AF_INET;    // host byte order
    their_addr.sin_port = htons(PORT);  // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr); // store server ip addess (from the
                                                           // gethostbyname function)
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
    
    // Used by TCP client, try to send connection request to server that is listening
    // need to know server address to make this connection
    // also need to know port number server is listening on
    if (connect(sockfd, (struct sockaddr *)&their_addr,     // connect to server
                sizeof their_addr) == -1)
    {
        perror("connect");
        exit(1);
    } // end if
    
    while(1) // infinite loop to request to server
    {
        std::string message = menu(); // get menu response from user
        
        std::vector<std::string> split_message = split_string(message); // holds words of message in vector
        
        if(split_message[0] == "-1") // invalid input
        {
            printf("*client: Invalid input please try again! \n \n");
            continue; // go back to beginning of loop
        } // end if
        
        if (send(sockfd, message.c_str(), MAXDATASIZE-1, 0) == -1) // send request to server
        {
            perror("send");
            exit(1);
        } // end if
        
        std::string message_str = ""; // form message by putting vector words together
        for (int i = 1; i < split_message.size(); i++)
        {
            message_str += split_message[i]; // add word
            
            // check if next element is less than vector size and also less than
            // vector size - 1
            if (i + 1 < split_message.size() && i + 1 < split_message.size() - 1)
            {
                message_str += " ";
            } // end if
            
        } // end for
        
        std::cout << "* client: sent '" << message_str << "' command to the server" << std::endl << std::endl;
        
        if (message == "3 bye") // bye
        {
            std::cout << "* client: internet copy client is down!" << std::endl << std::endl;
            close(sockfd); // close socket
            exit(0);
        } // end if
        if(split_message[1] == "ls") // ls
        {
            int words = 0;
            
            // read response from server
            if(numbytes=read(sockfd, &words, sizeof(int)) == -1)
            {
                perror("read");
            }// end if

            char buffer[MAXDATASIZE-1]; // buffer for message
            std::vector<std::string> message_buf; // vector buffer for message
            std::string temp_copy = ""; // temp string to hold buffer for each read
            
            // go through each word
            for (int i = 0; i < words; i++)
            {
                // read each word into buffer
                if(read(sockfd, buffer, MAXDATASIZE-1) == -1)
                {
                    perror("read");
                } // end if
                
                temp_copy = buffer;
                message_buf.push_back(temp_copy); // add word to message_buf vector
            }
            // output the contents of received message
            std::cout << "* client received: " << std::endl;
            for (int i = 0; i < message_buf.size();i++)
            {
                std::cout << message_buf[i] << std::endl;
            } // end for
        } // end if
        if(split_message[1] == "pwd") // pwd
        {
            // receive message from server
            if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
            { // receiving data from server
                perror("recv");
                exit(1);
            } // end if
            
            // output contents of received message
            buf[numbytes] = '\0';
            std::cout << "* client received: " << std::endl;
            std::cout << buf << std::endl;
        } // end if
        else if(split_message[1] == "download")
        {
            // receive status of download request from server
            if ((numbytes=recv(sockfd, status, MAXDATASIZE-1, 0)) == -1)
            { // receiving data from server
                perror("recv");
                exit(1);
            } // end if
            
            std::string str_status = convert_to_string(status, 1); // convert status to string
            
            if (str_status == "0") // if status is 0, means file doesn't exist
            {
                printf("* client: File does not exist on server for download! \n \n");
            } // end if
            else if (str_status == "1") // if status is 1, means file exists
            {
                char buffer[512]; // message  buffer
                bool f_exists = false; // does the file exist?

                FILE *fp; // file pointer
                
                // check to see if file currently exists in client directory
                if (file_exists(split_message[3].c_str()))
                {
                    f_exists = true;
                }
                else
                {
                    f_exists = false;
                }

                int ch = 0;
                fp = fopen(split_message[3].c_str(), "w"); // w flag set to overwrite in cases where file exists
                int words = 0; // number of words in message
                
                read(sockfd, &words, sizeof(int)); // read in number of words

                while(ch != words) // keep looping until number of words is read in
                {
                    read(sockfd, buffer, 512);
                    fprintf(fp, "%s", buffer); // write to file
                    ch++;
                } // end while
                
                if (f_exists) // if file exists write that it has been overwritten
                {
                    printf("* client: %s has been overwritten! \n \n", split_message[3].c_str());
                } // end if
                
                fclose(fp); // close file pointer

                printf("* client: File has been received successfully! \n \n");
            } // end else if
        } // end else if
        else if(split_message[1] == "upload") // upload
        {
            if (file_exists(split_message[2])) // check if file exists
            {
                if (send(sockfd, "1", 1, 0) == -1) // send data to server
                {
                    perror("send");
                } // end if
                
                char buffer[512]; // buffer for message
                
                FILE *f; // file pointer
                
                char c; //  holds for each character for message
                char str_buffer[100]; // holds message 100 characters at a time
                f = fopen(split_message[2].c_str(), "r"); // open file in read mode
                
                int lines = 0; // reads number of lines in message
                while(fgets(str_buffer, 100, f) != NULL)
                {
                    lines++;
                } // end while
                
                write(sockfd, &lines, sizeof(int)); // write number of lines in message
                rewind(f); // rewind file pointer
                
                // for every 100 characters of string write to sockfd
                while(fgets(str_buffer, 100, f) != NULL)
                {
                    write(sockfd, str_buffer, 512);
                } // end while
                
                fclose(f); // close file pointer
                
                printf("* client: The file has been successfully sent! \n \n");
            } // end if
            else
            {
                if (send(sockfd, "0", 1, 0) == -1) // sending data to server
                {
                    perror("send");
                } // end if
                printf("* client: The file does not exist! \n \n");
            } // end else
        } // end else if
    }
    printf("* client: closing the socket file \n \n");
    close(sockfd); // closing the socket file (disconnect from server)
    return 0;
}
