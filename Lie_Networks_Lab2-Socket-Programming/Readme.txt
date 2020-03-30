*******************************************************
*  Name      :  Tobby Lie        
*  Student ID:  103385744               
*  Class     :  CSCI 3761 - Intro to Networks           
*  Lab#      :  2 - Socket Programming                
*  Due Date  :  April 9, 2020
*******************************************************


                 Read Me


*******************************************************
*  Description of the program
*******************************************************

The program utilizes socket programming in C++ to create
A client-server interaction of two programs.

*******************************************************
*  Source files
*******************************************************

   Lie_Networks_Lab2-Socket-Programming/3761_Lab2_client/client.cpp - 
   Client program. Contains code for client side of
   Socket program. 

   Lie_Networks_Lab2-Socket-Programming/3761_Lab2_server/server.cpp -
   Server program. Contains code for server side of
   Socket program. Is able to handle multiple client
   connections.

*******************************************************
*  Executable files after running makefile
*******************************************************

   Lie_Networks_Lab2-Socket-Programming/3761_Lab2_client/client.o - Executable to run client program.

   Lie_Networks_Lab2-Socket-Programming/3761_Lab2_server/server.o - Executable to run server program
   
*******************************************************
*  Circumstances of programs
*******************************************************

   The program runs successfully.  
   
   The program was developed and tested on csegrid.  It was 
   compiled, run, and tested on csegrid.ucdenver.pvt.

*******************************************************
*  How to build and run the program
*******************************************************

1. Uncompress the homework. The homework file is compressed.  
   To uncompress it use the following command:
 
       % unzip Lie_Networks_Lab2-Socket-Programming.zip

   Now you should see multiple directories with files,
   the structure is as follows:

      Lie_Networks_Lab2-Socket-Programming - top level directory

         - makefile - top level makefile to run client and server makefiles
         - Readme.txt - Readme file

         3761_Lab2_client - directory for client

            - client.cpp - client source code
            - makefile - makefile for client

         3761_Lab2_server - directory for server

            - server.cpp - server source code
            - makefile - makefile for server

2. Build the program.

    Make sure you're in the top level directory -- Lie_Networks_Lab2-Socket-Programming
    Compile the program by (this will run makefiles for both client and server):

    % make

3. Run the client by (can run multiple clients by ssh in multiple windows):

   % cd Lie_Networks_Lab2-Socket-Programming/3761_Lab2_client
   % ./client

   Run the server by:

   % cd Lie_Networks_Lab2-Socket-Programming/3761_Lab2_server
   % ./server

4. Delete the obj files, executables, and core dump by:

   First make sure you are in the top level directory -- Lie_Networks_Lab2-Socket-Programming
   % make clean