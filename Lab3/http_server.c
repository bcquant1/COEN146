//  Created by Behnam Dezfouli
//  CSEN, Santa Clara University
//

// This program implements a web server
//
// The input arguments are as follows:
// argv[1]: Sever's port number


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h> // to get file size


#define COMMAND_BUFF    15000       // Size of the buffer used to store the result of command execution
#define SOCK_READ_BUFF  4096        // Size of the buffer used to store the bytes read over socket
#define REPLY_BUFF      20000       // Size of the buffer used to store the message sent to client
#define FILE_READ       10

#define HTML_FILE       "index.html"


// Socket descriptors
int        socket_fd = 0;            // socket descriptor
int        n_socket_fd = 0;        // new connection descriptor


void  INThandler(int sig)
{
    char  input;
    
    signal(sig, SIG_IGN);
    printf("Did you hit Ctrl-C?\n"
           "Do you want to quit? [y/n] ");
    
    input = getchar();    
    input = getchar();
    
    if (input == 'y' || input == 'Y')
    {
        close (n_socket_fd);
        close (socket_fd);
        exit(0);
    }
    else
    {
        // Reset handler to catch SIGINT next time
        signal(SIGINT, INThandler);
        printf ("Resuming program...\n");
    }
}


// main function ---------------
int main (int argc, char *argv[])
{
    
    // Register a function to handle SIGINT ( SIGNINT is interrupt the process )
    signal(SIGINT, INThandler);
    
    
    int        net_bytes_read;                // numer of bytes received over socket
    struct     sockaddr_in serv_addr;         // Address format structure
    char       net_buff[SOCK_READ_BUFF];      // buffer to hold characters read from socket
    char       message_buff[REPLY_BUFF];      // buffer to hold the message to be sent to the client
    
    char       file_buff[FILE_READ];          // to hold the bytes read from source file
    FILE       *source_file;                  // pointer to the source file
    
    
    // pointer to the file that will include the received bytes over socket
    FILE  *dest_file;
    
    
    if (argc < 2) // Note: the name of the program is counted as an argument
    {
        printf ("Port number not specified!\n");
        return 1;
    }
    
    
    
    // STUDENT WORK
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));       
    
    
    // Create socket
    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf ("Error: Could not create socket! \n");
        return 1;
    }
    
    // To prevent "Address in use" error
    // The SO_REUSEADDR socket option, which explicitly allows a process to bind to a port which remains in TIME_WAIT
    // STUDENT WORK
    int enable;
    if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR, &enable, sizeof(int)) < 0){
	printf("Setsockopt(SO_REUSEADDR) failed\n");
	return 1;
    }
    
    // bind it to all interfaces, on the specified port number
    // STUDENT WORK
    if (bind(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
	printf("Error binding socket\n");
	return 1;
	
    }
    
    // Accept up to 1 connections
    if (listen(socket_fd, 1) < 0)
    {
        perror("Listen failed!");
        exit(EXIT_FAILURE);
    }
    
    printf ("Listening to incoming connections... \n");
    
    
    unsigned int option = 0; // Variable to hold user option
    printf("1: System network configuration \n2: Regular HTTP server\n");
    scanf("%u", &option);
    
    // The web server returns current processor and memory utilization
    if ( option == 1 )
    {
        printf("System network configuration (using ifconfig)... \n");
    }
    // The requested file is returned
    else if (option == 2)
    {
        printf("Regular server (only serving html files)... \n\n");
    }
    else
        printf ("Invalid Option! \n\n");
    
    
    while (1)
    {
        // Accept incoming connection request from client
        // STUDENT WORK
        int n_socket_fd = accept(socket_fd, (struct sockaddr*) NULL, NULL);
        if (n_socket_fd < 0){
            printf("Error establishing connection\n");
            return 1;
        }
        
        printf ("Incoming connection: Accepted! \n");
        
        memset (net_buff, '\0', sizeof (net_buff));
        
        // Return system utilization info
        if ( option == 1 )
        {
            // run a command -- we run ifconfig here (you can try other commands as well)
            FILE *system_info = popen("ifconfig", "r");
            char command_input[] = "ifconfig";
            char command_output[COMMAND_BUFF];
            char line[COMMAND_BUFF];
            if (system_info){
                while (fgets (line, sizeof(line), system_info)){
                    strncpy(command_output + strlen(command_output), line, strlen(line));
                }
            }
            
            strncpy(message_buff, "HTTP/1.1 200 Ok\n", strlen("HTTP/1.1 200 Ok\n"));
            strncpy(message_buff + strlen(message_buff), "Server: SCU COEN Web Server\n", strlen("Server: SCU COEN Web Server\n"));
            strncpy(message_buff + strlen(message_buff), "Content-Type: text/plain\n", strlen("Content-Type: text/plain\n"));
            strncpy(message_buff + strlen(message_buff), "Content-length: ", strlen("Content-length: "));

            char content_length[10] = {'0'};
            sprintf(content_length, "%d\n", strlen(command_output));
            strncpy(message_buff + strlen(message_buff), content_length, strlen(content_length));

            strncpy(message_buff + strlen(message_buff), "\n", strlen("\n"));
            //printf("message_buff: %s \n ", message_buff);

            net_bytes_read = write(n_socket_fd, message_buff, strlen(message_buff));
            if (net_bytes_read < 0){
                printf ("Error writing header\n");
                return 1;
            }
            
            net_bytes_read = write(n_socket_fd, command_output, strlen(command_output));
            if (net_bytes_read < 0){
                printf("Error writing body\n");
                return 1;
            }
            
            // STUDENT WORK
            shutdown (n_socket_fd, SHUT_RDWR);
            close (n_socket_fd);
        }
        
        else if (option == 2)
        {
            // To get the size of html file
            struct stat sbuf;      /* file status */
            
            // make sure the file exists
            // HTML_FILE is index.html and is statically defined
            if (stat(HTML_FILE, &sbuf) < 0) {
                // STUDENT WORK
                shutdown(n_socket_fd, SHUT_RDWR);
                close(n_socket_fd);
            }
        
            //open the source file
            FILE *source_file;
            char line[REPLY_BUFF];
            int pack_size;
            source_file = fopen(HTML_FILE,"r");
             
            if (source_file == NULL)    // STUDENT WORK
            {
                printf ("Error: could not open the source file!\n");
                
                // STUDENT WORK
                fclose(source_file);
                shutdown(n_socket_fd, SHUT_RDWR);
                close (n_socket_fd);
                return 1;
            }
            
            else
            {
                memset(file_buff, '\0', sizeof(file_buff));
                while (pack_size = fread(file_buff,1,FILE_READ, source_file), pack_size > 0){
                    strncpy(line+strlen(line),file_buff,pack_size);
                }
		 
                
            // STUDENT WORK
                strncpy(message_buff, "HTTP/1.1 200 Ok\n", strlen("HTTP/1.1 200 Ok\n"));
                strncpy(message_buff + strlen(message_buff), "Server: SCU COEN Web Server\n", strlen("Server: SCU COEN Web Server\n"));
                strncpy(message_buff + strlen(message_buff), "Content-Type: text/html\n", strlen("Content-Type: text/html\n"));
                strncpy(message_buff + strlen(message_buff), "Content-length: ", strlen("Content-length: "));

                char content_length[10] = {'0'};
                sprintf(content_length, "%d\n", strlen(line));
                strncpy(message_buff + strlen(message_buff), content_length, strlen(content_length));
                strncpy(message_buff + strlen(message_buff), "\n", strlen("\n"));
	
                net_bytes_read = write(n_socket_fd, message_buff, strlen(message_buff));
                if (net_bytes_read < 0){
                    printf ("Error writing header\n");
                    fclose (n_socket_fd);
                    return 1;
                }
            
                net_bytes_read = write(n_socket_fd, line, strlen(line));
                if (net_bytes_read < 0){
                    printf("Error writing body\n");
                    return 1;
                }
                printf("Reply sent to the client!\n");
            }
            shutdown (n_socket_fd, SHUT_RDWR);
            close (n_socket_fd);
        }
    }
    close (socket_fd);
}
